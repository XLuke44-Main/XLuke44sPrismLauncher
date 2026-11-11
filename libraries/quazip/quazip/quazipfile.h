#ifndef QUA_ZIPFILE_H
#define QUA_ZIPFILE_H

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

#include <QtCore/QIODevice>

#include "quazip_global.h"
#include "quazip.h"
#include "quazipnewinfo.h"

class QuaZipFilePrivate;

class QUAZIP_EXPORT QuaZipFile: public QIODevice {
  friend class QuaZipFilePrivate;
  Q_OBJECT
  private:
    QuaZipFilePrivate *p;

    QuaZipFile(const QuaZipFile& that);
    QuaZipFile& operator=(const QuaZipFile& that);
  protected:

    qint64 readData(char *data, qint64 maxSize) override;

    qint64 writeData(const char *data, qint64 maxSize) override;
  public:

    QuaZipFile();

    QuaZipFile(QObject *parent);

    QuaZipFile(const QString& zipName, QObject *parent =nullptr);

    QuaZipFile(const QString& zipName, const QString& fileName,
        QuaZip::CaseSensitivity cs =QuaZip::csDefault, QObject *parent =nullptr);

    QuaZipFile(QuaZip *zip, QObject *parent =nullptr);

    ~QuaZipFile() override;

    QString getZipName()const;

    QuaZip* getZip()const;

    QString getFileName() const;

    QuaZip::CaseSensitivity getCaseSensitivity() const;

    QString getActualFileName()const;

    void setZipName(const QString& zipName);

    bool isRaw() const;

    void setZip(QuaZip *zip);

    void setFileName(const QString& fileName, QuaZip::CaseSensitivity cs =QuaZip::csDefault);

    bool open(OpenMode mode) override;

    inline bool open(OpenMode mode, const char *password)
    {return open(mode, nullptr, nullptr, false, password);}

    bool open(OpenMode mode, int *method, int *level, bool raw, const char *password =nullptr);

    bool open(OpenMode mode, const QuaZipNewInfo& info,
        const char *password =nullptr, quint32 crc =0,
        int method =Z_DEFLATED, int level =Z_DEFAULT_COMPRESSION, bool raw =false,
        int windowBits =-MAX_WBITS, int memLevel =DEF_MEM_LEVEL, int strategy =Z_DEFAULT_STRATEGY);

    bool isSequential()const override;

    qint64 pos()const override;

    bool atEnd()const override;

    qint64 size()const override;

    qint64 csize()const;

    qint64 usize()const;

    bool getFileInfo(QuaZipFileInfo *info);

    bool getFileInfo(QuaZipFileInfo64 *info);

    void close() override;

    int getZipError() const;

    qint64 bytesAvailable() const override;

    QByteArray getLocalExtraField();

    QDateTime getExtModTime();

    QDateTime getExtAcTime();

    QDateTime getExtCrTime();
};

#endif
