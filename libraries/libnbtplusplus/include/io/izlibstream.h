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
#ifndef IZLIBSTREAM_H_INCLUDED
#define IZLIBSTREAM_H_INCLUDED

#include "io/zlib_streambuf.h"
#include <istream>
#include <zlib.h>

namespace zlib
{

class NBT_EXPORT inflate_streambuf : public zlib_streambuf
{
public:

    explicit inflate_streambuf(std::istream& input, size_t bufsize = 32768, int window_bits = 32 + 15);
    ~inflate_streambuf() noexcept;

    std::istream& get_istr() const { return is; }

private:
    std::istream& is;
    bool stream_end;

    int_type underflow() override;
};

class NBT_EXPORT izlibstream : public std::istream
{
public:

    explicit izlibstream(std::istream& input, size_t bufsize = 32768):
        std::istream(&buf), buf(input, bufsize)
    {}

    std::istream& get_istr() const { return buf.get_istr(); }

private:
    inflate_streambuf buf;
};

}

#endif  
