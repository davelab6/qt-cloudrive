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

#ifndef CLOUDDRIVEOBJECT_H
#define CLOUDDRIVEOBJECT_H

#include <QString>
#include <QDateTime>
#include <QVariantMap>

extern const QString ObjectTypeRoot;
extern const QString ObjectTypeRecycle;
extern const QString ObjectTypeFolder;
extern const QString ObjectTypeFile;

class CloudDriveFileObject
{

private:
    QString objectName;
    int fileSize;
    QString parentObjectId;
    QString objectId;
    QString objectType;
    QDateTime lastModified;

public:
    CloudDriveFileObject(const QVariantMap& objectMap);

public:
    QString const& getObjectName() const;
    int getFileSize();
    QString const& getParentObjectId() const;
    QString const& getObjectId() const;
    QString const& getObjectType() const;
    QDateTime const& getLastModified() const;
};

#endif // CLOUDDRIVEOBJECT_H
