/* This file is part of qjson
  *
  * Copyright (C) 2009 Till Adam <adam@kde.org>
  * Copyright (C) 2009 Flavio Castelli <flavio@castelli.name>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License version 2.1, as published by the Free Software Foundation.
  * 
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#include "serializer.h"

#include <QtCore/QDataStream>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <cmath>

#ifdef _MSC_VER  // using MSVC compiler
#include <float.h>
#endif

using namespace QJson;

class Serializer::SerializerPrivate {
  public:
    SerializerPrivate() : specialNumbersAllowed(false), indentMode(QJson::IndentNone) {}
    bool specialNumbersAllowed;
    IndentMode indentMode;
    QByteArray buildIndent(int spaces);
    QByteArray serialize( const QVariant &v, int reserved = 0);
    QString sanitizeString( QString str );
    QByteArray join( const QList<QByteArray>& list, const QByteArray& sep );
};

QByteArray Serializer::SerializerPrivate::join( const QList<QByteArray>& list, const QByteArray& sep ) {
  QByteArray res;
  Q_FOREACH( const QByteArray& i, list ) {
    if ( !res.isEmpty() )
      res += sep;
    res += i;
  }
  return res;
}

QByteArray Serializer::SerializerPrivate::serialize( const QVariant &v, int reserved )
{
  QByteArray str;
  bool error = false;
  QByteArray indent;

  if ( ! v.isValid() ) { // invalid or null?
    str = "null";
  } else if (( v.type() == QVariant::List ) || ( v.type() == QVariant::StringList )){ // an array or a stringlist?
    const QVariantList list = v.toList();
    QList<QByteArray> values;
    Q_FOREACH( const QVariant& var, list )
    {
      reserved++;
      QByteArray serializedValue = serialize( var,reserved );
      reserved--;
      if ( serializedValue.isNull() ) {
        error = true;
        break;
      }
      values << serializedValue;
    }

    if (indentMode == QJson::IndentMinimum) {
      QByteArray indent = buildIndent(reserved - 1);
      str = "[\n" + join( values, ",\n" ) + "\n" + indent + "]";
    }
    else if (indentMode == QJson::IndentMedium || indentMode == QJson::IndentFull){
      QByteArray indent = buildIndent(reserved);
      str = "[\n" + join( values, ",\n" ) + "\n" + indent + "]";
    }
    else {
      str = "[ " + join( values, ", " ) + " ]";
    }

  } else if ( v.type() == QVariant::Map ) { // variant is a map?
    const QVariantMap vmap = v.toMap();
    QMapIterator<QString, QVariant> it( vmap );

    if (indentMode == QJson::IndentMinimum) {
      QByteArray indent = buildIndent(reserved);
      str = indent + "{ ";
    }
    else if (indentMode == QJson::IndentMedium || indentMode == QJson::IndentFull) {
      QByteArray indent = buildIndent(reserved);
      QByteArray nextindent = buildIndent(reserved + 1);
      str = indent + "{\n" + nextindent;
    }
    else {
      str = "{ ";
    }

    QList<QByteArray> pairs;
    while ( it.hasNext() ) {
      it.next();
      reserved++;
      QByteArray serializedValue = serialize( it.value() , reserved);
      reserved--;
      if ( serializedValue.isNull() ) {
        error = true;
        break;
      }
      pairs << sanitizeString( it.key() ).toUtf8() + " : " + serializedValue;
    }

    if (indentMode == QJson::IndentFull) {
      QByteArray indent = buildIndent(reserved + 1);
      str += join( pairs, ",\n" + indent);
    }
    else {
      str += join( pairs, ", " );
    }

    if (indentMode == QJson::IndentMedium || indentMode == QJson::IndentFull) {
      QByteArray indent = buildIndent(reserved);
      str += "\n" + indent + "}";
    }
    else {
      str += " }";
    }

  } else if (( v.type() == QVariant::String ) ||  ( v.type() == QVariant::ByteArray )) { // a string or a byte array?
    str = sanitizeString( v.toString() ).toUtf8();
  } else if (( v.type() == QVariant::Double) || (v.type() == QMetaType::Float)) { // a double or a float?
    const double value = v.toDouble();
#if defined _WIN32 && !defined(Q_OS_SYMBIAN)
    const bool special = _isnan(value) || !_finite(value);
#elif defined(Q_OS_SYMBIAN)
    const bool special = isnan(value) || isinf(value);
#else
    const bool special = std::isnan(value) || std::isinf(value);
#endif
    if (special) {
      if (specialNumbersAllowed) {
#if defined _WIN32 && !defined(Q_OS_SYMBIAN)
        if (_isnan(value)) {
#elif defined(Q_OS_SYMBIAN)
        if (isnan(value)) {
#else
        if (std::isnan(value)) {
#endif
          str += "NaN";
        } else {
          if (value<0) {
            str += '-';
          }
          str += "Infinity";
        }
      } else {
        qCritical("Attempt to write NaN or infinity, which is not supported by json");
        error = true;
    }
    } else {
      str = QByteArray::number( value );
      if( ! str.contains( "." ) && ! str.contains( "e" ) ) {
        str += ".0";
      }
    }
  } else if ( v.type() == QVariant::Bool ) { // boolean value?
    str = ( v.toBool() ? "true" : "false" );
  } else if ( v.type() == QVariant::ULongLong ) { // large unsigned number?
    str = QByteArray::number( v.value<qulonglong>() );
  } else if ( v.canConvert<qlonglong>() ) { // any signed number?
    str = QByteArray::number( v.value<qlonglong>() );
  } else if ( v.canConvert<QString>() ){ // can value be converted to string?
    // this will catch QDate, QDateTime, QUrl, ...
    str = sanitizeString( v.toString() ).toUtf8();
    //TODO: catch other values like QImage, QRect, ...
  } else {
    error = true;
  }
  if ( !error )
  {
    return str;
  }
  else
    return QByteArray();
}

QByteArray Serializer::SerializerPrivate::buildIndent(int spaces)
{
   QByteArray indent;
   if (spaces < 0) {
     spaces = 0;
   }
   for (int i = 0; i < spaces; i++ ) {
     indent += " ";
   }
   return indent;
}

QString Serializer::SerializerPrivate::sanitizeString( QString str )
{
  str.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );

  // escape unicode chars
  QString result;
  const ushort* unicode = str.utf16();
  unsigned int i = 0;

  while ( unicode[ i ] ) {
    if ( unicode[ i ] < 128 ) {
      result.append( QChar( unicode[ i ] ) );
    }
    else {
      QString hexCode = QString::number( unicode[ i ], 16 ).rightJustified( 4,
                                                           QLatin1Char('0') );

      result.append( QLatin1String ("\\u") ).append( hexCode );
    }
    ++i;
  }
  str = result;

  str.replace( QLatin1String( "\"" ), QLatin1String( "\\\"" ) );
  str.replace( QLatin1String( "\b" ), QLatin1String( "\\b" ) );
  str.replace( QLatin1String( "\f" ), QLatin1String( "\\f" ) );
  str.replace( QLatin1String( "\n" ), QLatin1String( "\\n" ) );
  str.replace( QLatin1String( "\r" ), QLatin1String( "\\r" ) );
  str.replace( QLatin1String( "\t" ), QLatin1String( "\\t" ) );

  return QString( QLatin1String( "\"%1\"" ) ).arg( str );
}

Serializer::Serializer()
  : d( new SerializerPrivate )
{
}

Serializer::~Serializer() {
  delete d;
}

void Serializer::serialize( const QVariant& v, QIODevice* io, bool* ok)
{
  Q_ASSERT( io );
  if (ok)
      *ok = false;

  if (!io->isOpen()) {
    if (!io->open(QIODevice::WriteOnly)) {
      qCritical ("Error opening device");
      return;
    }
  }

  if (!io->isWritable()) {
    qCritical ("Device is not readable");
    io->close();
    return;
  }

  const QByteArray str = serialize( v );
  if (io->write(str) == str.count()) {
    if (ok)
      *ok = true;
  }
}

QByteArray Serializer::serialize( const QVariant &v)
{
  return d->serialize(v);
}

void QJson::Serializer::allowSpecialNumbers(bool allow) {
  d->specialNumbersAllowed = allow;
}

bool QJson::Serializer::specialNumbersAllowed() const {
  return d->specialNumbersAllowed;
}

void QJson::Serializer::setIndentMode(IndentMode mode) {
  d->indentMode = mode;
}

IndentMode QJson::Serializer::indentMode() const {
  return d->indentMode;
}
