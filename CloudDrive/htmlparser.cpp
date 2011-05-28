
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

#include "htmlparser.h"

const int MaxHtmlElementLength = 4096;

HtmlParser::HtmlParser()
{    
    filterElements = NULL;
}

void HtmlParser::setElementsFilter(const char **filterElements)
{
    this->filterElements = filterElements;
}

bool isSpecialChar(char ch)
{
	return ((ch >= 0x00) && (ch <= 0x1F));
}

bool HtmlParser::parseAttributes(const char *elementData, QList<QHtmlAttribute *> &attributes)
{
    int state = 0;
    char currChar = 0x00;
    int currPos = 0;
    char currAttribName[MaxHtmlElementLength];
    int currAttribNamePos = 0;
    char currAttribVal[MaxHtmlElementLength];
    int currAttribValPos = 0;
    for ( ; ; )
    {
        currChar = elementData[currPos++];
        if (currChar == 0x00)
        {
            break;
        }
        switch (state)
        {
            case 0:
                if (isalnum(currChar) && !isspace(currChar) && (currChar != ' '))
                {
                    currAttribName[currAttribNamePos] = currChar;
                    currAttribNamePos++;
                    if (currAttribNamePos == MaxHtmlElementLength)
                    {
                        //RESET
                        currAttribNamePos = 0;
                        currAttribName[currAttribNamePos] = 0x00;
                        currAttribValPos = 0;
                        currAttribVal[currAttribValPos] = 0x00;
                        state = 0;
                    }
                    currAttribName[currAttribNamePos] = 0x00;
                }
                else if (currChar == '=')
                {
                    state = 1;
                }
                else
                {
                    //PROCESS ONLY NAME
                    if (currAttribNamePos > 0)
                    {
                        QHtmlAttribute *attribute = new QHtmlAttribute();
                        attribute->first = QString(currAttribName);
                        attribute->second = QString();
                        attributes.append(attribute);
                        //qDebug() << "A" << "N:" << currAttribName;
                    }
                    //END OF PROCESS ONLY NAME
                    currAttribNamePos = 0;
                    currAttribName[currAttribNamePos] = 0x00;
                    currAttribValPos = 0;
                    currAttribVal[currAttribValPos] = 0x00;
                    state = 0;
                }
                break;

            case 1:
                if (isspace(currChar) || (currChar == ' ') || (isSpecialChar(currChar)))
                {
                    continue;
                }
                else if (currChar == '"')
                {
                    state = 4;
                    continue;
                }
                else if (currChar == '\'')
                {
                    state = 5;
                    continue;
                }
                else
                {
                    currAttribVal[currAttribValPos] = currChar;
                    currAttribValPos++;
                    if (currAttribValPos == MaxHtmlElementLength)
                    {
                        //RESET
                        currAttribNamePos = 0;
                        currAttribName[currAttribNamePos] = 0x00;
                        currAttribValPos = 0;
                        currAttribVal[currAttribValPos] = 0x00;
                        state = 0;
                    }
                    currAttribVal[currAttribValPos] = 0x00;
                    state = 2;
                }
                break;

             case 2:
                if (isspace(currChar) || (currChar == ' ') || (isSpecialChar(currChar)))
                {
                    //PROCESS NAME:VAL
                    if ((currAttribNamePos > 0) /*&& (currAttribValPos > 0)*/)
                    {
                        QHtmlAttribute *attribute = new QHtmlAttribute();
                        attribute->first = QString(currAttribName);
                        attribute->second = QString(currAttribVal);
                        attributes.append(attribute);
                        //qDebug() << "A" << "N:" << currAttribName << "V:" << currAttribVal;
                    }
                    //END OF PROCESS NAME:VAL
                    currAttribNamePos = 0;
                    currAttribName[currAttribNamePos] = 0x00;
                    currAttribValPos = 0;
                    currAttribVal[currAttribValPos] = 0x00;
                    state = 0;
                }
                else
                {
                    currAttribVal[currAttribValPos] = currChar;
                    currAttribValPos++;
                    if (currAttribValPos == MaxHtmlElementLength)
                    {
                        //RESET
                        currAttribNamePos = 0;
                        currAttribName[currAttribNamePos] = 0x00;
                        currAttribValPos = 0;
                        currAttribVal[currAttribValPos] = 0x00;
                        state = 0;
                    }
                    currAttribVal[currAttribValPos] = 0x00;
                }
                break;

        case 4:
                if (isSpecialChar(currChar))
                {
                    continue;
                }
                else if (currChar == '"')
                {
                    //PROCESS NAME:VAL
                    if ((currAttribNamePos > 0) /*&& (currAttribValPos > 0)*/)
                    {
                        QHtmlAttribute *attribute = new QHtmlAttribute();
                        attribute->first = QString(currAttribName);
                        attribute->second = QString(currAttribVal);
                        attributes.append(attribute);
                        //qDebug() << "A" << "N:" << currAttribName << "V:" << currAttribVal;
                    }
                    //END OF PROCESS NAME:VAL
                    currAttribNamePos = 0;
                    currAttribName[currAttribNamePos] = 0x00;
                    currAttribValPos = 0;
                    currAttribVal[currAttribValPos] = 0x00;
                    state = 0;
                }
                else
                {
                    currAttribVal[currAttribValPos] = currChar;
                    currAttribValPos++;
                    if (currAttribValPos == MaxHtmlElementLength)
                    {
                        //RESET
                        currAttribNamePos = 0;
                        currAttribName[currAttribNamePos] = 0x00;
                        currAttribValPos = 0;
                        currAttribVal[currAttribValPos] = 0x00;
                        state = 0;
                    }
                    currAttribVal[currAttribValPos] = 0x00;
                }
            break;

        case 5:
                if (isSpecialChar(currChar))
                {
                    continue;
                }
                else if (currChar == '\'')
                {
                    //PROCESS NAME:VAL
                    if ((currAttribNamePos > 0) /*&& (currAttribValPos > 0)*/)
                    {
                        QHtmlAttribute *attribute = new QHtmlAttribute();
                        attribute->first = QString(currAttribName);
                        attribute->second = QString(currAttribVal);
                        attributes.append(attribute);
                        //qDebug() << "A" << "N:" << currAttribName << "V:" << currAttribVal;
                    }
                    //END OF PROCESS NAME:VAL
                    currAttribNamePos = 0;
                    currAttribName[currAttribNamePos] = 0x00;
                    currAttribValPos = 0;
                    currAttribVal[currAttribValPos] = 0x00;
                    state = 0;
                }
                else
                {
                    currAttribVal[currAttribValPos] = currChar;
                    currAttribValPos++;
                    if (currAttribValPos == MaxHtmlElementLength)
                    {
                        //RESET
                        currAttribNamePos = 0;
                        currAttribName[currAttribNamePos] = 0x00;
                        currAttribValPos = 0;
                        currAttribVal[currAttribValPos] = 0x00;
                        state = 0;
                    }
                    currAttribVal[currAttribValPos] = 0x00;
                }
            break;
        }
    }
    //PROCESS NAME:VAL
    if ((currAttribNamePos > 0) /*&& (currAttribValPos > 0)*/)
    {
        QHtmlAttribute *attribute = new QHtmlAttribute();
        attribute->first = QString(currAttribName);
        attribute->second = QString(currAttribVal);
        attributes.append(attribute);
        //qDebug() << "A" << "N:" << currAttribName << "V:" << currAttribVal;
    }
    //END OF PROCESS NAME:VAL
    return true;
}

