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

#include <QDebug>
#include <QNetworkReply>

#include "qjson/src/parser.h"
#include "clouddriveobject.h"
#include "cloudutils.h"
#include "jsonoperation.h"
#include "generalconfig.h"

JsonOperation::JsonOperation(
        QNetworkAccessManager *networkAccessManager,
        const QString &driveServer,        
        const QString &customerId,
        const QString &sessionId): QObject()
{
    this->networkAccessManager = networkAccessManager;
    this->driveServer = driveServer;
    this->customerId = customerId;
    this->sessionId = sessionId;
}

bool JsonOperation::execute(const QString &operationName,
                            const QList<JsonApiParams> &operationParams)
{
   QUrl amazonApiUrl;
   long id = qrand();
   id *= id;
   amazonApiUrl.setUrl("https://www.amazon.com" + driveServer);
   amazonApiUrl.addQueryItem("_", QString::number(id));
   amazonApiUrl.addQueryItem("Operation", operationName);
   amazonApiUrl.addQueryItem("customerId", customerId);
   QList<JsonApiParams>::const_iterator paramIter;
   for (paramIter = operationParams.begin(); paramIter != operationParams.end(); paramIter++)
   {
       amazonApiUrl.addEncodedQueryItem(
                   QByteArray(paramIter->first.toAscii()),
                   QByteArray(paramIter->second.toAscii()));
   }
   amazonApiUrl.addQueryItem("ContentType", "JSON");
   amazonApiUrl.addQueryItem("populatePath", "true");

   QNetworkRequest httpGetRequest(amazonApiUrl);
   httpGetRequest.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/x-www-form-urlencoded"));
   httpGetRequest.setRawHeader(QByteArray("Accept-Charset"), QByteArray("ISO-8859-1,utf-8;q=0.7,*;q=0.3"));
   httpGetRequest.setRawHeader(QByteArray("Accept-Language"), QByteArray("en-US,en;q=0.8"));
   httpGetRequest.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
   httpGetRequest.setRawHeader(QByteArray("Connection"), QByteArray("keep-alive"));
   httpGetRequest.setRawHeader(QByteArray("Host"), QByteArray("www.amazon.com"));
   httpGetRequest.setRawHeader(QByteArray("Origin"), QByteArray("https://www.amazon.com"));
   httpGetRequest.setRawHeader(QByteArray("X-Requested-With"), QByteArray("XMLHttpRequest"));
   httpGetRequest.setRawHeader(QByteArray("x-amzn-SessionId"), QByteArray(sessionId.toAscii()));
   httpGetRequest.setRawHeader(QByteArray("User-Agent"), UserAgent);

   QNetworkReply *networkReply = networkAccessManager->get(httpGetRequest);
   return connect(networkReply, SIGNAL(finished()), this, SLOT(finished()));
}

QByteArray JsonOperation::getData() const
{
    return internalBuff;
}

void JsonOperation::finished()
{    
    QNetworkReply *networkReply = ((QNetworkReply *)sender());
    if (!networkReply->error())
    {
        internalBuff = networkReply->readAll();
        networkReply->deleteLater();
        emit response();
    }
    else
    {        
        internalBuff = networkReply->readAll();
        networkReply->deleteLater();
        //qDebug() << QString(internalBuff);
        QJson::Parser parser;
        bool parseStatus;        
        QString errorCode = "";
        QString errorMessage = "";//QString(internalBuff);
        QVariant parseResult = parser.parse(internalBuff, &parseStatus);
        if (parseStatus)
        {
            QVariantMap resultMap = parseResult.toMap();
            if (resultMap["Error"].isValid())
            {
                errorCode = resultMap["Error"].toMap()["Code"].toString();
                errorMessage = resultMap["Error"].toMap()["Message"].toString();
            }
        }
        internalBuff.clear();
        qDebug() << errorCode << errorMessage;
        emit jsonOpError(errorCode, errorMessage);
    }
}


/*

  JSON Operations

*/

