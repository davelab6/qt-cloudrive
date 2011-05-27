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

#ifndef JSONOPERATION_H
#define JSONOPERATION_H

#include <QObject>
#include <QPair>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <QVariantMap>

typedef QPair<QString, QString> JsonApiParams;

class JsonOperationResponse;

class JsonOperation : public QObject
{
Q_OBJECT

public:
    JsonOperation(
        QNetworkAccessManager *networkAccessManager,
            const QString &driveServer,
            const QString &customerId,
            const QString &sessionId);

public slots:
    bool execute(const QString &operationName,
                 const QList<JsonApiParams> &operationParams);

public:
    QByteArray getData() const;

    bool createByPath(
        bool autoparent,
        const QString& conflictResolution,
        const QString& name,
        bool overwrite,
        const QString& path,
        const QString& type);
    bool getUploadUrlById(const QString& method,const QString& objectId, qlonglong fileSize);
    bool completeFileUploadById(const QString &objectId, const QByteArray &storageKey);
    bool removeBulkById(const QList<QString>& objectIds);
    bool recycleBulkById(const QList<QString>& objectIds);
    bool getDownloadUrlById(const QString &fileObjectId,
                            const QString &driveSerialNum);

private slots:
    void finished();
    void createByPathResponse();
    void getUploadUrlByIdResponse();
    void completeFileUploadByIdResponse();
    void removeBulkByIdResponse();
    void recycleBulkByIdResponse();
    void getDownloadUrlByIdResponse();

signals:
    void response();
    void jsonOpError(QString errorCode, QString errorMessage);
    void onCreateByPath(const QString& createdObjectId);
    void onGetUploadUrlById(
        const QByteArray &storageKey,
        const QString &endPoint,
        const QByteArray &resourcePath,
        const QVariantMap &uploadParams);
    void onCompleteFileUploadById(const QString &requestId);
    void onRemoveBulkById(const QString &requestId);
    void onRecycleBulkById(const QString &requestId);
    void onGetDownloadUrlById(const QByteArray &downloadURL);

private:
    QString driveServer;
    QString customerId;
    QString sessionId;
    QNetworkAccessManager *networkAccessManager;
    QByteArray internalBuff;
};

#endif // JSONOPERATION_H
