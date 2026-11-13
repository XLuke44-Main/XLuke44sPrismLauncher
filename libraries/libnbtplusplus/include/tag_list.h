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
#ifndef TAG_LIST_H_INCLUDED
#define TAG_LIST_H_INCLUDED

#include "crtp_tag.h"
#include "tagfwd.h"
#include "value_initializer.h"
#include <stdexcept>
#include <vector>

namespace nbt
{

class NBT_EXPORT tag_list final : public detail::crtp_tag<tag_list>
{
public:

    typedef std::vector<value>::iterator iterator;
    typedef std::vector<value>::const_iterator const_iterator;

    static constexpr tag_type type = tag_type::List;

    template<class T>
    static tag_list of(std::initializer_list<T> init);

    tag_list(): tag_list(tag_type::Null) {}

    explicit tag_list(tag_type type): el_type_(type) {}

    tag_list(std::initializer_list<int8_t> init);
    tag_list(std::initializer_list<int16_t> init);
    tag_list(std::initializer_list<int32_t> init);
    tag_list(std::initializer_list<int64_t> init);
    tag_list(std::initializer_list<float> init);
    tag_list(std::initializer_list<double> init);
    tag_list(std::initializer_list<std::string> init);
    tag_list(std::initializer_list<tag_byte_array> init);
    tag_list(std::initializer_list<tag_list> init);
    tag_list(std::initializer_list<tag_compound> init);
    tag_list(std::initializer_list<tag_int_array> init);
    tag_list(std::initializer_list<tag_long_array> init);

    tag_list(std::initializer_list<value> init);

    value& at(size_t i);
    const value& at(size_t i) const;

    value& operator[](size_t i) { return tags[i]; }
    const value& operator[](size_t i) const { return tags[i]; }

    void set(size_t i, value&& val);

    void push_back(value_initializer&& val);

    template<class T, class... Args>
    void emplace_back(Args&&... args);

    void pop_back() { tags.pop_back(); }

    tag_type el_type() const { return el_type_; }

    size_t size() const { return tags.size(); }

    void clear() { tags.clear(); }

    void reset(tag_type type = tag_type::Null);

    iterator begin() { return tags.begin(); }
    iterator end()   { return tags.end(); }
    const_iterator begin() const  { return tags.begin(); }
    const_iterator end() const    { return tags.end(); }
    const_iterator cbegin() const { return tags.cbegin(); }
    const_iterator cend() const   { return tags.cend(); }

    void read_payload(io::stream_reader& reader) override;

    void write_payload(io::stream_writer& writer) const override;

    friend NBT_EXPORT bool operator==(const tag_list& lhs, const tag_list& rhs);
    friend NBT_EXPORT bool operator!=(const tag_list& lhs, const tag_list& rhs);

private:
    std::vector<value> tags;
    tag_type el_type_;

    template<class T, class Arg>
    void init(std::initializer_list<Arg> il);
};

template<class T>
tag_list tag_list::of(std::initializer_list<T> il)
{
    tag_list result;
    result.init<T, T>(il);
    return result;
}

template<class T, class... Args>
void tag_list::emplace_back(Args&&... args)
{
    if(el_type_ == tag_type::Null)  
        el_type_ = T::type;
    else if(el_type_ != T::type)
        throw std::invalid_argument("The tag type does not match the list's content type");
    tags.emplace_back(make_unique<T>(std::forward<Args>(args)...));
}

template<class T, class Arg>
void tag_list::init(std::initializer_list<Arg> init)
{
    el_type_ = T::type;
    tags.reserve(init.size());
    for(const Arg& arg: init)
        tags.emplace_back(nbt::make_unique<T>(arg));
}

}

#endif  