bool HtmlParser::parseHtmlElement(char *elementName, char *elementData)
{
    if (elementName != NULL)
    {
        if (elementData != NULL)
        {
            if (filterElements != NULL)
            {
                for (const char **filterElement = filterElements; *filterElement != NULL; ++filterElement)
                {
                    if (_stricmp(elementName, *filterElement) == 0)
                    {
                        //qDebug() << "E:" << elementName << "D:" << elementData;
                        //if (parserListener != NULL)
                        {
                            QList<QHtmlAttribute *> elementAttributes;
                            parseAttributes(elementData, elementAttributes);
                            emit onHtmlElement(elementName, elementAttributes);
                        }
                    }
                }
            }
            else
            {
                //if (parserListener != NULL)
                {
                    //qDebug();
                    //qDebug() << "E:" << elementName << "D:" << elementData;
                    QList<QHtmlAttribute *> elementAttributes;
                    emit onHtmlElement(elementData, elementAttributes);
                    //qDebug();
                }
            }
        }
        else
        {
          //  qDebug() << "E:" << elementName;
        }
    }
    return true;
}

bool HtmlParser::parseHtmlPage(QIODevice *iodevice)
{
    char currElementName[MaxHtmlElementLength];
    int currElementNamePos = 0;
    char currElementData[MaxHtmlElementLength];
    int currElementDataPos = 0;

    char currChar = 0x00;
    ParserState parserState = BeforeElement;
    currElementName[currElementNamePos] = 0x00;
    currElementData[currElementDataPos] = 0x00;
    for ( ; ; )
    {
        if (iodevice->getChar(&currChar))
        {
            if (isSpecialChar(currChar))
            {
                continue;
            }         
            switch (parserState)
            {
                case BeforeElement:
                    if (currChar == '<')
                    {
                        parserState = InElementName;
                        currElementNamePos = 0;
                        currElementDataPos = 0;
                        currElementName[currElementNamePos] = 0x00;
                        currElementData[currElementDataPos] = 0x00;
                    }
                    break;

                 case InElementName:
                    if ((currChar == '!') && (currElementNamePos == 0))
                    {
                        parserState = SkipMode;
                    }
                    else if (isalpha(currChar) && !isspace(currChar))
                    {
                        currElementName[currElementNamePos] = currChar;
                        currElementNamePos++;
                        if (currElementNamePos == MaxHtmlElementLength)
                        {
                            //buffer overflow, reset state machine
                            parserState = BeforeElement;
                            currElementNamePos = 0;
                            currElementDataPos = 0;
                            currElementName[currElementNamePos] = 0x00;
                            currElementData[currElementDataPos] = 0x00;
                        }
                        currElementName[currElementNamePos] = 0x00;
                    }
                    else if (isspace(currChar))
                    {
                        if (currElementNamePos > 0)
                        {
                            parserState = InElementData;
                            int currElementDataPos = 0;
                            currElementData[currElementDataPos] = 0x00;
                        }
                    }
                    else if (currChar == '>')
                    {
                        parseHtmlElement(&currElementName[0], NULL);
                        //reset state machine
                        parserState = BeforeElement;
                        currElementNamePos = 0;
                        currElementDataPos = 0;
                        currElementName[currElementNamePos] = 0x00;
                        currElementData[currElementDataPos] = 0x00;

                    }
                    else
                    {
                        //invalid char in this state
                        //reset state machine
                        parserState = BeforeElement;
                        currElementNamePos = 0;
                        currElementDataPos = 0;
                        currElementName[currElementNamePos] = 0x00;
                        currElementData[currElementDataPos] = 0x00;
                    }
                    break;

                   case InElementData:
                    if (currChar != '>')
                    {
                        currElementData[currElementDataPos] = currChar;
                        currElementDataPos++;
                        if (currElementDataPos == MaxHtmlElementLength)
                        {
                            //buffer overflow, reset state machine
                            parserState = BeforeElement;
                            currElementNamePos = 0;
                            currElementDataPos = 0;
                            currElementName[currElementNamePos] = 0x00;
                            currElementData[currElementDataPos] = 0x00;
                        }
                        currElementData[currElementDataPos] = 0x00;
                    }
                    else if (currChar == '>')
                    {                        
                        parseHtmlElement(&currElementName[0], &currElementData[0]);
                        //reset state machine
                        parserState = BeforeElement;
                        currElementNamePos = 0;
                        currElementDataPos = 0;
                        currElementName[currElementNamePos] = 0x00;
                        currElementData[currElementDataPos] = 0x00;
                    }
                    break;

                case SkipMode:
                    //consume this element
                    if (currChar == '>')
                    {
                        //reset state machine
                        parserState = BeforeElement;
                        currElementNamePos = 0;
                        currElementDataPos = 0;
                        currElementName[currElementNamePos] = 0x00;
                        currElementData[currElementDataPos] = 0x00;
                    }
                break;
            }
        }
        else
        {
            break;
        }
    }
    return true;
}

