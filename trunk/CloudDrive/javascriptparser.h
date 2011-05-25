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

#ifndef JAVASCRIPTPARSER_H
#define JAVASCRIPTPARSER_H

#include <QObject>
#include <QIODevice>

#define MAX_TOKEN_SIZE 4096

class JavaScriptParser: public QObject
{
    Q_OBJECT

public:
    JavaScriptParser();

public:
    bool parseHtmlPage(QIODevice *iodevice, const char *varName, char *varValue);
    void setVarsFilter(const char **varElements);

signals:
    void onScriptVariable(QString variableName, QString variableValue);

private:
    const char **varElements;
};

#endif // JAVASCRIPTPARSER_H
