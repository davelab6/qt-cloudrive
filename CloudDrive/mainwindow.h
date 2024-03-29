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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>

#include "deferredmimedata.h"
#include "amazonwebsite.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

public slots:
    void onErrorInUserSignedIn(const QString &errorMessage);
    void onUserSignedIn(const QString &customerId, const QString &sessionId);
    void onGetRootPathObjectId(const QString &objectId);
    void onListObjects(const QList<CloudDriveFileObject> &objectList);    
    void onDownloadProgress(const QString &fileName, qint64 bytesReceived, qint64 bytesTotal);
    void onUploadProgress(const QString &fileName, qint64 bytesSent, qint64 bytesTotal);
    void onFileDownloaded(const QString &contentType, qlonglong contentLength,
                          const QString &fileName, QIODevice *inputStream,
                          const QString &fileDestDir);
    void userStorage(qlonglong freeSpace, qlonglong totalSpace, qlonglong usedSpace);
    void onFileUploaded(const QString &fileName);
    void onFolderCreated(const QString &folderObjectId);
    void onBulkRemovedById();
    void onBulkRecycledById();
    void onGotDownloadUrl(const QByteArray &downloadURL);

private slots:    

    void jsonOpError(QString errorCode, QString errorMessage);

    void on_actionE_xit_triggered();

    void on_action_Upload_triggered();

    void on_action_Download_triggered();

    void on_action_Account_triggered();

    void on_action_Refresh_triggered();

    void on_actionAbout_triggered();

    void on_actionCreate_Folder_triggered();

    void on_tblFiles_itemSelectionChanged();

    void on_actionDelete_triggered();

    void on_actionPlay_triggered();

    void on_btnPlay_clicked();

    void on_tblFiles_doubleClicked(QModelIndex index);

    void itemDragged(const QString& objectId, const QString& fileName);

    void mimeDargDataRequested(const QString &downloadObjectId, const QString &downloadFileName);

    void itemsDropped(const QList<QUrl>& urlList);

    void on_actionRename_triggered();

    void onMoveById();

protected:
    void initForm();
    void closeEvent(QCloseEvent *event);    
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Ui::MainWindow *ui;

    AmazonWebsite amazonWebSite;
    QString customerId;
    QString sessionId;
    QString rootObjectId;

    QStack<QString> parentObjectIds;
    QStack<QString> parentObjectPaths;
    QString lastDownloadObjectId;

    void openFolder(
        QString folderObjectName,
        QString folderObjectId,
        int row);
    void refreshCurrentFolder();
    void createBackItem();
    bool downloadFile(const QString &sourceFileName,
                      const QString &sourceFileObjectId,
                      const QString &destinationDir);
    QString getCurrentRemotePath();    
    void displayObjects(const QList<CloudDriveFileObject> &objectList);   
    void downloadFileByItemIndex(const QModelIndex & index);

signals:
    void newFilesOnUploadQueue();

};

#endif // MAINWINDOW_H
