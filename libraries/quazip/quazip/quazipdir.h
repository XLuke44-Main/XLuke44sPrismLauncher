#ifndef QUAZIP_QUAZIPDIR_H
#define QUAZIP_QUAZIPDIR_H

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

class QuaZipDirPrivate;

#include "quazip.h"
#include "quazipfileinfo.h"
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>

class QUAZIP_EXPORT QuaZipDir {
private:
    QSharedDataPointer<QuaZipDirPrivate> d;
public:

    QuaZipDir(const QuaZipDir &that);

    QuaZipDir(QuaZip *zip, const QString &dir = QString());

    ~QuaZipDir();

    bool operator==(const QuaZipDir &that);

    inline bool operator!=(const QuaZipDir &that) {return !operator==(that);}

    QuaZipDir& operator=(const QuaZipDir &that);

    QString operator[](int pos) const;

    QuaZip::CaseSensitivity caseSensitivity() const;

    bool cd(const QString &dirName);

    bool cdUp();

    uint count() const;

    QString dirName() const;

    QList<QuaZipFileInfo> entryInfoList(const QStringList &nameFilters,
        QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;

    QList<QuaZipFileInfo> entryInfoList(QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;

    QList<QuaZipFileInfo64> entryInfoList64(const QStringList &nameFilters,
        QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;

    QList<QuaZipFileInfo64> entryInfoList64(QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;

    QStringList entryList(const QStringList &nameFilters,
        QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;

    QStringList entryList(QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;

    bool exists(const QString &fileName) const;

    bool exists() const;

    QString filePath(const QString &fileName) const;

    QDir::Filters filter();

    bool isRoot() const;

    QStringList nameFilters() const;

    QString path() const;

    QString relativeFilePath(const QString &fileName) const;

    void setCaseSensitivity(QuaZip::CaseSensitivity caseSensitivity);

    void setFilter(QDir::Filters filters);

    void setNameFilters(const QStringList &nameFilters);

    void setPath(const QString &path);

    void setSorting(QDir::SortFlags sort);

    QDir::SortFlags sorting() const;
};

#endif  
