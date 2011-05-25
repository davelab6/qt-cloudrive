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

#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QIODevice>
#include <QList>
#include <QPair>
#include <QString>

#define MAX_ELEMENT_SIZE 4096

typedef QPair<QString, QString> QHtmlAttribute;

typedef enum tagParserState { BeforeElement = 0, InElementName = 1, InElementData = 2, SkipMode = 3 } ParserState;

class HtmlParser: public QObject
{
    Q_OBJECT

public:
    HtmlParser();

public:
    bool parseHtmlPage(QIODevice *iodevice);
    void setElementsFilter(const char **filterElements);

private:
    bool parseHtmlElement(char *elementName, char *elementData);
    bool parseAttributes(const char *elementData, QList<QHtmlAttribute *> &attributes);

signals:
    void onHtmlElement(QString elementName, QList<QHtmlAttribute *> elementAttributes);

private:        
    const char **filterElements;    
};

#endif // HTMLPARSER_H
