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

#include <QDebug>

#include "cloudutils.h"

void Utils::char2hex( QChar dec, QString &str )
{
    char dig1 = (dec.toLatin1() & 0xF0)>>4;
    char dig2 = (dec.toLatin1() & 0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1 += '0';    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1 += ('A' - 10); //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2 += '0';
    if (10<= dig2 && dig2<=15) dig2 += ('A' - 10);

    str.append( dig1);
    str.append( dig2);
}

QString Utils::urlEncode(const QByteArray &c)
{

    QString escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='(' || c[i]==')')
        )
        {
            escaped.append(c.at(i));
        }
        else if (c[i] == 0x20)
        {
            escaped.append('+');
        }
        else
        {
            escaped.append("%");
            char2hex(c.at(i), escaped);//converts char 255 to string "ff"
        }
    }
    return escaped;
}

QString Utils::urlEncode(const QString &c)
{

    QString escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='(' || c[i]==')')
        )
        {
            escaped.append(c.at(i).toLatin1());
        }
        else if (c[i] == 0x20)
        {
            escaped.append('+');
        }
        else
        {
            escaped.append("%");
            char2hex(c.at(i), escaped);//converts char 255 to string "ff"
        }
    }
    return escaped;
}

QString Utils::formatStorage(qlonglong storageSpace)
{
    QString result;
    if (storageSpace >= (1024 * 1024 * 1024))
    {
        double gigaStorage = (float)storageSpace / (1024.0f * 1024.0f * 1024.0f);
        result = QString("%1 GB").arg(gigaStorage, 0, 'f', 1);
    }
    else if (storageSpace >= (1024 * 1024))
    {
        double megaStorage = (float)storageSpace / (1024.0f * 1024.0f);
        result = QString("%1 MB").arg(megaStorage, 0, 'f', 1);
    }
    else if (storageSpace >= 1024)
    {
        double kiloStorage = (float)storageSpace / (1024.0f);
        result = QString("%1 KB").arg(kiloStorage, 0, 'f', 1);
    }
    else
    {
        result = QString("%1").arg(storageSpace);
    }
    return result;
}

QString Utils::decodeFileName(QString contentDisp)
{
    if (!contentDisp.trimmed().startsWith("attachment"))
    {
        return "";
    }
    if (contentDisp.contains("filename="))
    {
        int indexOf = contentDisp.indexOf("filename=");
        if (indexOf >= 0)
        {
            indexOf += 9;
            int lpar = contentDisp.indexOf('"', indexOf);
            if (lpar >= 0)
            {
                QString fileName = contentDisp;
                fileName.remove(0, lpar + 1);
                int rpar = fileName.indexOf('"');
                if (rpar >= 0)
                {
                    fileName.remove(rpar, fileName.length() - rpar);
                    return fileName;
                }
            }
            else
            {
                QString fileName = contentDisp;
                fileName.remove(0, indexOf);
                int i = fileName.indexOf(";");
                if (i >= 0)
                {
                    fileName.remove(i, fileName.length() - i);
                }
                return fileName;
            }
        }
    }
    return "";
}


QString Utils::extractFileName(const QString &fileName)
{
    QString name;
    if (fileName.contains("/"))
    {
        int pos = fileName.lastIndexOf("/");
        name = fileName.right(fileName.length() - pos - 1);

    }
    else if (fileName.contains("\\"))
    {
        int pos = fileName.lastIndexOf("\\");
        name = fileName.right(fileName.length() - pos - 1);
    }
    else
    {
        name = fileName;
    }
    return name;
}

QString Utils::extractFileDir(const QString &fileName)
{
    QString name;
    if (fileName.contains("/"))
    {
        int pos = fileName.lastIndexOf("/");
        name = fileName.left(pos + 1);

    }
    else if (fileName.contains("\\"))
    {
        int pos = fileName.lastIndexOf("\\");
        name = fileName.left(pos + 1);
    }
    else
    {
        name = fileName;
    }
    return name;
}

QDateTime Utils::decodeJsonDateTime(const QString &jsonDateTime)
{
    if (jsonDateTime.contains('.')
            && (jsonDateTime.contains('E') || jsonDateTime.contains('e')))
    {
        int indexOfDot = jsonDateTime.indexOf('.');
        if (indexOfDot >= 0)
        {
            int indexOfE = jsonDateTime.indexOf('E', indexOfDot + 1);
            if (indexOfE < 0)
            {
                indexOfE = jsonDateTime.indexOf('e', indexOfDot + 1);
            }
            if (indexOfE > 0)
            {
                quint64 res = 0;
                int i = 0;
                for (; i < indexOfDot; i++)
                {
                    QChar ch = jsonDateTime.at(i);
                    if (ch.isDigit())
                    {
                        res *= 10;
                        res += (ch.toAscii() - '0');
                    }
                    else
                    {
                        //invalid format
                        qDebug() << "Invalid date time format";
                        return QDateTime();
                    }
                }
                int tenths = 0;
                bool ok = 0;
                tenths = jsonDateTime.right(jsonDateTime.length() - indexOfE - 1).toInt(&ok);
                if (ok)
                {
                    i++;
                    for (; /*tenths >0 &&*/ i < indexOfE; i++, tenths--)
                    {
                        QChar ch = jsonDateTime.at(i);
                        if (ch.isDigit())
                        {
                            res *= 10;
                            res += (ch.toAscii() - '0');
                        }
                        else
                        {
                            //invalid format
                            qDebug() << "Invalid date time format";
                            return QDateTime();
                        }
                    }
                    int d = (indexOfE - indexOfDot);
                    if ((d > 0) && (d < 13))
                    {
                        d = 13 - d;
                        for ( ; d > 0; d--)
                        {
                            res *= 10;
                        }
                    }
                    QDateTime result;
                    result.setMSecsSinceEpoch(res);
                    return result;
                }
            }
        }
    }
    return QDateTime();
}
