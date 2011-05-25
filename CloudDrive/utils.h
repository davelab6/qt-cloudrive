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

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QByteArray>

void char2hex( QChar dec, QString &str );
QString urlEncode(const QByteArray &c);
QString urlEncode(const QString &c);
QString formatStorage(qlonglong storageSpace);
QString decodeFileName(QString contentDisp);
QString extractFileName(const QString &fileName);
QString extractFileDir(const QString &fileName);

#endif // UTILS_H
