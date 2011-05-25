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

#include "qurl2.h"

QUrl2::QUrl2(const QByteArray &rawUrl)
{
    /*
    scheme://username:password@domain:port/path?query_string#fragment_id
    */
    QString url = QString(rawUrl);

    if (url.startsWith("https://"))
    {
        url = url.right(url.length() - 8);
    }
    else if (url.startsWith("http://"))
    {
        url = url.right(url.length() - 7);
    }
    else
    {
        return;
    }
    int indexOfSlash = url.indexOf("/");
    if (indexOfSlash >= 0)
    {
        host = url.left(indexOfSlash);
        httpObject = url.right(url.length() - indexOfSlash);
    }
    else
    {
        host = url;
        httpObject = "/";
    }
}

QString QUrl2::getHost()
{
    return host;
}

QString QUrl2::getHttpObject()
{
    return httpObject;
}
