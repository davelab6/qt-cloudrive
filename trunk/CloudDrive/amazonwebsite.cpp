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

#include <QtGlobal>
#include <qjson/src/parser.h>
#include "amazonwebsite.h"
#include "utils.h"
#include "javascriptparser.h"

AmazonWebsite::AmazonWebsite(): QObject()
{    
    signedIn = false;
    qsrand(QDateTime::currentDateTime().toTime_t());
    networkAccessManager.setCookieJar(new QNetworkCookieJar(this));    
    connect(&networkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

    driveServer = "/clouddrive/api/";    
    driveSerialNum = "191-5955500-0636011";
    downloadQueue = NULL;
    uploadQueue = NULL;
}

bool AmazonWebsite::signIn(QString email, QString password)
{
    this->email = email;
    this->password = password;
    return loadSignInPage();
}

bool AmazonWebsite::loadSignInPage()
{
#ifdef QT_NO_OPENSSL
#error SSL IS REQUIRED
#endif

    QUrl signInPageUrl = QUrl(
                "https://www.amazon.com/ap/signin?"
                "_encoding=UTF8&"
                "openid.assoc_handle=usflex&"
                "openid.return_to=https://www.amazon.com/clouddrive?_encoding=UTF8%26ref_=sa_menu_acd_urc2&"
                "openid.mode=checkid_setup&"
                "openid.ns=http://specs.openid.net/auth/2.0&"
                "openid.claimed_id=http://specs.openid.net/auth/2.0/identifier_select&"
                "openid.pape.max_auth_age=900&"
                "openid.ns.pape=http://specs.openid.net/extensions/pape/1.0&"
                "openid.identity=http://specs.openid.net/auth/2.0/identifier_select");

    QNetworkRequest httpGetRequest(signInPageUrl);
    httpGetRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    httpGetRequest.setRawHeader(QByteArray("Accept"),
        QByteArray("application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5"));
    httpGetRequest.setRawHeader(QByteArray("Accept-Charset"), QByteArray("ISO-8859-1,utf-8;q=0.7,*;q=0.3"));
    httpGetRequest.setRawHeader(QByteArray("Accept-Language"), QByteArray("en-US,en;q=0.8"));
    httpGetRequest.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    httpGetRequest.setRawHeader(QByteArray("Connection"), QByteArray("keep-alive"));
    httpGetRequest.setRawHeader(QByteArray("Host"), QByteArray("www.amazon.com"));
    httpGetRequest.setRawHeader(QByteArray("Origin"), QByteArray("https://www.amazon.com"));
    httpGetRequest.setRawHeader(QByteArray("User-Agent"), QByteArray(USER_AGENT));

    QNetworkReply* reply = networkAccessManager.get(httpGetRequest);
    return connect(reply, SIGNAL(finished()), this, SLOT(loadSignInPageFinished()));
}

void AmazonWebsite::loadSignInPageFinished()
{
    QNetworkReply *networkReply = ((QNetworkReply *)sender());
    if (!networkReply->error())
    {                
        networkReply->deleteLater();

        const char *filterElements[] = {"form", "input", NULL};
        htmlParser.setElementsFilter(filterElements);
        connect(&htmlParser,
                SIGNAL(onHtmlElement(QString, QList<QHtmlAttribute *>)),
                this,
                SLOT(onSignInPageHtmlElement(QString, QList<QHtmlAttribute *>)));
        signInFormParams.clear();
        htmlParser.parseHtmlPage(networkReply);
        disconnect(this, SLOT(onSignInPageHtmlElement(QString, QList<QHtmlAttribute *>)));
        sendCreditentials();
    }
    else
    {        
        qDebug() <<  networkReply->errorString();
        networkReply->deleteLater();
    }    
}

void AmazonWebsite::onSignInPageHtmlElement(QString elementName, QList<QHtmlAttribute *> elementAttributes)
{
    QList<QHtmlAttribute *>::const_iterator iter;
    if (elementName.compare(QString("input"), Qt::CaseInsensitive) == 0)
    {
        QString name;
        QString value;
        QString type;
        for (iter = elementAttributes.begin(); iter != elementAttributes.end(); iter++)
        {
            QString attribName = (*iter)->first;
            if (attribName.compare(QString("name"), Qt::CaseInsensitive) == 0)
            {
                name = (*iter)->second;
            }
            else if (attribName.compare(QString("value"), Qt::CaseInsensitive) == 0)
            {
                value = (*iter)->second;
            }
            else if (attribName.compare(QString("type"), Qt::CaseInsensitive) == 0)
            {
                type = (*iter)->second;
            }
        }
        if (type.compare(QString("hidden"), Qt::CaseInsensitive) == 0)
        {
            signInFormParams.append(SignInFormNV(name, value));
        }
    }
    else if (elementName.compare(QString("form"), Qt::CaseInsensitive) == 0)
    {
        if (loginAction.isEmpty())
        {
            QString currAction;

            for (iter = elementAttributes.begin(); iter != elementAttributes.end(); iter++)
            {
                QString attribName = (*iter)->first;
                if (attribName.compare(QString("action"), Qt::CaseInsensitive) == 0)
                {
                    currAction = (*iter)->second;
                    if (currAction.startsWith("http"))
                    {
                        loginAction = currAction;
                        break;
                    }
                }
            }
        }
    }
}

bool AmazonWebsite::sendCreditentials()
{
#ifdef QT_NO_OPENSSL
#error SSL IS REQUIRED
#endif

    QUrl loginActionUrl = QUrl(loginAction);
    QByteArray signInRefererPageUrl = QByteArray(
                "https://www.amazon.com/ap/signin?_encoding=UTF8&"
                "openid.assoc_handle=usflex&"
                "openid.return_to=https%3A%2F%2Fwww.amazon.com%2Fclouddrive%3F_encoding%3DUTF8%26ref_%3Dsa_menu_acd_urc2&"
                "openid.mode=checkid_setup&"
                "openid.ns=http%3A%2F%2Fspecs.openid.net%2Fauth%2F2.0&"
                "openid.claimed_id=http%3A%2F%2Fspecs.openid.net%2Fauth%2F2.0%2Fidentifier_select&"
                "openid.pape.max_auth_age=900&"
                "openid.ns.pape=http%3A%2F%2Fspecs.openid.net%2Fextensions%2Fpape%2F1.0&"
                "openid.identity=http%3A%2F%2Fspecs.openid.net%2Fauth%2F2.0%2Fidentifier_select");

    QNetworkRequest httpPostRequest(loginActionUrl);
    httpPostRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    httpPostRequest.setRawHeader(QByteArray("Accept"),
        QByteArray("application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5"));
    httpPostRequest.setRawHeader(QByteArray("Accept-Charset"), QByteArray("ISO-8859-1,utf-8;q=0.7,*;q=0.3"));
    httpPostRequest.setRawHeader(QByteArray("Accept-Language"), QByteArray("en-US,en;q=0.8"));
    httpPostRequest.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    httpPostRequest.setRawHeader(QByteArray("Connection"), QByteArray("keep-alive"));
    httpPostRequest.setRawHeader(QByteArray("Host"), QByteArray("www.amazon.com"));
    httpPostRequest.setRawHeader(QByteArray("Origin"), QByteArray("https://www.amazon.com"));    
    httpPostRequest.setRawHeader(QByteArray("Referer"), signInRefererPageUrl);
    httpPostRequest.setRawHeader(QByteArray("User-Agent"), QByteArray(USER_AGENT));
    httpPostRequest.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/x-www-form-urlencoded"));

    QUrl params;
    params.addEncodedQueryItem(QByteArray("action"), QByteArray(urlEncode(loginAction).toAscii()));
    QListIterator<SignInFormNV> formParamIter(signInFormParams);
    while (formParamIter.hasNext())
    {
        SignInFormNV param = formParamIter.next();
        params.addEncodedQueryItem(
                    QByteArray(param.first.toAscii()),
                    QByteArray(urlEncode(param.second).toAscii()));
    }

    params.addEncodedQueryItem(QByteArray("email"), QByteArray(urlEncode(email).toAscii()));
    params.addEncodedQueryItem(QByteArray("password"), QByteArray(urlEncode(password).toAscii()));
    params.addEncodedQueryItem("create", "0");
    params.addEncodedQueryItem("x", "145");
    params.addEncodedQueryItem("y", "9");

    QNetworkReply *networkReply = networkAccessManager.post(httpPostRequest, params.toEncoded());
    return connect(networkReply, SIGNAL(finished()), this, SLOT(handleSendCreditentialsPage()));
}

void AmazonWebsite::handleSendCreditentialsPage()
{
    QNetworkReply *networkReply = ((QNetworkReply *)sender());
    networkReply->deleteLater();
    if (networkReply->error() == QNetworkReply::NoError)
    {
        if (networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 302)
        {
            QString redirectLocation = networkReply->rawHeader("Location");
            fetchSessionAndCustomerId(redirectLocation);
        }
    }
}

bool AmazonWebsite::fetchSessionAndCustomerId(QString redirectLocation)
{
#ifdef QT_NO_OPENSSL
#error SSL IS REQUIRED
#endif

    QUrl cloudDrivePageUrl = QUrl(redirectLocation);

    QNetworkRequest httpGetRequest(cloudDrivePageUrl);
    httpGetRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    httpGetRequest.setRawHeader(QByteArray("Accept"),
        QByteArray("application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5"));
    httpGetRequest.setRawHeader(QByteArray("Accept-Charset"), QByteArray("ISO-8859-1,utf-8;q=0.7,*;q=0.3"));
    httpGetRequest.setRawHeader(QByteArray("Accept-Language"), QByteArray("en-US,en;q=0.8"));
    httpGetRequest.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    httpGetRequest.setRawHeader(QByteArray("Connection"), QByteArray("keep-alive"));
    httpGetRequest.setRawHeader(QByteArray("Host"), QByteArray("www.amazon.com"));
    httpGetRequest.setRawHeader(QByteArray("Origin"), QByteArray("https://www.amazon.com"));
    httpGetRequest.setRawHeader(QByteArray("User-Agent"), QByteArray(USER_AGENT));

    QNetworkReply *networkReply = networkAccessManager.get(httpGetRequest);
    return connect(networkReply, SIGNAL(finished()),
            this, SLOT(fetchSessionAndCustomerIdFinished()));
}

void AmazonWebsite::createDownloadQueue()
{
    if (downloadQueue != NULL)
    {
        delete downloadQueue;
    }
    signedIn = true;
    downloadQueue = new DownloadQueue(&networkAccessManager, customerId, sessionId, driveSerialNum, driveServer);
    connect(downloadQueue, SIGNAL(onDownloadProgress(const QString &, qint64, qint64)), this, SIGNAL(onDownloadProgress(const QString &, qint64, qint64)));
    connect(downloadQueue,
            SIGNAL(onFileDownloaded(const QString &, qlonglong, const QString &, QIODevice *, const QString &)),
            this,
            SIGNAL(onFileDownloaded(const QString &, qlonglong, const QString &, QIODevice *, const QString &)));
}

void AmazonWebsite::createUploadQueue()
{
    if (uploadQueue != NULL)
    {
        delete uploadQueue;
    }
    signedIn = true;
    uploadQueue = new UploadQueue(&networkAccessManager, customerId, sessionId, driveSerialNum, driveServer);
    connect(uploadQueue, SIGNAL(onUploadProgress(const QString &, qint64, qint64)),
            this, SIGNAL(onUploadProgress(const QString &, qint64, qint64)));
    connect(uploadQueue, SIGNAL(onFileUploaded(const QString &)),
            this, SIGNAL(onFileUploaded(const QString &)));
}

void AmazonWebsite::fetchSessionAndCustomerIdFinished()
{
    QNetworkReply *networkReply = ((QNetworkReply *)sender());
   if (!networkReply->error())
    {
        networkReply->deleteLater();

        const char *filterElements[] = {"input", NULL};
        htmlParser.setElementsFilter(filterElements);
        connect(&htmlParser,
                SIGNAL(onHtmlElement(QString, QList<QHtmlAttribute *>)),
                this,
                SLOT(onCloudDrivePageHtmlElement(QString, QList<QHtmlAttribute *>)));

        customerId = "";
        sessionId = "";

        htmlParser.parseHtmlPage(networkReply);
        disconnect(this, SLOT(onCloudDrivePageHtmlElement(QString, QList<QHtmlAttribute *>)));

        //qDebug() << sessionId << customerId;

        if (!sessionId.isEmpty() && !customerId.isEmpty())
        {            
            createDownloadQueue();
            createUploadQueue();
            emit onUserSignedIn(customerId, sessionId);
        }
    }
    else
    {
        networkReply->deleteLater();
        qDebug() <<  networkReply->errorString();
    }
}

void AmazonWebsite::onCloudDrivePageHtmlElement(
    QString elementName,
    QList<QHtmlAttribute *> elementAttributes)
{
    QList<QHtmlAttribute *>::const_iterator iter;
    if (elementName.compare(QString("input"), Qt::CaseInsensitive) == 0)
    {
        QString id;
        QString value;
        QString type;
        for (iter = elementAttributes.begin(); iter != elementAttributes.end(); iter++)
        {
            QString attribName = (*iter)->first;
            if (attribName.compare(QString("id"), Qt::CaseInsensitive) == 0)
            {
                id = (*iter)->second;
            }
            else if (attribName.compare(QString("value"), Qt::CaseInsensitive) == 0)
            {
                value = (*iter)->second;
            }
            else if (attribName.compare(QString("type"), Qt::CaseInsensitive) == 0)
            {
                type = (*iter)->second;
            }
        }
        if (type.compare(QString("hidden"), Qt::CaseInsensitive) == 0)
        {
            if (id.compare(QString("sessionId")) == 0)
            {
                sessionId = value;
            }
            else if (id.compare(QString("customerId")) == 0)
            {
                customerId = value;
            }
        }
    }
}


bool AmazonWebsite::signOut()
{
    QUrl signOutPageUrl = QUrl(
                "http://www.amazon.com/gp/flex/sign-out.html/ref=pd_ys_nav_signin?"
                "ie=UTF8&"
                "path=/gp/yourstore/home&"
                "signIn=1&"
                "useRedirectOnSuccess=1&"
                "action=sign-out");

    QNetworkRequest httpGetRequest(signOutPageUrl);
    httpGetRequest.setRawHeader(QByteArray("Accept"),
        QByteArray("application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5"));
    httpGetRequest.setRawHeader(QByteArray("Accept-Charset"), QByteArray("ISO-8859-1,utf-8;q=0.7,*;q=0.3"));
    httpGetRequest.setRawHeader(QByteArray("Accept-Language"), QByteArray("en-US,en;q=0.8"));
    httpGetRequest.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    httpGetRequest.setRawHeader(QByteArray("Connection"), QByteArray("keep-alive"));
    httpGetRequest.setRawHeader(QByteArray("Host"), QByteArray("www.amazon.com"));
    httpGetRequest.setRawHeader(QByteArray("Origin"), QByteArray("https://www.amazon.com"));
    httpGetRequest.setRawHeader(QByteArray("User-Agent"), QByteArray(USER_AGENT));


    QEventLoop loop;
    //connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)),
    //        this, SLOT(handleLoadSignOutPage(QNetworkReply*)));
    connect(&networkAccessManager,
            SIGNAL(finished(QNetworkReply*)),
            &loop,
            SLOT(quit()));
    QNetworkReply *networkReply = networkAccessManager.get(httpGetRequest);
    loop.exec();


    if (!networkReply->error())
    {
        if (networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 302)
        {
            qDebug() << "Succesfull logout";
        }
        else
        {
            qDebug() << networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        }
        networkReply->readAll();
        networkReply->deleteLater();
        networkAccessManager.disconnect();
    }
    else
    {
        networkReply->deleteLater();
        qDebug() <<  networkReply->errorString();
    }
    return true;
}

void AmazonWebsite::sslErrors(QNetworkReply* reply,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }
    qDebug() << errorString;
    reply->ignoreSslErrors();
}

