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
#ifndef TAG_H_INCLUDED
#define TAG_H_INCLUDED

#include <cstdint>
#include <iosfwd>
#include <memory>
#include "nbt_export.h"

namespace nbt
{

enum class tag_type : int8_t
{
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    Byte_Array = 7,
    String = 8,
    List = 9,
    Compound = 10,
    Int_Array = 11,
    Long_Array = 12,
    Null = -1    
};

NBT_EXPORT bool is_valid_type(int type, bool allow_end = false);

class nbt_visitor;
class const_nbt_visitor;
namespace io
{
    class stream_reader;
    class stream_writer;
}

class NBT_EXPORT tag
{
public:

    virtual ~tag() noexcept {}

    virtual tag_type get_type() const noexcept = 0;

    virtual std::unique_ptr<tag> clone() const& = 0;
    virtual std::unique_ptr<tag> move_clone() && = 0;
    std::unique_ptr<tag> clone() &&;

    template<class T>
    T& as();
    template<class T>
    const T& as() const;

    virtual tag& assign(tag&& rhs) = 0;

    virtual void accept(nbt_visitor& visitor) = 0;
    virtual void accept(const_nbt_visitor& visitor) const = 0;

    virtual void read_payload(io::stream_reader& reader) = 0;

    virtual void write_payload(io::stream_writer& writer) const = 0;

    static std::unique_ptr<tag> create(tag_type type);

    friend NBT_EXPORT bool operator==(const tag& lhs, const tag& rhs);
    friend NBT_EXPORT bool operator!=(const tag& lhs, const tag& rhs);

private:

    virtual bool equals(const tag& rhs) const = 0;
};

NBT_EXPORT std::ostream& operator<<(std::ostream& os, tag_type tt);

NBT_EXPORT std::ostream& operator<<(std::ostream& os, const tag& t);

template<class T>
T& tag::as()
{
    static_assert(std::is_base_of<tag, T>::value, "T must be a subclass of tag");
    return dynamic_cast<T&>(*this);
}

template<class T>
const T& tag::as() const
{
    static_assert(std::is_base_of<tag, T>::value, "T must be a subclass of tag");
    return dynamic_cast<const T&>(*this);
}

}

#endif  
