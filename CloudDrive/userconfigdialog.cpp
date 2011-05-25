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

#include <QSettings>

#include "generalconfig.h"
#include "userconfigdialog.h"
#include "ui_userconfigdialog.h"

UserConfigDialog::UserConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserConfigDialog)
{
    ui->setupUi(this);
    setWindowTitle("CloudDrive Account Info");
    QSettings settings(ORGANIZATION, PRODUCT);
    ui->edtEmail->setText(settings.value(EMAIL_CONFIG_KEY, QString()).toString());
    ui->edtPassword->setText(settings.value(PASS_CONFIG_KEY, QString()).toString());
}

UserConfigDialog::~UserConfigDialog()
{
    delete ui;
}

void UserConfigDialog::on_buttonBox_accepted()
{
    QString userEmail = ui->edtEmail->text();
    QString userPass = ui->edtPassword->text();
    if (userEmail.trimmed().isEmpty()
            || userPass.trimmed().isEmpty())
    {
        return;
    }
    QSettings settings(ORGANIZATION, PRODUCT);
    settings.setValue(EMAIL_CONFIG_KEY, userEmail);
    settings.setValue(PASS_CONFIG_KEY, userPass);
    finished(QDialog::Accepted);
}

void UserConfigDialog::on_buttonBox_rejected()
{
    finished(QDialog::Rejected);
}
