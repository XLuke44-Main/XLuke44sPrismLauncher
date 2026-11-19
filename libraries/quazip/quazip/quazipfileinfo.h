#ifndef QUA_ZIPFILEINFO_H
#define QUA_ZIPFILEINFO_H

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

Original ZIP package is copyrighted by Gilles Vollant and contributors,
see quazip/(un)zip.h files for details. Basically it's the zlib license.
*/

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QHash>

#include "quazip_global.h"

typedef QHash<quint16, QList<QByteArray> > QuaExtraFieldHash;

struct QUAZIP_EXPORT QuaZipFileInfo {

  QString name;

  quint16 versionCreated;

  quint16 versionNeeded;

  quint16 flags;

  quint16 method;

  QDateTime dateTime;

  quint32 crc;

  quint32 compressedSize;

  quint32 uncompressedSize;

  quint16 diskNumberStart;

  quint16 internalAttr;

  quint32 externalAttr;

  QString comment;

  QByteArray extra;

  QFile::Permissions getPermissions() const;
};

struct QUAZIP_EXPORT QuaZipFileInfo64 {

  QString name;

  quint16 versionCreated;

  quint16 versionNeeded;

  quint16 flags;

  quint16 method;

  QDateTime dateTime;

  quint32 crc;

  quint64 compressedSize;

  quint64 uncompressedSize;

  quint16 diskNumberStart;

  quint16 internalAttr;

  quint32 externalAttr;

  QString comment;

  QByteArray extra;

  QFile::Permissions getPermissions() const;

  bool isSymbolicLink() const;

  bool toQuaZipFileInfo(QuaZipFileInfo &info) const;

  QDateTime getNTFSmTime(int *fineTicks = nullptr) const;

  QDateTime getNTFSaTime(int *fineTicks = nullptr) const;

  QDateTime getNTFScTime(int *fineTicks = nullptr) const;

  QDateTime getExtModTime() const;

  bool isEncrypted() const {return (flags & 1) != 0;}

  static QuaExtraFieldHash parseExtraField(const QByteArray &extraField);

  static QDateTime getExtTime(const QByteArray &extra, int flag);
};

#endif