bool JsonOperation::createByPath(
    bool autoparent,
    const QString& conflictResolution,
    const QString& name,
    bool overwrite,
    const QString& path,
    const QString& type)
{
    QList<JsonApiParams> params;
    params.append(JsonApiParams("autoparent", autoparent?"true":"false"));
    params.append(JsonApiParams("conflictResolution", conflictResolution));
    params.append(JsonApiParams("name", Utils::urlEncode(name)));
    params.append(JsonApiParams("overwrite", overwrite?"true":"false"));
    params.append(JsonApiParams("path", Utils::urlEncode(path)));
    params.append(JsonApiParams("type", type /*FILE*/));
    connect(this, SIGNAL(response()), this, SLOT(createByPathResponse()));
    return execute("createByPath", params);
}

void JsonOperation::createByPathResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        emit onCreateByPath(QString());
    }
    else
    {
        emit onCreateByPath(parseResult["createByPathResponse"]
                            .toMap()["createByPathResult"]
                            .toMap()["objectId"].toString());
    }
}

bool JsonOperation::removeBulkById(const QList<QString>& objectIds)
{
    QList<JsonApiParams> params;
    for (int i = 0; i < objectIds.length(); i++)
    {
        params.append(JsonApiParams(QString("inclusionIds.member.%1").arg(i+1), objectIds.at(i)));
    }
    connect(this, SIGNAL(response()), this, SLOT(removeBulkByIdResponse()));
    return execute("removeBulkById", params);
}

void JsonOperation::removeBulkByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        return;
    }
    else
    {
        QString requestId = parseResult["removeBulkByIdResponse"]
                .toMap()["ResponseMetadata"]
                .toMap()["RequestId"].toString();
        if (!requestId.isEmpty())
        {
            onRemoveBulkById(requestId);
        }
    }
}

bool JsonOperation::recycleBulkById(const QList<QString>& objectIds)
{
    QList<JsonApiParams> params;
    for (int i = 0; i < objectIds.length(); i++)
    {
        params.append(JsonApiParams(QString("inclusionIds.member.%1").arg(i+1), objectIds.at(i)));
    }
    connect(this, SIGNAL(response()), this, SLOT(recycleBulkByIdResponse()));
    return execute("recycleBulkById", params);
}

void JsonOperation::recycleBulkByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        return;
    }
    else
    {
        QString requestId = parseResult["recycleBulkByIdResponse"]
                .toMap()["ResponseMetadata"]
                .toMap()["RequestId"].toString();
        if (!requestId.isEmpty())
        {
            onRecycleBulkById(requestId);
        }
    }
}

bool JsonOperation::getUploadUrlById(const QString& method,
                                     const QString& objectId,
                                     qlonglong fileSize)
{
    QList<JsonApiParams> params;
    params.append(JsonApiParams("method", method /*POST*/));
    params.append(JsonApiParams("objectId", objectId));
    params.append(JsonApiParams("size", QString::number(fileSize)));
    connect(this, SIGNAL(response()), this, SLOT(getUploadUrlByIdResponse()));
    return execute("getUploadUrlById", params);
}

void JsonOperation::getUploadUrlByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        return;
    }
    else
    {
       QByteArray storageKey = parseResult["getUploadUrlByIdResponse"]
                .toMap()["getUploadUrlByIdResult"]
                .toMap()["storageKey"].toByteArray();
        QString endPoint = parseResult["getUploadUrlByIdResponse"]
                .toMap()["getUploadUrlByIdResult"]
                .toMap()["httpRequest"]
                .toMap()["endpoint"].toString();
        QByteArray resourcePath = parseResult["getUploadUrlByIdResponse"]
                .toMap()["getUploadUrlByIdResult"]
                .toMap()["httpRequest"]
                .toMap()["resourcePath"].toByteArray();
        QVariantMap uploadParams = parseResult["getUploadUrlByIdResponse"]
                .toMap()["getUploadUrlByIdResult"]
                .toMap()["httpRequest"]
                .toMap()["parameters"].toMap();
        emit onGetUploadUrlById(storageKey, endPoint, resourcePath, uploadParams);
    }
}

