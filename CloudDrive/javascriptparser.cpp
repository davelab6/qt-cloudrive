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

#include "javascriptparser.h"

const int MaxTokenLength = 4096;

JavaScriptParser::JavaScriptParser()
{
}

inline bool getNextToken(QIODevice *iodevice, char *tokenBuf)
{
    int tokenBufPos = 0;
    bool inToken = false;
    bool isStringLiteral = false;
    char currChar = 0x00;
    for ( ; ; )
    {
        if (!iodevice->getChar(&currChar))
        {
            break;
        }
        if (inToken && (isspace(currChar) || iscntrl(currChar) || (currChar == ';')))
        {
            inToken = false;
            return true;
        }
        else if (!inToken && (currChar=='"'))
        {
            inToken = true;
            isStringLiteral = true;
        }
        else if (isprint(currChar))
        {
            inToken = true;
            if ((currChar == '"') && isStringLiteral)
            {
                return true;
            }
            tokenBuf[tokenBufPos] = currChar;
            tokenBufPos++;
            if (tokenBufPos == MaxTokenLength)
            {
                tokenBufPos = 0;
            }
            tokenBuf[tokenBufPos] = 0x00;
        }
    }
    if (tokenBufPos > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool JavaScriptParser::parseHtmlPage(QIODevice *iodevice, const char *varName, char *varValue)
{
    char tokenBuf[MaxTokenLength];
    /*
    ADriveServer = "/clouddrive/api/";
    MusicServiceURL = "/gp/dmusic/mp3/player"
    ubid = "191-5957700-0635023";
    */
    bool tokenFound = false;
    for ( ; ; )
    {
        if (getNextToken(iodevice, &tokenBuf[0]))
        {
            if (strcmp(&tokenBuf[0], varName) == 0)
            {
                tokenFound = true;
                break;
            }
        }
        else
        {
            break;
        }
    }
    if (tokenFound)
    {
        tokenFound = false;
        if (getNextToken(iodevice, &tokenBuf[0]))
        {
            if (strcmp(&tokenBuf[0], "=") == 0)
            {
                tokenFound = true;
            }
        }
    }
    if (tokenFound)
    {
        tokenFound = getNextToken(iodevice, &tokenBuf[0]);
        if (tokenFound)
        {
            if ((tokenBuf[0] == '"') || (tokenBuf[0] == '\''))
            {
                int i;
                for (i = 0; ; i++)
                {
                    if (tokenBuf[i + 1] == 0x00)
                    {
                        tokenBuf[i] = 0x00;
                        break;
                    }
                    else
                    {
                        tokenBuf[i] = tokenBuf[i + 1];
                    }
                }
            }
            strcpy(varValue, &tokenBuf[0]);
        }
        else
        {
            varValue[0] = 0x00;
        }
    }
    return tokenFound;
}

void JavaScriptParser::setVarsFilter(const char **varElements)
{
    this->varElements = varElements;
}

