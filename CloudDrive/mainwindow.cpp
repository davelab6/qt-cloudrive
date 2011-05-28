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

#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QSysInfo>
#include <QInputDialog>
#include <QEventLoop>

#include "generalconfig.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clouddriveobject.h"
#include "cloudutils.h"
#include "userconfigdialog.h"
#include "deferredmimedata.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    setWindowTitle(QString(tr("Cloud Drive Explorer b%1")).arg(ProductVersion));
    ui->action_Upload->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    ui->actionRename->setEnabled(false);
    ui->btnPlay->setEnabled(false);
    ui->frmPlayMusic->setVisible(false);
    ui->statusBar->showMessage(tr("Signing in..."));
    QSettings settings(Organization, Product);
    QString userEmail = settings.value(EmailConfigKey, QString()).toString();
    QString userPass = settings.value(PasswordConfigKey, QString()).toString();
    if (userEmail.isEmpty() || userPass.isEmpty())
    {
        UserConfigDialog userConfigDialog(this);
        if (userConfigDialog.exec() == QDialog::Accepted)
        {
            userEmail = settings.value(EmailConfigKey).toString();
            userPass = settings.value(PasswordConfigKey).toString();
        }
        else
        {
            return;
        }
    }
    if (userEmail.isEmpty() || userPass.isEmpty())
    {
        return;
    }
    connect(&amazonWebSite, SIGNAL(jsonOpError(QString, QString)),
            this, SLOT(jsonOpError(QString, QString)));
    connect(&amazonWebSite,
                SIGNAL(onUserSignedIn(const QString&, const QString&)),
                this,
                SLOT(onUserSignedIn(const QString&, const QString&)));
    connect(&amazonWebSite, SIGNAL(onFileUploaded(const QString &)),
            this, SLOT(onFileUploaded(const QString &)),
            Qt::UniqueConnection);
    connect(&amazonWebSite, SIGNAL(onUploadProgress(const QString &, qint64, qint64)),
            this, SLOT(onUploadProgress(const QString &, qint64, qint64)),
            Qt::UniqueConnection);
    connect(ui->tblFiles, SIGNAL(itemDragged(const QString&, const QString&)),
            this, SLOT(itemDragged(const QString&, const QString&)));
    connect(ui->tblFiles, SIGNAL(itemsDropped(const QList<QUrl>&)),
            this, SLOT(itemsDropped(const QList<QUrl>&)));
    connect(&amazonWebSite,
            SIGNAL(onFileDownloaded(const QString &,
                                    qlonglong,
                                    const QString &,
                                    QIODevice *,
                                    const QString &)),
            this,
            SLOT(onFileDownloaded(const QString &,
                                  qlonglong,
                                  const QString &,
                                  QIODevice *,
                                  const QString &)),
            Qt::UniqueConnection);

    connect(&amazonWebSite,
            SIGNAL(onDownloadProgress(const QString &, qint64, qint64)),
            this,
            SLOT(onDownloadProgress(const QString &, qint64, qint64)),
            Qt::UniqueConnection);

    amazonWebSite.signIn(userEmail, userPass);
}

MainWindow::~MainWindow()
{
    amazonWebSite.signOut();
    delete ui;
}

void MainWindow::onUserSignedIn(const QString &customerId, const QString &sessionId)
{
    this->customerId = customerId;
    this->sessionId = sessionId;

    parentObjectPaths.push("/");
    parentObjectIds.clear();

    connect(&amazonWebSite,
            SIGNAL(onGetInfoByPath(const QString&)),
            this,
            SLOT(onGetRootPathObjectId(const QString&)),
            Qt::UniqueConnection);

    connect(&amazonWebSite,
            SIGNAL(onListObjects(const QList<CloudDriveFileObject> &)),
            this,
            SLOT(onListObjects(const QList<CloudDriveFileObject> &)));


    amazonWebSite.getInfoByPath("/");
}

void MainWindow::onGetRootPathObjectId(const QString &objectId)
{
    ui->action_Upload->setEnabled(true);
    rootObjectId = objectId;    
    parentObjectIds.push(objectId);
    ui->tblFiles->clearContents();
    ui->tblFiles->setRowCount(0);

    amazonWebSite.listById(objectId,
                           "type,keyName,creationDate",
                           0,
                           1000,
                           "type != 'RECYCLE' and status != 'PENDING' and hidden = false");
}

