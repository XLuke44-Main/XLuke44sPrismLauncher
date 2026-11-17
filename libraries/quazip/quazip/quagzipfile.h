#ifndef QUAZIP_QUAGZIPFILE_H
#define QUAZIP_QUAGZIPFILE_H

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

#include <QtCore/QIODevice>
#include "quazip_global.h"

#include <zlib.h>

class QuaGzipFilePrivate;

class QUAZIP_EXPORT QuaGzipFile: public QIODevice {
  Q_OBJECT
public:

  QuaGzipFile();

  QuaGzipFile(QObject *parent);

  QuaGzipFile(const QString &fileName, QObject *parent = nullptr);

  ~QuaGzipFile() override;

  void setFileName(const QString& fileName);

  QString getFileName() const;

  bool isSequential() const override;

  bool open(QIODevice::OpenMode mode) override;

  virtual bool open(int fd, QIODevice::OpenMode mode);

  virtual bool flush();

  void close() override;
protected:

  qint64 readData(char *data, qint64 maxSize) override;

  qint64 writeData(const char *data, qint64 maxSize) override;
private:

    QuaGzipFile(const QuaGzipFile &that);
    QuaGzipFile& operator=(const QuaGzipFile &that);
    QuaGzipFilePrivate *d;
};

#endif  
