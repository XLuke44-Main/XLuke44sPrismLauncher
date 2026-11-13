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
#ifndef STREAM_WRITER_H_INCLUDED
#define STREAM_WRITER_H_INCLUDED

#include "tag.h"
#include "endian_str.h"
#include <iosfwd>
#include <string>

namespace nbt
{
namespace io
{

NBT_EXPORT void write_tag(const std::string& key, const tag& t, std::ostream& os, endian::endian e = endian::big);

class NBT_EXPORT stream_writer
{
public:

    static constexpr size_t max_string_len = UINT16_MAX;

    static constexpr uint32_t max_array_len = INT32_MAX;

    explicit stream_writer(std::ostream& os, endian::endian e = endian::big) noexcept:
        os(os), endian(e)
    {}

    std::ostream& get_ostr() const { return os; }

    endian::endian get_endian() const { return endian; }

    void write_tag(const std::string& key, const tag& t);

    void write_payload(const tag& t) { t.write_payload(*this); }

    void write_type(tag_type tt) { write_num(static_cast<int8_t>(tt)); }

    template<class T>
    void write_num(T x);

    void write_string(const std::string& str);

private:
    std::ostream& os;
    const endian::endian endian;
};

template<class T>
void stream_writer::write_num(T x)
{
    endian::write(os, x, endian);
}

}
}

#endif  