void MainWindow::onListObjects(const QList<CloudDriveFileObject> &objectList)
{
    displayObjects(objectList);    
    connect(&amazonWebSite,
            SIGNAL(userStorage(qlonglong, qlonglong, qlonglong)),
            this,
            SLOT(userStorage(qlonglong, qlonglong, qlonglong)),
            Qt::UniqueConnection);
    amazonWebSite.getUserStorage();
}

void MainWindow::displayObjects(const QList<CloudDriveFileObject> &objectList)
{
    QList<CloudDriveFileObject>::const_iterator fileObjectIter;
    for (fileObjectIter = objectList.begin(); fileObjectIter != objectList.end(); fileObjectIter++)
    {
        CloudDriveFileObject fileObject = *fileObjectIter;
        QTableWidgetItem *itemFileName = new QTableWidgetItem(fileObject.getObjectName());
        itemFileName->setData(Ui::ObjectIdRole, fileObject.getObjectId());
        itemFileName->setData(Ui::ParentObjectIdRole, fileObject.getParentObjectId());
        itemFileName->setData(Ui::ObjectTypeRole, fileObject.getObjectType());
        QTableWidgetItem *itemFileType = new QTableWidgetItem(fileObject.getObjectType());
        QTableWidgetItem *itemFileSize = new QTableWidgetItem(Utils::formatStorage(fileObject.getFileSize()));
        QTableWidgetItem *itemFileDateTime = new QTableWidgetItem(fileObject.getLastModified().toLocalTime().toString());
        int row = ui->tblFiles->rowCount();
        ui->tblFiles->insertRow(row);
        ui->tblFiles->setItem(row, 0, itemFileName);
        ui->tblFiles->setItem(row, 1, itemFileType);
        ui->tblFiles->setItem(row, 2, itemFileSize);
        ui->tblFiles->setItem(row, 3, itemFileDateTime);
        ui->tblFiles->resizeColumnToContents(0);
        ui->tblFiles->resizeColumnToContents(1);
        ui->tblFiles->resizeColumnToContents(2);
        ui->tblFiles->resizeColumnToContents(3);
    }
}

void MainWindow::createBackItem()
{
    QTableWidgetItem *itemFileName = new QTableWidgetItem("..");
    itemFileName->setData(Ui::ObjectIdRole, parentObjectIds.last());
    itemFileName->setData(Ui::ParentObjectIdRole, "");
    itemFileName->setData(Ui::ObjectTypeRole, ObjectTypeFolder);
    ui->tblFiles->insertRow(0);
    ui->tblFiles->setItem(0, 0, itemFileName);
}

void MainWindow::openFolder(
    QString folderObjectName,
    QString folderObjectId,
    int row)
{
    if ((row == 0) && (folderObjectName.compare("..") == 0))
    {
        if (!parentObjectIds.isEmpty())
        {
             folderObjectId = parentObjectIds.pop();
             parentObjectPaths.pop();
             if (!parentObjectIds.isEmpty())
             {
                 folderObjectId = parentObjectIds.last();
             }
        }
        else
        {
            return;
        }
    }
    else
    {
        parentObjectIds.push(folderObjectId);
        parentObjectPaths.push(folderObjectName);
    }
    ui->tblFiles->clearContents();
    ui->tblFiles->setRowCount(0);
    if (parentObjectIds.count() > 1)
    {
        createBackItem();
    }
    amazonWebSite.listById(folderObjectId,
                            "type,keyName,creationDate",
                            0,
                            1000,
                            "type != 'RECYCLE' and status != 'PENDING' and hidden = false");
}

void MainWindow::refreshCurrentFolder()
{
    if (ui->tblFiles->rowCount() > 0)
    {
        if (ui->tblFiles->item(0, 0)->text().compare("..") != 0)
        {
            ui->tblFiles->clearContents();
            ui->tblFiles->setRowCount(0);
        }
        else
        {
            ui->tblFiles->setRowCount(1);
        }
    }
    amazonWebSite.listById(parentObjectIds.last(),
                            "type,keyName,creationDate",
                            0,
                            1000,
                            "type != 'RECYCLE' and status != 'PENDING' and hidden = false");
}

bool MainWindow::downloadFile(const QString &sourceFileName,
                              const QString &sourceFileObjectId,
                              const QString &destinationDir)
{
    qDebug() << "Downloading file " << sourceFileName << " into " << destinationDir << " dir ";
    return amazonWebSite.addFileToDownloadQueue(sourceFileName, sourceFileObjectId, destinationDir);
}

