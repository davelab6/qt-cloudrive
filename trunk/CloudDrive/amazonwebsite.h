/*
    QT Cloud Drive, desktop application for connecting to Cloud Drive
    Copyright (C) 2011 Vasko Mitanov vasko.mitanov@hotmail.com

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

#ifndef AMAZONWEBSITE_H
#define AMAZONWEBSITE_H

#include <QtNetwork>
#include <QObject>

#include "generalconfig.h"
#include "htmlparser.h"
#include "jsonoperation.h"
#include "downloadqueue.h"
#include "uploadqueue.h"
#include "clouddriveobject.h"

typedef QPair<QString, QString> SignInFormNV;

class AmazonWebsite : public QObject
    {
    Q_OBJECT

public:
    AmazonWebsite();

public:
    bool signIn(QString email, QString password);    
    bool signOut();    
    bool getUserStorage();
    bool downloadFile(const QString &fileName, const QString fileObjectId);
    bool uploadFile(const QString &localFilePath, const QString &remoteFileDir);
    bool addFileToDownloadQueue(const QString &sourceFileName,
                                const QString &sourceFileObjectId,
                                const QString &destinationDir);
    bool addFileToDownloadQueue(DownloadQueueItem *item);
    bool addFileToUploadQueue(const QString &localFilePath, const QString &remoteFileDir);
    bool createFolder(const QString &parentFolder, const QString &folderName);
    bool removeBulkById(const QList<QString>& objectIds);
    bool recycleBulkById(const QList<QString>& objectIds);
    bool getDownloadUrlById(const QString &fileObjectId);
    bool moveById(
        const QString& sourceId,
        const QString& destinationParentId,
        const QString& destinationName,
        bool overwrite);

private:
    JsonOperation* createJsonOperation();    
    bool loadSignInPage();
    bool sendCreditentials();
    void createDownloadQueue();
    void createUploadQueue();
    bool fetchSessionAndCustomerId(QString redirectLocation);
    bool downloadFileFromUrl(const QByteArray &fileUrl);
    bool parseStorageInfo(
        QByteArray response,
        qlonglong *freeSpace,
        qlonglong *totalSpace,
        qlonglong *usedSpace);
    void addExtraHeaders(QNetworkRequest& httpRequest);
    bool parseFileObjects(QByteArray encodedObjects, QList<CloudDriveFileObject>& fileObjects ) const;


private slots:     
     void error(QString errorCode, QString errorMessage);
     void onSignInPageHtmlElement(QString elementName, QList<QHtmlAttribute *> elementAttributes);
     void loadSignInPageFinished();
     void handleSendCreditentialsPage();
     void fetchSessionAndCustomerIdFinished();
     void onCloudDrivePageHtmlElement(QString elementName, QList<QHtmlAttribute *> elementAttributes);     
     void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
     void onCreateFolderByPath(const QString& createdObjectId);
     void onRemoveBulkById(const QString &requestId);
     void onRecycleBulkById(const QString &requestId);
     void onGetDownloadUrlById(const QByteArray &downloadURL);
     void onGetUserStorage(
         qlonglong freeSpace,
         qlonglong totalSpace,
         qlonglong usedSpace);
     void getInfoByPathResponse();
     void listByIdResponse();
     void onMoveByIdResponse(const QString &objectId);

public:
     //JSON operations
     bool getInfoByPath(QString path);
     bool listById(QString objectId, QString ordering, int nextToken, int MaxItems, QString filter);

signals:
    void jsonOpError(QString errorCode, QString errorMessage);
    void onUserSignedIn(const QString &customerId, const QString &sessionId);
    void onGetInfoByPath(const QString &objectId);
    void onListObjects(const QList<CloudDriveFileObject> &objectList);
    void userStorage(qlonglong freeSpace, qlonglong totalSpace, qlonglong usedSpace);
    void onDownloadProgress(const QString &fileName, qint64, qint64);
    void onFileDownloaded(const QString &contentType, qlonglong contentLength,
                          const QString &fileName, QIODevice *inputStream,
                          const QString &fileDestDir);
    void onUploadProgress(const QString &fileName, qint64, qint64);
    void onFileUploaded(const QString &fileName);
    void onFolderCreated(const QString &folderObjectId);
    void onBulkRemovedById();
    void onBulkRecycledById();
    void onGotDownloadUrl(const QByteArray &downloadURL);
    void onMoveById();


private:
    bool signedIn;
    QNetworkAccessManager networkAccessManager;
    HtmlParser htmlParser;

    QString loginAction;
    QList<SignInFormNV> signInFormParams;

    QString email;
    QString password;

    QString customerId;
    QString sessionId;

    QByteArray currentUploadingFileStorageKey;
    QString currentUploadingFile;
    QString currentUploadingFileObjectId;
    QString currentDownloadingFile;
    DownloadQueue *downloadQueue;
    UploadQueue *uploadQueue;
};

#endif // AMAZONWEBSITE_H
