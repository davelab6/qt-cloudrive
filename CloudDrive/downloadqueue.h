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

#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H
#include <QQueue>
#include <QNetworkAccessManager>
#include "jsonoperation.h"

class DownloadQueueItem: public QObject
{
     Q_OBJECT

public:
    DownloadQueueItem(const QString &sourceFileName,
                      const QString &sourceFileObjectId,
                      const QString &destinationDir);

    QString GetSourceFileName() const;
    QString GetSourceFileObjectId() const;
    QString GetDestinationDir() const;
    void EmitOnQueueItemDownloaded();

private:
    QString m_sourceFileName;
    QString m_sourceFileObjectId;
    QString m_destinationDir;

public: signals:
    void onQueueItemDownloaded();
};

class DownloadQueue: public QObject
{

    Q_OBJECT

public:
    DownloadQueue(QNetworkAccessManager *networkAccessManager,
                  QString customerId,
                  QString sessionId,
                  QString driveSerialNum,
                  QString driveServer);

private:
    QNetworkAccessManager *networkAccessManager;
    QString customerId;
    QString sessionId;
    QString driveSerialNum;
    QString driveServer;
    QQueue<DownloadQueueItem *> filesQueue;
    volatile bool downloadInProgress;

private:
    JsonOperation* createJsonOperation();
    bool downloadFile(DownloadQueueItem *downloadQueueItem);
    bool getDownloadUrlById(QString fileObjectId);
    bool downloadFile(const QByteArray &fileUrl);
    void processQueueFiles();
    void dequeueFile();

private slots:
    void downloadProgress(qint64 bytesDownloaded, qint64 totalBytes);    
    void downloadFileFinished();
    void onGetDownloadUrlById(const QByteArray &downloadURL);

public:    
    bool addFileToDownloadQueue(const QString &sourceFileName,
                                const QString &sourceFileObjectId,
                                const QString &destinationDir);
    bool addFileToDownloadQueue(DownloadQueueItem *item);

signals:
    void fileOnQueueAdded();
    void onDownloadProgress(const QString &fileName, qint64 bytesDownloaded, qint64 totalBytes);
    void onFileDownloaded(const QString &contentType, qlonglong contentLength,
                          const QString &fileName, QIODevice *inputStream, const QString &fileDestDir);
    void jsonOpError(QString errorCode, QString errorMessage);

};

#endif // DOWNLOADQUEUE_H