bool JsonOperation::completeFileUploadById(const QString &objectId,
                                           const QByteArray &storageKey)
{
    QList<JsonApiParams> params;
    params.append(JsonApiParams("objectId", objectId));
    params.append(JsonApiParams("storageKey", Utils::urlEncode(storageKey)));
    connect(this, SIGNAL(response()), this, SLOT(completeFileUploadByIdResponse()));
    return execute("completeFileUploadById", params);
}

void JsonOperation::completeFileUploadByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        return;
    }
    else
    {
        //parse:  completeFileUploadByIdResponse.ResponseMetadata.RequestId
        QString requestId = parseResult["completeFileUploadByIdResponse"]
                .toMap()["ResponseMetadata"]
                .toMap()["RequestId"].toString();
        if (!requestId.isEmpty())
        {
            onCompleteFileUploadById(requestId);
        }
    }
}

bool JsonOperation::getDownloadUrlById(const QString &fileObjectId,
                                       const QString &driveSerialNum)
{
    QList<JsonApiParams> params;
    params.append(JsonApiParams("objectId", fileObjectId));
    params.append(JsonApiParams("attachment", "true"));
    params.append(JsonApiParams("deviceId.deviceSerialNumber", Utils::urlEncode(driveSerialNum)));
    params.append(JsonApiParams("deviceId.deviceType", "ADrive_Web"));

    connect(this, SIGNAL(response()), this, SLOT(getDownloadUrlByIdResponse()));
    return execute("getDownloadUrlById", params);
}

void JsonOperation::getDownloadUrlByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing getDownloadUrlByIdResponse";
    }
    else
    {
        QByteArray downloadURL =
                parseResult["getDownloadUrlByIdResponse"]
                .toMap()["getDownloadUrlByIdResult"]
                .toMap()["url"].toByteArray();
        //downloadFile(downloadURL);
        emit onGetDownloadUrlById(downloadURL);
    }
}

bool JsonOperation::getUserStorage()
{
    QList<JsonApiParams> params;
    connect(this, SIGNAL(response()), this, SLOT(getUserStorageResponse()));
    return execute("getUserStorage", params);
}

void JsonOperation::getUserStorageResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    qlonglong freeSpace;
    qlonglong totalSpace;
    qlonglong usedSpace;
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";        
    }
    else
    {
        freeSpace =
                parseResult["getUserStorageResponse"]
                .toMap()["getUserStorageResult"]
                .toMap()["freeSpace"].toLongLong();
        totalSpace =
                parseResult["getUserStorageResponse"]
                .toMap()["getUserStorageResult"]
                .toMap()["totalSpace"].toLongLong();
        usedSpace =
                parseResult["getUserStorageResponse"]
                .toMap()["getUserStorageResult"]
                .toMap()["usedSpace"].toLongLong();
        emit onGetUserStorage(freeSpace, totalSpace, usedSpace);
    }
}

bool JsonOperation::moveById(
    const QString& sourceId,
    const QString& destinationParentId,
    const QString& destinationName,
    bool overwrite)
{
    QList<JsonApiParams> params;    
    params.append(JsonApiParams("sourceId", sourceId));
    params.append(JsonApiParams("destinationParentId", destinationParentId));
    params.append(JsonApiParams("destinationName", Utils::urlEncode(destinationName)));
    params.append(JsonApiParams("overwrite", overwrite?"true":"false"));
    connect(this, SIGNAL(response()), this, SLOT(moveByIdResponse()));
    return execute("moveById", params);
}

void JsonOperation::moveByIdResponse()
{
    JsonOperation *jsonOp = (JsonOperation *)sender();
    QJson::Parser parser;
    bool parseStatus;
    QVariantMap parseResult = parser.parse(jsonOp->getData(), &parseStatus).toMap();
    if (!parseStatus)
    {
        qDebug() << "An error occurred during parsing";
        emit onMoveById(QString());
    }
    else
    {
        emit onMoveById(parseResult["moveByIdResponse"]
                            .toMap()["moveByIdResult"]
                            .toMap()["objectId"].toString());
    }
}
