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
#ifndef STREAM_READER_H_INCLUDED
#define STREAM_READER_H_INCLUDED

#include "endian_str.h"
#include "tag.h"
#include "tag_compound.h"
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <utility>

namespace nbt
{
namespace io
{

class NBT_EXPORT input_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

NBT_EXPORT std::pair<std::string, std::unique_ptr<tag_compound>> read_compound(std::istream& is, endian::endian e = endian::big);

NBT_EXPORT std::pair<std::string, std::unique_ptr<tag>> read_tag(std::istream& is, endian::endian e = endian::big);

class NBT_EXPORT stream_reader
{
public:

    explicit stream_reader(std::istream& is, endian::endian e = endian::big) noexcept;

    std::istream& get_istr() const;

    endian::endian get_endian() const;

    std::pair<std::string, std::unique_ptr<tag_compound>> read_compound();

    std::pair<std::string, std::unique_ptr<tag>> read_tag();

    std::unique_ptr<tag> read_payload(tag_type type);

    tag_type read_type(bool allow_end = false);

    template<class T>
    void read_num(T& x);

    std::string read_string();

private:
    std::istream& is;
    int depth = 0;
    const endian::endian endian;
};

template<class T>
void stream_reader::read_num(T& x)
{
    endian::read(is, x, endian);
}

}
}

#endif  