void MainWindow::onDownloadProgress(const QString &fileName, qint64 bytesReceived, qint64 bytesTotal)
{
    ui->statusBar->showMessage(QString("Downloading %1 (%2/%3)")
              .arg(fileName)
              .arg(Utils::formatStorage(bytesReceived))
              .arg(Utils::formatStorage(bytesTotal)),
              1000);
}

void MainWindow::onFileDownloaded(const QString &contentType,
                                  qlonglong contentLength,
                                  const QString &fileName,
                                  QIODevice *inputStream,
                                  const QString &fileDestDir)
{
    if ((contentLength > 0) && (contentLength < (1024*1024*1024)))
    {
        QString destFilePath = QString();
        if (fileDestDir != NULL)
        {
            if (!fileDestDir.isNull() && !fileDestDir.isEmpty())
            {
                destFilePath = fileDestDir;
                if (!destFilePath.endsWith(QDir::separator()))
                {
                    if (!fileName.startsWith(QDir::separator()))
                    {
                        destFilePath.append(QDir::separator());
                    }
                }
                destFilePath.append(fileName);
            }
        }
        if (fileDestDir.isEmpty())
        {
            QFileDialog fileSaveDialog(this);
            fileSaveDialog.setFileMode(QFileDialog::AnyFile);
            destFilePath = QFileDialog::getSaveFileName(this, tr("Save File"), fileName);
        }
        QFile file(destFilePath);
        file.open(QIODevice::Truncate | QIODevice::ReadWrite);
        qlonglong totalBytes = contentLength;
        char currChar;
        for ( ; totalBytes != 0; totalBytes--)
        {
            if (inputStream->getChar(&currChar))
            {
                file.write(&currChar, 1);
            }
            else
            {
                break;
            }
        }
        file.close();
        qDebug() << "MainWindow::onFileDownloaded File saved under: " << destFilePath;
    }
    inputStream->close();
}

void MainWindow::userStorage(
    qlonglong freeSpace,
    qlonglong totalSpace,
    qlonglong usedSpace)
{
    ui->statusBar->showMessage(
                QString(tr("Signed in, customer id:%1 Storage Total: %2, Used: %3, Free: %4"))
        .arg(customerId)
        .arg(Utils::formatStorage(totalSpace))
        .arg(Utils::formatStorage(usedSpace))
        .arg(Utils::formatStorage(freeSpace)));
}

void MainWindow::onFileUploaded(const QString &fileName)
{    
    refreshCurrentFolder();
    ui->statusBar->showMessage(QString("Successfully uploaded %1").arg(fileName));
}

void MainWindow::on_actionE_xit_triggered()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Close Cloud Drive Application"));
    msgBox.setInformativeText(tr("Are you sure you want to close the application?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Ok)
    {       
        amazonWebSite.signOut();
        this->close();
    }
}

QString MainWindow::getCurrentRemotePath()
{
    QListIterator<QString> pathIter(parentObjectPaths.toList());
    QString remotePath = "";
    if (pathIter.hasNext())
    {
        remotePath.append(pathIter.next());
    }
    while (pathIter.hasNext())
    {
        QString pathElement = pathIter.next();
        if (!pathElement.isEmpty())
        {
            if (!remotePath.endsWith("/"))
            {
                remotePath.append("/");
            }
            remotePath.append(pathElement);
        }
    }
    return remotePath;
}

void MainWindow::on_action_Upload_triggered()
{
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if (fileDialog.exec())
    {
        if (fileDialog.selectedFiles().count() > 0)
        {
            QString selectedFileName = fileDialog.selectedFiles().first();
            QString remotePath = getCurrentRemotePath();

            amazonWebSite.addFileToUploadQueue(selectedFileName,remotePath);
            ui->statusBar->showMessage(
                        QString(tr("Uploading %1 to %2"))
                        .arg(Utils::extractFileName(selectedFileName))
                        .arg(remotePath));
            connect(&amazonWebSite,
                    SIGNAL(onFileUploaded(const QString &)),
                    this,
                    SLOT(onFileUploaded(const QString &)),
                    Qt::UniqueConnection);
            connect(&amazonWebSite, SIGNAL(onUploadProgress(const QString &, qint64, qint64)),
                    this, SLOT(onUploadProgress(const QString &, qint64, qint64)),
                    Qt::UniqueConnection);
        }
    }
}

void MainWindow::on_action_Account_triggered()
{
    QSettings settings(Organization, Product);
    QString userEmail = settings.value(EmailConfigKey, QString()).toString();
    QString userPass = settings.value(PasswordConfigKey, QString()).toString();

    UserConfigDialog userConfigDialog(this);
    if (userConfigDialog.exec() == QDialog::Accepted)
    {
        userEmail = settings.value(EmailConfigKey).toString();
        userPass = settings.value(PasswordConfigKey).toString();
    }
    else
    {
        return;
    }
    if (userEmail.isEmpty() || userPass.isEmpty())
    {
        return;
    }

    connect(&amazonWebSite,
                SIGNAL(onUserSignedIn(const QString&, const QString&)),
                this,
                SLOT(onUserSignedIn(const QString&, const QString&)));
    amazonWebSite.signIn(userEmail, userPass);
}

void MainWindow::closeEvent(QCloseEvent *event)
{    
    amazonWebSite.signOut();
    event->accept();
}

void MainWindow::itemsDropped(const QList<QUrl>& urlList)
{
    foreach (QUrl url, urlList)
    {
        QString localFilePath = url.toLocalFile();
        if (QDir(localFilePath).exists())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Uploading folders not yet implemented :-)"));
            msgBox.exec();
        }
        else if (QFile(localFilePath).exists())
        {
            if (urlList.size() > 1)
            {
                ui->statusBar->showMessage(tr("Multiple files dropped."));
            }
            else
            {
                ui->statusBar->showMessage(QString(tr("Dropped %1")).arg(localFilePath));
            }
            amazonWebSite.addFileToUploadQueue(localFilePath, getCurrentRemotePath());
        }
     }
}

