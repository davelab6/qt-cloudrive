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

#include <QDebug>
#include <QFile>

#include "qjson/src/parser.h"
#include "generalconfig.h"
#include "uploadqueue.h"
#include "clouddriveobject.h"
#include "cloudutils.h"

const QString UploadConflictResRename = "RENAME";
const QString UploadConflictResMerge = "MERGE";

UploadQueueItem::UploadQueueItem(const QString &localFilePath, const QString &remoteFileDir)
{
    this->localFilePath = localFilePath;
    this->remoteFileDir = remoteFileDir;
}

void UploadQueueItem::setObjectId(const QString &objectId)
{
    this->objectId = objectId;
}

QString UploadQueueItem::getObjectId()
{
    return objectId;
}

UploadQueue::UploadQueue(QNetworkAccessManager *networkAccessManager,
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
    uploadInProgress = false;
    formPost = NULL;
}

JsonOperation* UploadQueue::createJsonOperation()
{
    JsonOperation* jsonOp =
            new JsonOperation(networkAccessManager, driveServer, customerId, sessionId);
    connect(jsonOp, SIGNAL(jsonOpError(QString, QString)),
            this, SIGNAL(jsonOpError(QString, QString)));
    return jsonOp;
}

bool UploadQueue::addFileToUploadQueue(const QString &localFilePath, const QString &remoteFileDir)
{
    filesQueue.enqueue(UploadQueueItem(localFilePath, remoteFileDir));
    if (!uploadInProgress)
    {
        processQueueFiles();
    }
    return true;
}

void UploadQueue::processQueueFiles()
{
    if (!filesQueue.isEmpty())
    {
        uploadInProgress = true;
        uploadFile(filesQueue.first());
    }
    else
    {
        uploadInProgress = false;
    }
}

void UploadQueue::dequeueFile()
{
    filesQueue.dequeue();
    processQueueFiles();
}

bool UploadQueue::uploadFile(const UploadQueueItem &queueItem)
{
    JsonOperation *jsonOper = createJsonOperation();
    connect(jsonOper, SIGNAL(onCreateByPath(const QString&)),
            this, SLOT(onCreateByPath(const QString&)));
    return jsonOper->createByPath(
                true,
                UploadConflictResRename,
                Utils::extractFileName(queueItem.localFilePath),
                true,
                queueItem.remoteFileDir,
                ObjectTypeFile);
}

void UploadQueue::onCreateByPath(const QString& createdObjectId)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    filesQueue.first().setObjectId(createdObjectId);
    jsonOp->deleteLater();
    if (!createdObjectId.isEmpty())
    {
        QFile file(filesQueue.first().localFilePath);
        if (file.exists())
        {
            JsonOperation *jsonOper = createJsonOperation();
            connect(jsonOper,
                    SIGNAL(onGetUploadUrlById(
                        const QByteArray &,
                        const QString &,
                        const QByteArray &,
                        const QVariantMap &)),
                    this,
                    SLOT(onGetUploadUrlById(
                        const QByteArray &,
                        const QString &,
                        const QByteArray &,
                        const QVariantMap &)));
            jsonOper->getUploadUrlById("POST", filesQueue.first().getObjectId(), file.size());
        }
    }
    else
    {
        qDebug() << "An error occured while creating object for path.";
    }
}

bool UploadQueue::onGetUploadUrlById(
    const QByteArray &storageKey,
    const QString &endPoint,
    const QByteArray &resourcePath,
    const QVariantMap &uploadParams)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    filesQueue.first().storageKey = storageKey;
    jsonOp->deleteLater();
    return postFile(endPoint, resourcePath, uploadParams, filesQueue.first().localFilePath);
}

bool UploadQueue::postFile(const QString &endPoint,
                               const QByteArray &resourcePath,
                               const QVariantMap &uploadParams,
                               const QString &localFileName)
{
#ifdef QT_NO_OPENSSL
#error SSL IS REQUIRED
#endif

    formPost = new FormPost(UserAgent);
    formPost->setNetworkAccessManager(networkAccessManager);    
    QMapIterator<QString, QVariant> iter(uploadParams);
    while (iter.hasNext())
    {
        iter.next();
        formPost->addField(iter.key(), iter.value().toString());
    }
    formPost->addField("Filename", Utils::extractFileName(localFileName));
    formPost->setFile("file", localFileName, "application/octet-stream");

    QNetworkReply *networkReply = formPost->postData(endPoint);
    connect(
        networkReply, SIGNAL(uploadProgress(qint64, qint64)),
        this, SLOT(uploadProgress(qint64, qint64)));
    return connect(networkReply, SIGNAL(finished()), this, SLOT(postFileFinished()));
}

void UploadQueue::uploadProgress(qint64 bytesUploaded, qint64 totalBytes)
{
    emit onUploadProgress(filesQueue.first().localFilePath, bytesUploaded, totalBytes);
}

void UploadQueue::postFileFinished()
{
    if (formPost != NULL)
    {
        delete formPost;
        formPost = NULL;
    }
    QNetworkReply *networkReply = ((QNetworkReply *)sender());
    if (networkReply->error() == QNetworkReply::NoError)
    {
        JsonOperation *jsonOper = createJsonOperation();
        connect(jsonOper,
                SIGNAL(onCompleteFileUploadById(const QString &)),
                       this,
                SLOT(onCompleteFileUploadById(const QString &)));
        jsonOper->completeFileUploadById(
                    filesQueue.first().getObjectId(),
                    filesQueue.first().storageKey);
    }
    else
    {
        qDebug() << networkReply->errorString();
        qDebug() << QString(networkReply->readAll());
    }
}

void UploadQueue::onCompleteFileUploadById(const QString &requestId)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    jsonOp->deleteLater();
    emit onFileUploaded(filesQueue.first().localFilePath);
    dequeueFile();
}
