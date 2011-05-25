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

#include "formpost.h"
#include <QDebug>

FormPost::FormPost():QObject(0)
{
  userAgentS = "";
  refererS = "";
  networkAccessManager = NULL;  
  postSendBuffer.open(QBuffer::ReadWrite);
}

QString FormPost::userAgent() {
  return userAgentS;
}

void FormPost::setUserAgent(QString agent) {
  userAgentS=agent;
}

void FormPost::setNetworkAccessManager(QNetworkAccessManager * nam)
{
    networkAccessManager = nam;
}

QString FormPost::referer() {
  return refererS;
}

void FormPost::setReferer(QString ref) {
  refererS=ref;
}

void FormPost::addField(QString name, QString value)
{
    postSendBuffer.addField(name, value);
}

void FormPost::setFile(QString fieldName, QString fileName, QString mime)
{
    postSendBuffer.setFile(fieldName, fileName, mime);
}

QNetworkReply * FormPost::postData(QString url)
{
    if (networkAccessManager == NULL)
    {
        return NULL;
    }

  QString host = url.right(url.length()-url.indexOf("://")-3);
  host = host.left(host.indexOf("/"));
  QString contentType;
  qlonglong contentLength;

  postSendBuffer.buildPostTemplate(contentType, contentLength);
  QNetworkRequest request;
  request.setRawHeader("Host", host.toAscii());
  if (userAgentS!="") request.setRawHeader("User-Agent", userAgentS.toAscii());
  if (refererS!="") request.setRawHeader("Referer", refererS.toAscii());
  request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toAscii());
  request.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(contentLength));
  request.setUrl(QUrl(url));  
  postSendBuffer.seek(0);
  QNetworkReply * reply = networkAccessManager->post(request, &postSendBuffer);
  return reply;

}