bool AmazonWebsite::addFileToDownloadQueue(const QString &sourceFileName,
                                           const QString &sourceFileObjectId,
                                           const QString &destinationDir)
{
    return downloadQueue->addFileToDownloadQueue(sourceFileName, sourceFileObjectId, destinationDir);
}

bool AmazonWebsite::addFileToDownloadQueue(DownloadQueueItem *item)
{
    return downloadQueue->addFileToDownloadQueue(item);
}

bool  AmazonWebsite::addFileToUploadQueue(const QString &localFilePath, const QString &remoteFileDir)
{
    return uploadQueue->addFileToUploadQueue(localFilePath, remoteFileDir);
}

/*

    JSON Operations

*/

JsonOperation* AmazonWebsite::createJsonOperation()
{
    JsonOperation* jsonOp =
            new JsonOperation(&networkAccessManager, driveServer, customerId, sessionId);
    connect(jsonOp, SIGNAL(error(const QString&, const QString&)),
            this, SLOT(error(const QString&, const QString&)));
    return jsonOp;
}

void AmazonWebsite::error(const QString& errorCode, const QString& errorMessage)
{
    qDebug() << errorCode << errorMessage;
    emit jsonOpError(errorCode, errorMessage);
}

bool AmazonWebsite::getInfoByPath(QString path)
{
    QList<JsonApiParams> params;
    params.append(JsonApiParams("path", path));    
    JsonOperation* jsonOp = createJsonOperation();
    connect(jsonOp, SIGNAL(response()), this, SLOT(getInfoByPathResponse()));
    return jsonOp->execute("getInfoByPath", params);
}

