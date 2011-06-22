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

#ifndef FILETABLEWIDGET_H
#define FILETABLEWIDGET_H

#include <QTableWidget>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

namespace Ui {
    enum UserRoles
    {
        ObjectIdRole = Qt::UserRole,
        ParentObjectIdRole = Qt::UserRole + 1,
        ObjectTypeRole = Qt::UserRole + 2
    };
}

class FileTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit FileTableWidget(QWidget *parent = 0);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void startDrag(Qt::DropActions supportedActions);
    Qt::DropActions supportedDropActions () const;
    void keyPressEvent(QKeyEvent *event);
signals:
    void itemDragged(const QString& objectId, const QString& fileName);
    void itemsDropped(const QList<QUrl>& urlList);
    void returnPressed();

public slots:

private:

};

#endif // FILETABLEWIDGET_H
