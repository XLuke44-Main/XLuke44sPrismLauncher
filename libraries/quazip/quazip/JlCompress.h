#ifndef JLCOMPRESSFOLDER_H_
#define JLCOMPRESSFOLDER_H_

/*
Copyright (C) 2010 Roberto Pompermaier
Copyright (C) 2005-2016 Sergey A. Tachenov

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

#include "quazip.h"
#include "quazipfile.h"
#include "quazipfileinfo.h"
#include "quazip_qt_compat.h"
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>

class QUAZIP_EXPORT JlCompress {
public:
    static bool copyData(QIODevice &inFile, QIODevice &outFile);
    static QStringList extractDir(QuaZip &zip, const QString &dir);
    static QStringList getFileList(QuaZip *zip);
    static QString extractFile(QuaZip &zip, QString fileName, QString fileDest);
    static QStringList extractFiles(QuaZip &zip, const QStringList &files, const QString &dir);

    static bool compressFile(QuaZip* zip, QString fileName, QString fileDest);

    static bool compressSubDir(QuaZip* parentZip, QString dir, QString parentDir, bool recursive,
                               QDir::Filters filters);

    static bool extractFile(QuaZip* zip, QString fileName, QString fileDest);

    static bool removeFile(QStringList listFile);

    static bool compressFile(QString fileCompressed, QString file);

    static bool compressFiles(QString fileCompressed, QStringList files);

    static bool compressDir(QString fileCompressed, QString dir = QString(), bool recursive = true);

    static bool compressDir(QString fileCompressed, QString dir,
                            bool recursive, QDir::Filters filters);

    static QString extractFile(QString fileCompressed, QString fileName, QString fileDest = QString());

    static QStringList extractFiles(QString fileCompressed, QStringList files, QString dir = QString());

    static QStringList extractDir(QString fileCompressed, QString dir = QString());

    static QStringList extractDir(QString fileCompressed, QTextCodec* fileNameCodec, QString dir = QString());

    static QStringList getFileList(QString fileCompressed);

    static QString extractFile(QIODevice *ioDevice, QString fileName, QString fileDest = QString());

    static QStringList extractFiles(QIODevice *ioDevice, QStringList files, QString dir = QString());

    static QStringList extractDir(QIODevice *ioDevice, QString dir = QString());

    static QStringList extractDir(QIODevice* ioDevice, QTextCodec* fileNameCodec, QString dir = QString());

    static QStringList getFileList(QIODevice *ioDevice); 
};

#endif  