void AmazonWebsite::getInfoByPathResponse()
{    
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap result = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        jsonOp->deleteLater();
        qDebug() << "An error occurred during parsing";
    }
    else
    {
        jsonOp->deleteLater();
        QString objectId = result["getInfoByPathResponse"]
                .toMap()["getInfoByPathResult"]
                .toMap()["objectId"].toString();
        emit onGetInfoByPath(objectId);        
    }

}

bool AmazonWebsite::listById(QString objectId,
                             QString ordering, /*type,keyName,creationDate*/
                             int nextToken,
                             int maxItems,
                             QString filter /*type != 'RECYCLE' and status != 'PENDING' and hidden = false*/
                             )
{
    JsonOperation* jsonOp = createJsonOperation();
    QList<JsonApiParams> params;
    params.append(JsonApiParams("objectId", objectId));
    params.append(JsonApiParams("ordering", urlEncode(ordering)));
    params.append(JsonApiParams("nextToken", QString::number(nextToken)));
    params.append(JsonApiParams("maxItems", QString::number(maxItems)));
    params.append(JsonApiParams("filter", urlEncode(filter)));
    connect(jsonOp, SIGNAL(response()), this, SLOT(listByIdResponse()));
    return jsonOp->execute("listById", params);
}

void AmazonWebsite::listByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    emit onListObjects(FileObject::parseFileObjects(jsonOp->getData()));
    jsonOp->deleteLater();
}

