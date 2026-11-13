#ifndef QUACHECKSUM32_H
#define QUACHECKSUM32_H

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
#include "quazip_global.h"

class QUAZIP_EXPORT QuaChecksum32
{

public:
    virtual ~QuaChecksum32();

	virtual quint32 calculate(const QByteArray &data) = 0;

	virtual void reset() = 0;

	virtual void update(const QByteArray &buf) = 0;

	virtual quint32 value() = 0;
};

#endif  
