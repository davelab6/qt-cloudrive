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

#ifndef __FORMPOST_H__
#define __FORMPOST_H__

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QProgressBar>
#include <QDateTime>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "httppostsendbuffer.h"

class FormPost: public QObject
{
  Q_OBJECT

  public:
    FormPost();
    QString userAgent();
    void setUserAgent(QString agent);
    void setNetworkAccessManager(QNetworkAccessManager * networkAccessManager);
    QString referer();
    void setReferer(QString ref);
    void addField(QString name, QString value);
    void setFile(QString fieldName, QString fileName, QString mime);
    QNetworkReply * postData(QString url);

  private:
    QNetworkAccessManager * networkAccessManager;    
    HttpPostSendbuffer postSendBuffer;
    QString userAgentS;
    QString refererS;

 // private slots:
 //   void readData(QNetworkReply * r);
};

#endif
