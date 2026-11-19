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
#ifndef TAG_REF_PROXY_H_INCLUDED
#define TAG_REF_PROXY_H_INCLUDED

#include "tag.h"
#include <string>
#include <type_traits>

namespace nbt
{

class NBT_EXPORT value
{
public:

    value() noexcept {}
    explicit value(std::unique_ptr<tag>&& t) noexcept: tag_(std::move(t)) {}
    explicit value(tag&& t);

    value(value&&) noexcept = default;
    value& operator=(value&&) noexcept = default;

    explicit value(const value& rhs);
    value& operator=(const value& rhs);

    value& operator=(tag&& t);
    void set(tag&& t);

    operator tag&() { return get(); }
    operator const tag&() const { return get(); }
    tag& get() { return *tag_; }
    const tag& get() const { return *tag_; }

    template<class T>
    T& as();
    template<class T>
    const T& as() const;

    value& operator=(int8_t val);
    value& operator=(int16_t val);
    value& operator=(int32_t val);
    value& operator=(int64_t val);
    value& operator=(float val);
    value& operator=(double val);

    value& operator=(const std::string& str);
    value& operator=(std::string&& str);

    explicit operator int8_t() const;
    explicit operator int16_t() const;
    explicit operator int32_t() const;
    explicit operator int64_t() const;
    explicit operator float() const;
    explicit operator double() const;

    explicit operator const std::string&() const;

    explicit operator bool() const { return tag_ != nullptr; }

    value& at(const std::string& key);
    const value& at(const std::string& key) const;

    value& operator[](const std::string& key);
    value& operator[](const char* key);  

    value& at(size_t i);
    const value& at(size_t i) const;

    value& operator[](size_t i);
    const value& operator[](size_t i) const;

    std::unique_ptr<tag>& get_ptr() { return tag_; }
    const std::unique_ptr<tag>& get_ptr() const { return tag_; }

    void set_ptr(std::unique_ptr<tag>&& t) { tag_ = std::move(t); }

    tag_type get_type() const;

    friend NBT_EXPORT bool operator==(const value& lhs, const value& rhs);
    friend NBT_EXPORT bool operator!=(const value& lhs, const value& rhs);

private:
    std::unique_ptr<tag> tag_;
};

template<class T>
T& value::as()
{
    return tag_->as<T>();
}

template<class T>
const T& value::as() const
{
    return tag_->as<T>();
}

}

#endif  
