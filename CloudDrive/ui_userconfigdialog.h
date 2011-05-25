/********************************************************************************
** Form generated from reading UI file 'userconfigdialog.ui'
**
** Created: Tue May 3 02:23:33 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERCONFIGDIALOG_H
#define UI_USERCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserConfigDialog
{
public:
    QVBoxLayout *verticalLayout_5;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *lblEmail;
    QLineEdit *edtEmail;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lblPassword;
    QLineEdit *edtPassword;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *UserConfigDialog)
    {
        if (UserConfigDialog->objectName().isEmpty())
            UserConfigDialog->setObjectName(QString::fromUtf8("UserConfigDialog"));
        UserConfigDialog->resize(377, 187);
        verticalLayout_5 = new QVBoxLayout(UserConfigDialog);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        widget = new QWidget(UserConfigDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(100, 20));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lblEmail = new QLabel(widget);
        lblEmail->setObjectName(QString::fromUtf8("lblEmail"));

        horizontalLayout->addWidget(lblEmail);

        edtEmail = new QLineEdit(widget);
        edtEmail->setObjectName(QString::fromUtf8("edtEmail"));

        horizontalLayout->addWidget(edtEmail);


        verticalLayout_5->addWidget(widget);

        widget_2 = new QWidget(UserConfigDialog);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMinimumSize(QSize(300, 30));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lblPassword = new QLabel(widget_2);
        lblPassword->setObjectName(QString::fromUtf8("lblPassword"));

        horizontalLayout_2->addWidget(lblPassword);

        edtPassword = new QLineEdit(widget_2);
        edtPassword->setObjectName(QString::fromUtf8("edtPassword"));
        edtPassword->setEchoMode(QLineEdit::Password);

        horizontalLayout_2->addWidget(edtPassword);


        verticalLayout_5->addWidget(widget_2);

        buttonBox = new QDialogButtonBox(UserConfigDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_5->addWidget(buttonBox);


        retranslateUi(UserConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UserConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UserConfigDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UserConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *UserConfigDialog)
    {
        UserConfigDialog->setWindowTitle(QApplication::translate("UserConfigDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        lblEmail->setText(QApplication::translate("UserConfigDialog", "E-Mail:", 0, QApplication::UnicodeUTF8));
        lblPassword->setText(QApplication::translate("UserConfigDialog", "Password:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UserConfigDialog: public Ui_UserConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERCONFIGDIALOG_H