QList<FileObject> FileObject::parseFileObjects(QByteArray encodedObjects)
{
    QList<FileObject> result;    
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(encodedObjects, &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
    }
    else
    {
        QVariantList objectList =
                parseResult["listByIdResponse"].toMap()["listByIdResult"].toMap()["objects"].toList();
        QVariantList::const_iterator rIter;
        for (rIter = objectList.begin(); rIter != objectList.end(); rIter++)
        {
            FileObject fileObject;
            QVariantMap objectMap = (*rIter).toMap();
            fileObject.ObjectId = objectMap["objectId"].toString();
            fileObject.ParentObjectId = objectMap["parentObjectId"].toString();
            fileObject.ObjectName = objectMap["name"].toString();
            fileObject.ObjectType = objectMap["type"].toString();
            fileObject.FileSize = objectMap["size"].toInt();
            fileObject.LastModified = objectMap["lastUpdatedDate"].toDateTime();
            result.append(fileObject);
        }
    }
    return result;
}

bool AmazonWebsite::removeBulkById(QList<QString> objectIds)
{    
    JsonOperation* jsonOper = createJsonOperation();
    connect(jsonOper, SIGNAL(onRemoveBulkById(const QString &)),
            this, SLOT(onRemoveBulkById(const QString &)));
    return jsonOper->removeBulkById(objectIds);
}