void MainWindow::onUploadProgress(const QString &fileName, qint64 bytesSent, qint64 bytesTotal)
{
    ui->statusBar->showMessage(QString(tr("Uploading %1 %2/%3"))
              .arg(fileName)
              .arg(Utils::formatStorage(bytesSent))
              .arg(Utils::formatStorage(bytesTotal)), 1000);
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
    //event->ignore();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
    //event->ignore();
}

void MainWindow::on_action_Refresh_triggered()
{
    refreshCurrentFolder();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
    msgBox.setText(QString(tr("Cloud Drive Application %1")).arg(ProductVersion));
    msgBox.setInformativeText(tr(
        "QT Cloud Drive, desktop application for connecting to Cloud Drive \r\n"
        "Copyright (C) 2011 Vasko Mitanov vasko.mitanov@hotmail.com \r\n"
        "This program is free software: you can redistribute it and/or modify \r\n"
        "it under the terms of the GNU General Public License as published by \r\n"
        "the Free Software Foundation, either version 3 of the License, or \r\n"
        "(at your option) any later version."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_actionCreate_Folder_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Create new folder"),
                                         tr("Folder name:"),
                                         QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
    {
        connect(&amazonWebSite, SIGNAL(onFolderCreated(const QString&)),
                this, SLOT(onFolderCreated(const QString&)),
                Qt::UniqueConnection);
        amazonWebSite.createFolder(getCurrentRemotePath(), text);
    }
}

void MainWindow::onFolderCreated(const QString &folderObjectId)
{
    ui->statusBar->showMessage(tr("Folder created."));
    refreshCurrentFolder();
}

void MainWindow::on_tblFiles_itemSelectionChanged()
{
    QModelIndexList selectedList =
            ui->tblFiles->selectionModel()->selectedRows();

    if (selectedList.isEmpty())
    {
        ui->actionDelete->setEnabled(false);
        ui->actionRename->setEnabled(false);
        return;
    }
    int totalFilesSelected = 0;
    ui->frmPlayMusic->setVisible(false);
    ui->btnPlay->setEnabled(false);
    foreach (QModelIndex index, selectedList)
    {
        QTableWidgetItem *itemFileName =
                ui->tblFiles->item(index.row(), 0);
        if (itemFileName != NULL)
        {
            if (itemFileName->text().endsWith(".mp3"))

            {
                //to enable player first we need to resolve
                //problem with multiple connections limit
                ui->btnPlay->setEnabled(false);

                ui->frmPlayMusic->setVisible(true);
            }
            totalFilesSelected++;
        }
    }

    if (totalFilesSelected > 0)
    {
        ui->actionDelete->setEnabled(true);
        ui->actionRename->setEnabled(true);
    }

}

void MainWindow::on_actionDelete_triggered()
{
    QModelIndexList selectedList =
            ui->tblFiles->selectionModel()->selectedRows();

    if (selectedList.isEmpty())
    {
        ui->actionDelete->setEnabled(false);
        ui->actionRename->setEnabled(false);
        return;
    }
    int totalFilesSelected = 0;
    QList<QString> objectIds;
    foreach (QModelIndex index, selectedList)
    {
        QTableWidgetItem *itemFileName =
                ui->tblFiles->item(index.row(), 0);
        if (itemFileName != NULL)
        {
            if ((itemFileName->text() == "..")
                    && (index.row() == 0))
            {

            }
            else
            {
                QVariant vObjectId = itemFileName->data(Ui::ObjectIdRole);
                QVariant vObjectType = itemFileName->data(Ui::ObjectTypeRole);
                qDebug() << vObjectId << vObjectType;
                objectIds.append(vObjectId.toString());
                totalFilesSelected++;
            }
        }
    }

    if (totalFilesSelected > 0)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Delete file"));
        msgBox.setInformativeText(
                    QString(tr("Are you sure you want to delete %1 files?"))
                    .arg(totalFilesSelected));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if (msgBox.exec() != QMessageBox::Yes)
        {
            return;
        }
    }

    if (!objectIds.isEmpty())
    {
        connect(&amazonWebSite, SIGNAL(onBulkRemovedById()),
                this, SLOT(onBulkRemovedById()),
                Qt::UniqueConnection);
        amazonWebSite.removeBulkById(objectIds);
    }
}

void MainWindow::onBulkRemovedById()
{
    refreshCurrentFolder();
}

void MainWindow::onBulkRecycledById()
{
    refreshCurrentFolder();
}

void MainWindow::on_actionPlay_triggered()
{
    QModelIndexList selectedList =
            ui->tblFiles->selectionModel()->selectedRows();

    if (selectedList.isEmpty())
    {
        return;
    }

    QList<QString> objectIds;
    foreach (QModelIndex index, selectedList)
    {
        QTableWidgetItem *itemFileName =
                ui->tblFiles->item(index.row(), 0);
        if (itemFileName != NULL)
        {
            if ((itemFileName->text() == "..") && (index.row() == 0))
            {

            }
            else
            {
                QVariant vObjectId = itemFileName->data(Ui::ObjectIdRole);
                QVariant vObjectType = itemFileName->data(Ui::ObjectTypeRole);
                if (vObjectType.toString() == ObjectTypeFile)
                {
                    objectIds.append(vObjectId.toString());
                }
            }
        }
    }

    if (!objectIds.isEmpty())
    {
        connect(&amazonWebSite, SIGNAL(onGotDownloadUrl(const QByteArray &)),
                this, SLOT(onGotDownloadUrl(const QByteArray &)),
                Qt::UniqueConnection);
        //Play only the first object
        amazonWebSite.getDownloadUrlById(objectIds.at(0));
    }
}

void MainWindow::onGotDownloadUrl(const QByteArray &downloadURL)
{    
    Phonon::MediaObject *mediaObject = new Phonon::MediaObject(this);
    QUrl url(downloadURL);
    mediaObject->setCurrentSource(Phonon::MediaSource(url));
    Phonon::AudioOutput *audioOutput =
            new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::Path path = Phonon::createPath(mediaObject, audioOutput);
    mediaObject->play();

}

void MainWindow::jsonOpError(QString errorCode, QString errorMessage)
{
    QMessageBox msgBox;
    msgBox.setText(QString(tr("Error %1 occured")).arg(errorCode.trimmed()));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setInformativeText(QString(tr("%1"))
                              .arg(errorMessage.trimmed()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_btnPlay_clicked()
{    
    on_actionPlay_triggered();
}

void MainWindow::itemDragged(const QString& objectId, const QString& fileName)
{
    qDebug() << "itemDragged" << objectId << fileName;
    DeferredMimeData *dragMimeData = new DeferredMimeData(fileName, objectId);
    connect(dragMimeData, SIGNAL(dataRequested(const QString &, const QString &)),
            this, SLOT(mimeDargDataRequested(const QString &, const QString &)),
            Qt::DirectConnection);

    QDrag *drag = new QDrag(this);    
    drag->setMimeData(dragMimeData);
    //drag->setPixmap(iconPixmap);
    drag->exec(Qt::CopyAction);
}

void MainWindow::mimeDargDataRequested(const QString &downloadObjectId, const QString &downloadFileName)
{
    //DeferredMimeData *mimeData = (DeferredMimeData *)sender();
    if (lastDownloadObjectId == downloadObjectId)
    {

    }
    else
    {
        qDebug() << "MainWindow::mimeDargDataRequested" << downloadObjectId << downloadFileName ;
        lastDownloadObjectId = downloadObjectId;
        DownloadQueueItem *item =
                new DownloadQueueItem(Utils::extractFileName(downloadFileName),
                                      downloadObjectId,
                                      Utils::extractFileDir(downloadFileName));
        QEventLoop localEventLoop;
        connect(item, SIGNAL(onQueueItemDownloaded()), &localEventLoop, SLOT(quit()));
        qDebug() << "MainWindow::mimeDargDataRequested added item on download queue" ;
        amazonWebSite.addFileToDownloadQueue(item);
        localEventLoop.exec();
        qDebug() << "MainWindow::mimeDargDataRequested Exit from local loop" ;
    }
    //mimeData->deleteLater();
}

void MainWindow::downloadFileByItemIndex(const QModelIndex & index)
{
    QTableWidgetItem *itemFileName = ui->tblFiles->item(index.row(), 0);
    if (itemFileName != NULL)
    {
        QVariant vObjectId = itemFileName->data(Ui::ObjectIdRole);
        QVariant vObjectType = itemFileName->data(Ui::ObjectTypeRole);

        if (!vObjectId.isNull())
        {
           if (QString(ObjectTypeFolder).compare(vObjectType.toString()) == 0)
           {
               openFolder(itemFileName->text(), vObjectId.toString(), index.row());
           }
           else if (QString(ObjectTypeFile).compare(vObjectType.toString()) == 0)
           {
               downloadFile(itemFileName->text(), vObjectId.toString(), QString());
           }
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{

}

void MainWindow::on_tblFiles_doubleClicked(QModelIndex index)
{
    downloadFileByItemIndex(index);
}

void MainWindow::on_action_Download_triggered()
{
    QModelIndexList selectedObjects =
             ui->tblFiles->selectionModel()->selectedRows();

     foreach (QModelIndex selectedObject, selectedObjects)
     {
         downloadFileByItemIndex(selectedObject);
     }
}

void MainWindow::on_actionRename_triggered()
{
    QModelIndexList selectedList =
            ui->tblFiles->selectionModel()->selectedRows();

    if (selectedList.isEmpty())
    {
        ui->actionDelete->setEnabled(false);
        ui->actionRename->setEnabled(false);
        return;
    }
    int totalFilesSelected = 0;
    QString selectedObjectId;
    QString selectedObjectName;
    QString selectedObjectType;
    QString parentObjectId;

    foreach (QModelIndex index, selectedList)
    {
        QTableWidgetItem *itemFileName =
                ui->tblFiles->item(index.row(), 0);
        if (itemFileName != NULL)
        {
            if ((itemFileName->text() == "..")
                    && (index.row() == 0))
            {

            }
            else
            {
                QVariant vObjectId = itemFileName->data(Ui::ObjectIdRole);
                QVariant vParentObjectId = itemFileName->data(Ui::ParentObjectIdRole);
                QVariant vObjectType = itemFileName->data(Ui::ObjectTypeRole);
                selectedObjectId = vObjectId.toString();
                selectedObjectName = itemFileName->text();
                selectedObjectType = vObjectType.toString();
                parentObjectId = vParentObjectId.toString();
                totalFilesSelected++;
            }
        }
    }

    if (totalFilesSelected > 0)
    {
        QString msg = QString(tr("Rename %1 '%2' to:"))
                .arg(selectedObjectType.toLower())
                .arg(selectedObjectName);
        bool ok;
        QString newFileName = QInputDialog::getText(
                   this,
                   tr("Rename"),
                   msg,
                   QLineEdit::Normal,
                   selectedObjectName,
                   &ok);
        if (ok && !newFileName.isEmpty())
        {
            connect(&amazonWebSite, SIGNAL(onMoveById()), this, SLOT(onMoveById()), Qt::UniqueConnection);
            amazonWebSite.moveById(selectedObjectId, parentObjectId, newFileName, true);
        }
    }
}

void MainWindow::onMoveById()
{
    refreshCurrentFolder();
}
