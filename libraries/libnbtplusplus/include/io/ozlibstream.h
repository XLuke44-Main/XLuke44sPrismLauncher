/*
 * libnbt++ - A library for the Minecraft Named Binary Tag format.
 * Copyright (C) 2013, 2015  ljfa-ag
 *
 * This file is part of libnbt++.
 *
 * libnbt++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnbt++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libnbt++.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OZLIBSTREAM_H_INCLUDED
#define OZLIBSTREAM_H_INCLUDED

#include "io/zlib_streambuf.h"
#include <ostream>
#include <zlib.h>

namespace zlib
{

class NBT_EXPORT deflate_streambuf : public zlib_streambuf
{
public:

    explicit deflate_streambuf(std::ostream& output, size_t bufsize = 32768, int level = Z_DEFAULT_COMPRESSION, int window_bits = 15, int mem_level = 8, int strategy = Z_DEFAULT_STRATEGY);
    ~deflate_streambuf() noexcept;

    std::ostream& get_ostr() const { return os; }

    void close();
private:
    std::ostream& os;

    void deflate_chunk(int flush = Z_NO_FLUSH);

    int_type overflow(int_type ch) override;
    int sync() override;
};

class NBT_EXPORT ozlibstream : public std::ostream
{
public:

    explicit ozlibstream(std::ostream& output, int level = Z_DEFAULT_COMPRESSION, bool gzip = false, size_t bufsize = 32768):
        std::ostream(&buf), buf(output, bufsize, level, 15 + (gzip ? 16 : 0))
    {}

    std::ostream& get_ostr() const { return buf.get_ostr(); }

    void close();
private:
    deflate_streambuf buf;
};

}

#endif  