void AmazonWebsite::onRemoveBulkById(const QString &requestId)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    jsonOp->deleteLater();
    emit onBulkRemovedById();
}

bool AmazonWebsite::recycleBulkById(QList<QString> objectIds)
{
    JsonOperation* jsonOper = createJsonOperation();
    connect(jsonOper, SIGNAL(onRecycleBulkById(const QString &)),
            this, SLOT(onRecycleBulkById(const QString &)));
    return jsonOper->removeBulkById(objectIds);
}

void AmazonWebsite::onRecycleBulkById(const QString &requestId)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    jsonOp->deleteLater();
    emit onBulkRecycledById();
}

bool AmazonWebsite::getDownloadUrlById(const QString &objectId)
{
    JsonOperation *jsonOp = createJsonOperation();
    connect(jsonOp, SIGNAL(onGetDownloadUrlById(const QByteArray &)),
                            this, SLOT(onGetDownloadUrlById(const QByteArray &)));
    return jsonOp->getDownloadUrlById(objectId, driveSerialNum);
}

void AmazonWebsite::onGetDownloadUrlById(const QByteArray &downloadURL)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    jsonOp->deleteLater();
    emit onGotDownloadUrl(downloadURL);
}

bool AmazonWebsite::createFolder(const QString &parentFolder, const QString &folderName)
{
    JsonOperation* jsonOper = createJsonOperation();
    connect(jsonOper, SIGNAL(onCreateByPath(const QString&)),
            this, SLOT(onCreateFolderByPath(const QString&)));
    return jsonOper->createByPath(
                true,
                UPLOAD_CONFLICT_RESOLUTION_RENAME,
                folderName,
                true,
                parentFolder,
                OBJECT_TYPE_FOLDER);
}

