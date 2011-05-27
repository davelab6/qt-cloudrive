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

#ifndef UPLOADQUEUE_H
#define UPLOADQUEUE_H

#include <QQueue>
#include <QNetworkAccessManager>
#include <QVariantMap>
#include "jsonoperation.h"
#include "formpost.h"

#define UPLOAD_CONFLICT_RESOLUTION_RENAME "RENAME"
#define UPLOAD_CONFLICT_RESOLUTION_MERGE "MERGE"


class UploadQueueItem
{
public:
    UploadQueueItem(const QString &localFilePath, const QString &remoteFileDir);
    QString localFilePath;
    QString remoteFileDir;
    QByteArray storageKey;

private:
    QString objectId;

public:
    void setObjectId(const QString &objectId);
    QString getObjectId();
};

class UploadQueue: public QObject
{

    Q_OBJECT

public:    
    UploadQueue(QNetworkAccessManager *networkAccessManager,
                  QString customerId,
                  QString sessionId,
                  QString driveSerialNum,
                  QString driveServer);

private:
    QNetworkAccessManager *networkAccessManager;
    FormPost *formPost;
    QString customerId;
    QString sessionId;
    QString driveSerialNum;
    QString driveServer;
    QQueue<UploadQueueItem> filesQueue;
    volatile bool uploadInProgress;

private:
    JsonOperation* createJsonOperation();
    bool uploadFile(const UploadQueueItem &queueItem);
    void processQueueFiles();
    void dequeueFile();        
    bool postFile(const QString &endPoint,
                       const QByteArray &resourcePath,
                       const QVariantMap &uploadParams,
                       const QString &localFileName);


private slots:        
    void postFileFinished();    
    void uploadProgress(qint64 bytesUploaded, qint64 totalBytes);
    void onCreateByPath(const QString& createdObjectId);
    bool onGetUploadUrlById(
        const QByteArray &storageKey,
        const QString &endPoint,
        const QByteArray &resourcePath,
        const QVariantMap &uploadParams);
    void onCompleteFileUploadById(const QString &requestId);


public:
    bool addFileToUploadQueue(const QString &localFilePath, const QString &remoteFileDir);

signals:
    void fileOnQueueAdded();    
    void onUploadProgress(const QString &fileName, qint64 bytesUploaded, qint64 totalBytes);
    void onFileUploaded(const QString &fileName);
    void jsonOpError(QString errorCode, QString errorMessage);

};

#endif // UPLOADQUEUE_H
