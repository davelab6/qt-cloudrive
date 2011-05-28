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

#ifndef USERCONFIGDIALOG_H
#define USERCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class UserConfigDialog;
}

class UserConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserConfigDialog(QWidget *parent = 0);
    ~UserConfigDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::UserConfigDialog *ui;
};

#endif // USERCONFIGDIALOG_H
