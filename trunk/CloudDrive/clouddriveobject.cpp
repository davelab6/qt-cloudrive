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

#include "clouddriveobject.h"

const QString ObjectTypeRoot("ROOT");
const QString ObjectTypeRecycle("RECYCLE");
const QString ObjectTypeFolder("FOLDER");
const QString ObjectTypeFile("FILE");

CloudDriveFileObject::CloudDriveFileObject(const QVariantMap& objectMap)
{
    objectId = objectMap["objectId"].toString();
    parentObjectId = objectMap["parentObjectId"].toString();
    objectName = objectMap["name"].toString();
    objectType = objectMap["type"].toString();
    fileSize = objectMap["size"].toInt();
    lastModified = objectMap["lastUpdatedDate"].toDateTime();
}

QString const& CloudDriveFileObject::getObjectName() const
{
    return objectName;
}

int CloudDriveFileObject::getFileSize()
{
    return fileSize;
}

QString const& CloudDriveFileObject::getParentObjectId() const
{
    return parentObjectId;
}

QString const& CloudDriveFileObject::getObjectId() const
{
    return objectId;
}

QString const& CloudDriveFileObject::getObjectType() const
{
    return objectType;
}

QDateTime const& CloudDriveFileObject::getLastModified() const
{
    return lastModified;
}
