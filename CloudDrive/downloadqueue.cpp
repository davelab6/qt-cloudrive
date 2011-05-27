/*
    QT Cloud Drive, desktop application for connecting to Cloud Drive
    Copyright (C) 2011 Vasko Mitanov vasko.mitanov@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.    
*/

#include <QVariantMap>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtConcurrentRun>
#include <QFuture>

#include "generalconfig.h"
#include "downloadqueue.h"
#include "cloudutils.h"
#include <qjson/src/parser.h>

DownloadQueueItem::DownloadQueueItem(const QString &sourceFileName,
                                     const QString &sourceFileObjectId,
                                     const QString &destinationDir):
    QObject(0),
    m_sourceFileName(sourceFileName),
    m_sourceFileObjectId(sourceFileObjectId),
    m_destinationDir(destinationDir)
{

}

QString const& DownloadQueueItem::getSourceFileName() const
{
    return m_sourceFileName;
}

QString const& DownloadQueueItem::getSourceFileObjectId() const
{
    return m_sourceFileObjectId;
}

QString const& DownloadQueueItem::getDestinationDir() const
{
    return m_destinationDir;
}

void DownloadQueueItem::emitOnQueueItemDownloaded()
{
    emit onQueueItemDownloaded();
}

DownloadQueue::DownloadQueue(QNetworkAccessManager *networkAccessManager,
                             QString customerId,
                             QString sessionId,
                             QString driveSerialNum,
                             QString driveServer): QObject(0)
{
    this->networkAccessManager = networkAccessManager;
    this->customerId = customerId;
    this->sessionId = sessionId;
    this->driveSerialNum = driveSerialNum;
    this->driveServer = driveServer;
    downloadInProgress = false;
}

JsonOperation* DownloadQueue::createJsonOperation()
{
    JsonOperation* jsonOp =
            new JsonOperation(networkAccessManager, driveServer, customerId, sessionId);
    connect(jsonOp, SIGNAL(jsonOpError(QString, QString)),
            this, SIGNAL(jsonOpError(QString, QString)));
    return jsonOp;
}

bool DownloadQueue::addFileToDownloadQueue(
    const QString &sourceFileName,
    const QString &sourceFileObjectId,
    const QString &destinationDir)
{
    qDebug() << "DownloadQueue::addFileToDownloadQueue: Added file on download queue";
    filesQueue.enqueue(new DownloadQueueItem(sourceFileName, sourceFileObjectId, destinationDir));
    if (!downloadInProgress)
    {
        processQueueFiles();
    }
    return true;
}

bool DownloadQueue::addFileToDownloadQueue(DownloadQueueItem* item)
{
    qDebug() << "DownloadQueue::addFileToDownloadQueue: Added file on download queue";
    filesQueue.enqueue(item);
    if (!downloadInProgress)
    {
        processQueueFiles();
    }
    return true;
}

void DownloadQueue::processQueueFiles()
{    
    if (!filesQueue.isEmpty())
    {
        downloadInProgress = true;
        DownloadQueueItem *item = filesQueue.first();
        downloadFile(item);
    }
    else
    {
        downloadInProgress = false;
    }
}

void DownloadQueue::dequeueFile()
{
    qDebug() << "DownloadQueue::dequeueFile: Removed file from download queue";
    DownloadQueueItem *item = filesQueue.dequeue();
    delete item;
    processQueueFiles();
}

bool DownloadQueue::downloadFile(DownloadQueueItem *downloadQueueItem)
{
    JsonOperation *jsonOp = createJsonOperation();
    connect(jsonOp, SIGNAL(onGetDownloadUrlById(const QByteArray &)),
                            this, SLOT(onGetDownloadUrlById(const QByteArray &)));
    return jsonOp->getDownloadUrlById(
                downloadQueueItem->getSourceFileObjectId(),
                driveSerialNum);
}

void DownloadQueue::onGetDownloadUrlById(const QByteArray &downloadURL)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    jsonOp->deleteLater();
    downloadFile(downloadURL);
}


bool DownloadQueue::downloadFile(const QByteArray &fileUrl)
{
   QUrl downloadUrl;
   downloadUrl.setEncodedUrl(fileUrl, QUrl::StrictMode);
   QNetworkRequest httpGetRequest(downloadUrl);
   httpGetRequest.setRawHeader(QByteArray("Accept"),
       QByteArray("Accept: application/xml,application/xhtml+xml,text/html,text/plain,*/*"));
   httpGetRequest.setRawHeader(QByteArray("Accept-Charset"), QByteArray("ISO-8859-1,utf-8;q=0.7,*;q=0.3"));
   httpGetRequest.setRawHeader(QByteArray("Accept-Language"), QByteArray("en-US,en;q=0.8"));
   httpGetRequest.setRawHeader(QByteArray("Accept-Encoding"), QByteArray("gzip, deflate"));
   httpGetRequest.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
   httpGetRequest.setRawHeader(QByteArray("Connection"), QByteArray("keep-alive"));
   httpGetRequest.setRawHeader(QByteArray("Host"), QByteArray(downloadUrl.host().toLatin1()));
   httpGetRequest.setRawHeader(QByteArray("Origin"), QByteArray("https://www.amazon.com"));
   httpGetRequest.setRawHeader(QByteArray("User-Agent"), UserAgent);

   QNetworkReply *networkReply = networkAccessManager->get(httpGetRequest);
   connect(
       networkReply, SIGNAL(downloadProgress(qint64, qint64)),
       this, SLOT(downloadProgress(qint64, qint64)));

   qDebug() << "DownloadQueue::downloadFile: Download file started";
   return connect(networkReply, SIGNAL(finished()), this, SLOT(downloadFileFinished()));   
}


void DownloadQueue::downloadProgress(qint64 bytesDownloaded, qint64 totalBytes)
{
    emit onDownloadProgress(filesQueue.first()->getSourceFileName(),
                            bytesDownloaded,
                            totalBytes);
}

void DownloadQueue::downloadFileFinished()
{
    qDebug() << "DownloadQueue::downloadFileFinished: Download file finished";
    QNetworkReply *networkReply = ((QNetworkReply *)sender());
    if (!networkReply->error())
    {
        if (networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 200)
        {
            QString contentType = networkReply->header(QNetworkRequest::ContentTypeHeader).toString();
            qlonglong contentLength = networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
            //QString fileName = decodeFileName(networkReply->rawHeader("Content-Disposition"));            
            DownloadQueueItem *item = filesQueue.first();
            emit onFileDownloaded(
                        contentType,
                        contentLength,
                        item->getSourceFileName(),
                        networkReply,
                        item->getDestinationDir());
            item->emitOnQueueItemDownloaded();
            dequeueFile();
        }
        networkReply->deleteLater();

    }
    else
    {
        qDebug() <<  networkReply->errorString();
        networkReply->deleteLater();
    }
}
