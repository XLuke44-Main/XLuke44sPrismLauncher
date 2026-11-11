#ifndef QUA_ZIPNEWINFO_H
#define QUA_ZIPNEWINFO_H

/*
Copyright (C) 2005-2014 Sergey A. Tachenov

This file is part of QuaZip.

QuaZip is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

QuaZip is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QuaZip.  If not, see <http://www.gnu.org/licenses/>.

See COPYING file for the full LGPL text.

Original ZIP package is copyrighted by Gilles Vollant, see
quazip/(un)zip.h files for details, basically it's zlib license.
 **/

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QString>

#include "quazip_global.h"

#include "quazipfileinfo.h"

struct QUAZIP_EXPORT QuaZipNewInfo {

  QString name;

  QDateTime dateTime;

  quint16 internalAttr;

  quint32 externalAttr;

  QString comment;

  QByteArray extraLocal;

  QByteArray extraGlobal;

  ulong uncompressedSize;

  QuaZipNewInfo(const QString& name);

  /** Initializes name with \a name. Timestamp and permissions are taken
   * from the specified file. If the \a file does not exists or its timestamp
   * is inaccessible (e. g. you do not have read permission for the
   * directory file in), uses current time and zero permissions. Other attributes are
   * initialized with zeros, comment and extra field with null values.
   * 
   * \sa setFileDateTime()
   **/
  QuaZipNewInfo(const QString& name, const QString& file);

  QuaZipNewInfo(const QuaZipFileInfo &existing);

  QuaZipNewInfo(const QuaZipFileInfo64 &existing);

  void setFileDateTime(const QString& file);

  void setFilePermissions(const QString &file);

  void setPermissions(QFile::Permissions permissions);

  void setFileNTFSTimes(const QString &fileName);

  void setFileNTFSmTime(const QDateTime &mTime, int fineTicks = 0);

  void setFileNTFSaTime(const QDateTime &aTime, int fineTicks = 0);

  void setFileNTFScTime(const QDateTime &cTime, int fineTicks = 0);
};

#endif
