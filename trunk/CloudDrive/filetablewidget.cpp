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

#include <QDebug>
#include <QUrl>

#include "filetablewidget.h"
#include "clouddriveobject.h"

FileTableWidget::FileTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void FileTableWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "FileTableWidget::dropEvent";
    if (event->source() == 0)
    {
        QTableWidget::dropEvent(event);
        const QMimeData *mimeData = event->mimeData();
        if (mimeData->hasUrls())
        {
           event->acceptProposedAction();
           QList<QUrl> urlList = mimeData->urls();
           emit itemsDropped(urlList);
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->ignore();
    }
}

void FileTableWidget::startDrag(Qt::DropActions supportedActions)
{
    QTableWidgetItem *draggedItem = currentItem();
    QMimeData *mimeData = new QMimeData;
    qDebug() << "Start dragging" << draggedItem->row();
    QVariant vObjectId = draggedItem->data(Ui::ObjectIdRole);
    QVariant vObjectType = draggedItem->data(Ui::ObjectTypeRole);

    if (vObjectType.toString() == ObjectTypeFile)
    {
        if ((draggedItem->row() == 0) && (draggedItem->text() == ".."))
        {

            return;
        }
        else
        {
            qDebug() << "FileTableWidget::startDrag";
            emit itemDragged(vObjectId.toString(), draggedItem->text());
        }
    }
}

void FileTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QTableWidget::dragEnterEvent(event);
    event->acceptProposedAction();
}

void FileTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QTableWidget::dragMoveEvent(event);  
    event->acceptProposedAction();
}

Qt::DropActions FileTableWidget::supportedDropActions () const
{
  return Qt::MoveAction;
}
