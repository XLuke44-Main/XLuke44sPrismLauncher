#ifndef QUA_ZIP_H
#define QUA_ZIP_H

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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include "quazip_qt_compat.h"

#include "zip.h"
#include "unzip.h"

#include "quazip_global.h"
#include "quazipfileinfo.h"

#ifndef UNZ_OPENERROR

#define UNZ_OPENERROR -1000
#endif

class QuaZipPrivate;

class QUAZIP_EXPORT QuaZip {
  friend class QuaZipPrivate;
  public:

    enum Constants {
      MAX_FILE_NAME_LENGTH=256  

    };

    enum Mode {
      mdNotOpen,  
      mdUnzip,  
      mdCreate,  
      mdAppend,  

      mdAdd  
    };

    enum CaseSensitivity {
      csDefault=0,  
      csSensitive=1,  
      csInsensitive=2  
    };

    static Qt::CaseSensitivity convertCaseSensitivity(
            CaseSensitivity cs);
  private:
    QuaZipPrivate *p;
  public:

    QuaZip();

    QuaZip(const QString& zipName);

    QuaZip(QIODevice *ioDevice);

    QuaZip(const QuaZip& that) = delete;

    QuaZip& operator=(const QuaZip& that) = delete;

    ~QuaZip();

    bool open(Mode mode, zlib_filefunc_def *ioApi =nullptr);

    void close();

    void setFileNameCodec(QTextCodec *fileNameCodec);

    void setFileNameCodec(const char *fileNameCodecName);

    void setOsCode(uint osCode);

    uint getOsCode() const;

    QTextCodec* getFileNameCodec() const;

    void setCommentCodec(QTextCodec *commentCodec);

    void setCommentCodec(const char *commentCodecName);

    QTextCodec* getCommentCodec() const;

    QString getZipName() const;

    void setZipName(const QString& zipName);

    QIODevice *getIoDevice() const;

    void setIoDevice(QIODevice *ioDevice);

    Mode getMode() const;

    bool isOpen() const;

    int getZipError() const;

    int getEntriesCount() const;

    QString getComment() const;

    void setComment(const QString& comment);

    bool goToFirstFile();

    bool goToNextFile();

    bool setCurrentFile(const QString& fileName, CaseSensitivity cs =csDefault);

    bool hasCurrentFile() const;

    bool getCurrentFileInfo(QuaZipFileInfo* info)const;

    bool getCurrentFileInfo(QuaZipFileInfo64* info)const;

    QString getCurrentFileName()const;

    unzFile getUnzFile();

    zipFile getZipFile();

    void setDataDescriptorWritingEnabled(bool enabled);

    bool isDataDescriptorWritingEnabled() const;

    QStringList getFileNameList() const;

    QList<QuaZipFileInfo> getFileInfoList() const;

    QList<QuaZipFileInfo64> getFileInfoList64() const;

    void setZip64Enabled(bool zip64);

    bool isZip64Enabled() const;

    void setUtf8Enabled(bool utf8);

    bool isUtf8Enabled() const;

    bool isAutoClose() const;

    void setAutoClose(bool autoClose) const;

    static void setDefaultFileNameCodec(QTextCodec *codec);

    static void setDefaultFileNameCodec(const char *codecName);

    static void setDefaultOsCode(uint osCode);

    static uint getDefaultOsCode();
};

#endif
