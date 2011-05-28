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

#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QFile>
#include "deferredmimedata.h"

DeferredMimeDataItem::DeferredMimeDataItem(const QUrl &filenameLocalUrl, const QString &objectId):
    downloadFileNameLocalUrl(filenameLocalUrl),
    downloadObjectId(objectId)
{

}

QUrl const& DeferredMimeDataItem::getDownloadFileNameLocalUrl() const
{
    return downloadFileNameLocalUrl;
}

QString const& DeferredMimeDataItem::getDownloadObjectId() const
{
    return downloadObjectId;
}

DeferredMimeData::DeferredMimeData(QString downloadFileName, QString downloadObjectId)
{
    QList<QUrl> tempFileNameList;
    dataLoadState = 0;
    qDebug() << "Creating DeferredMimeData with ObjectId: " << downloadObjectId;
    QDir tempDir(QDir::tempPath());
    QString tempFileName = tempDir.absolutePath() + QDir::separator() + downloadFileName;
    QFileInfo tempFile(tempFileName);
    qDebug() << "making path" << tempFile.absolutePath();
    QFile::remove(tempFileName);
    tempDir.mkpath(tempFile.absolutePath());
    qDebug() << "storing url" << QUrl::fromLocalFile(tempFileName);
    tempFileNameList << QUrl::fromLocalFile(tempFileName);
    setUrls(tempFileNameList);
    deferredMimeDataItems.append(DeferredMimeDataItem(QUrl::fromLocalFile(tempFileName), downloadObjectId));
}

DeferredMimeData::~DeferredMimeData()
{
        qDebug() << "Destroying DeferredMimeData";
        foreach(QUrl url, urls())
        {
                QFile::remove(url.toLocalFile());
        }
        deferredMimeDataItems.clear();
}

QVariant DeferredMimeData::retrieveData(const QString &mimeType,
                                  QVariant::Type preferredType) const
{
    if (hasFormat(mimeType) && (preferredType == QVariant::List))
    {
        //qDebug() << "Requested" << mimeType << preferredType;
        QVariant data = QMimeData::retrieveData(mimeType, preferredType);
        QList<QVariant> tempFileNameList = data.toList();
        if (!tempFileNameList.isEmpty())
        {
            for(int i = 0; i < tempFileNameList.length(); i++)
            {
                QVariant tempFileName = tempFileNameList.at(i);
                const QString&  downloadObjectId = deferredMimeDataItems.at(i).getDownloadObjectId();
                //qDebug() << "Begin DeferredMimeData::retrieveData " << downloadObjectId << mimeType;
                emit const_cast<DeferredMimeData*>(this)->dataRequested(
                            downloadObjectId,
                            tempFileName.toUrl().toLocalFile());
                //qDebug() << "End DeferredMimeData::retrieveData " << downloadObjectId << mimeType;
                //ONLY one file for now
                break;
            }
            return data;
        }
    }
    return QVariant();
}

bool DeferredMimeData::hasFormat(const QString &mimeType) const
{
    return mimeType == QLatin1String("text/uri-list");
}
