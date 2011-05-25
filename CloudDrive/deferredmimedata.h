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

#ifndef DEFERREDMIMEDATA_H
#define DEFERREDMIMEDATA_H
#include <QHash>
#include <QList>
#include <QUrl>
#include <QMimeData>

class DeferredMimeDataItem
{
public:
    DeferredMimeDataItem(const QUrl &filenameLocalUrl, const QString &objectId);
    QUrl downloadFileNameLocalUrl;
    QString downloadObjectId;
};

class DeferredMimeData: public QMimeData
{
    Q_OBJECT

public:
    DeferredMimeData(QString downloadFileName, QString downloadObjectId);
    ~DeferredMimeData();

private:    
    int dataLoadState;    
    QList<DeferredMimeDataItem> deferredMimeDataItems;

protected:
    QVariant retrieveData(const QString &mimeType, QVariant::Type preferredType) const;
    bool hasFormat(const QString &mimeType) const;

public: signals:
    void dataRequested(const QString &downloadObjectId, const QString &downloadFileName);

};

#endif // DEFERREDMIMEDATA_H