void AmazonWebsite::onCreateFolderByPath(const QString& createdObjectId)
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    jsonOp->deleteLater();
    emit onFolderCreated(createdObjectId);
}

bool AmazonWebsite::getUserStorage()
{
    JsonOperation* jsonOp = createJsonOperation();
    QList<JsonApiParams> params;
    connect(jsonOp, SIGNAL(response()), this, SLOT(getUserStorageResponse()));
    return jsonOp->execute("getUserStorage", params);
}

void AmazonWebsite::getUserStorageResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    qlonglong freeSpace;
    qlonglong totalSpace;
    qlonglong usedSpace;
    if (parseStorageInfo(jsonOp->getData(), &freeSpace, &totalSpace, &usedSpace))
    {
        emit onGetUserStorage(freeSpace, totalSpace, usedSpace);
    }
    jsonOp->deleteLater();
}

bool AmazonWebsite::parseStorageInfo(
    QByteArray response,
    qlonglong *freeSpace,
    qlonglong *totalSpace,
    qlonglong *usedSpace)
{
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(response, &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        return false;
    }
    else
    {
        *freeSpace =
                parseResult["getUserStorageResponse"]
                .toMap()["getUserStorageResult"]
                .toMap()["freeSpace"].toLongLong();
        *totalSpace =
                parseResult["getUserStorageResponse"]
                .toMap()["getUserStorageResult"]
                .toMap()["totalSpace"].toLongLong();
        *usedSpace =
                parseResult["getUserStorageResponse"]
                .toMap()["getUserStorageResult"]
                .toMap()["usedSpace"].toLongLong();

        return true;
    }
}
