/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue May 3 02:23:33 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

#include "filetablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionE_xit;
    QAction *action_Account;
    QAction *action_Upload;
    QAction *action_Download;
    QAction *action_Refresh;
    QAction *actionAbout;
    QAction *actionDelete;
    QAction *actionCreate_Folder;
    QAction *actionRename;
    QAction *actionPlay;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    FileTableWidget *tblFiles;
    QFrame *frmPlayMusic;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnPlay;
    QSlider *sldrTrack;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Tools;
    QMenu *menuHelp;
    QMenu *menuOperations;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(612, 492);
        actionE_xit = new QAction(MainWindow);
        actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
        action_Account = new QAction(MainWindow);
        action_Account->setObjectName(QString::fromUtf8("action_Account"));
        action_Upload = new QAction(MainWindow);
        action_Upload->setObjectName(QString::fromUtf8("action_Upload"));
        action_Download = new QAction(MainWindow);
        action_Download->setObjectName(QString::fromUtf8("action_Download"));
        action_Refresh = new QAction(MainWindow);
        action_Refresh->setObjectName(QString::fromUtf8("action_Refresh"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionDelete = new QAction(MainWindow);
        actionDelete->setObjectName(QString::fromUtf8("actionDelete"));
        actionCreate_Folder = new QAction(MainWindow);
        actionCreate_Folder->setObjectName(QString::fromUtf8("actionCreate_Folder"));
        actionRename = new QAction(MainWindow);
        actionRename->setObjectName(QString::fromUtf8("actionRename"));
        actionPlay = new QAction(MainWindow);
        actionPlay->setObjectName(QString::fromUtf8("actionPlay"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tblFiles = new FileTableWidget(centralWidget);
        if (tblFiles->columnCount() < 4)
            tblFiles->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblFiles->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tblFiles->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tblFiles->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tblFiles->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        tblFiles->setObjectName(QString::fromUtf8("tblFiles"));
        tblFiles->setAutoFillBackground(true);
        tblFiles->setSelectionMode(QAbstractItemView::SingleSelection);

        verticalLayout->addWidget(tblFiles);

        frmPlayMusic = new QFrame(centralWidget);
        frmPlayMusic->setObjectName(QString::fromUtf8("frmPlayMusic"));
        frmPlayMusic->setFrameShape(QFrame::StyledPanel);
        frmPlayMusic->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frmPlayMusic);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btnPlay = new QPushButton(frmPlayMusic);
        btnPlay->setObjectName(QString::fromUtf8("btnPlay"));

        horizontalLayout->addWidget(btnPlay);

        sldrTrack = new QSlider(frmPlayMusic);
        sldrTrack->setObjectName(QString::fromUtf8("sldrTrack"));
        sldrTrack->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(sldrTrack);


        verticalLayout->addWidget(frmPlayMusic);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 612, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Tools = new QMenu(menuBar);
        menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuOperations = new QMenu(menuBar);
        menuOperations->setObjectName(QString::fromUtf8("menuOperations"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Tools->menuAction());
        menuBar->addAction(menuOperations->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menu_File->addAction(action_Upload);
        menu_File->addAction(action_Download);
        menu_File->addSeparator();
        menu_File->addAction(action_Refresh);
        menu_File->addSeparator();
        menu_File->addAction(actionE_xit);
        menu_File->addSeparator();
        menu_Tools->addAction(action_Account);
        menuHelp->addAction(actionAbout);
        menuOperations->addAction(actionCreate_Folder);
        menuOperations->addSeparator();
        menuOperations->addAction(actionDelete);
        menuOperations->addSeparator();
        menuOperations->addAction(actionRename);
        menuOperations->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionE_xit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
        action_Account->setText(QApplication::translate("MainWindow", "&Account", 0, QApplication::UnicodeUTF8));
        action_Upload->setText(QApplication::translate("MainWindow", "&Upload", 0, QApplication::UnicodeUTF8));
        action_Download->setText(QApplication::translate("MainWindow", "&Download", 0, QApplication::UnicodeUTF8));
        action_Refresh->setText(QApplication::translate("MainWindow", "&Refresh", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionDelete->setText(QApplication::translate("MainWindow", "Delete", 0, QApplication::UnicodeUTF8));
        actionCreate_Folder->setText(QApplication::translate("MainWindow", "Create Folder", 0, QApplication::UnicodeUTF8));
        actionRename->setText(QApplication::translate("MainWindow", "Rename", 0, QApplication::UnicodeUTF8));
        actionPlay->setText(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tblFiles->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "File Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tblFiles->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tblFiles->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "Size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tblFiles->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "Modified", 0, QApplication::UnicodeUTF8));
        btnPlay->setText(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_Tools->setTitle(QApplication::translate("MainWindow", "&Tools", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        menuOperations->setTitle(QApplication::translate("MainWindow", "Operation", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
