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
#ifndef TAG_COMPOUND_H_INCLUDED
#define TAG_COMPOUND_H_INCLUDED

#include "crtp_tag.h"
#include "value_initializer.h"
#include <map>
#include <string>

namespace nbt
{

class NBT_EXPORT tag_compound final : public detail::crtp_tag<tag_compound>
{
    typedef std::map<std::string, value> map_t_;

public:

    typedef map_t_::iterator iterator;
    typedef map_t_::const_iterator const_iterator;

    static constexpr tag_type type = tag_type::Compound;

    tag_compound() {}

    tag_compound(std::initializer_list<std::pair<std::string, value_initializer>> init);

    value& at(const std::string& key);
    const value& at(const std::string& key) const;

    value& operator[](const std::string& key) { return tags[key]; }

    std::pair<iterator, bool> put(const std::string& key, value_initializer&& val);

    std::pair<iterator, bool> insert(const std::string& key, value_initializer&& val);

    template<class T, class... Args>
    std::pair<iterator, bool> emplace(const std::string& key, Args&&... args);

    bool erase(const std::string& key);

    bool has_key(const std::string& key) const;

    bool has_key(const std::string& key, tag_type type) const;

    size_t size() const { return tags.size(); }

    void clear() { tags.clear(); }

    iterator begin() { return tags.begin(); }
    iterator end()   { return tags.end(); }
    const_iterator begin() const  { return tags.begin(); }
    const_iterator end() const    { return tags.end(); }
    const_iterator cbegin() const { return tags.cbegin(); }
    const_iterator cend() const   { return tags.cend(); }

    void read_payload(io::stream_reader& reader) override;
    void write_payload(io::stream_writer& writer) const override;

    friend bool operator==(const tag_compound& lhs, const tag_compound& rhs)
    { return lhs.tags == rhs.tags; }
    friend bool operator!=(const tag_compound& lhs, const tag_compound& rhs)
    { return !(lhs == rhs); }

private:
    map_t_ tags;
};

template<class T, class... Args>
std::pair<tag_compound::iterator, bool> tag_compound::emplace(const std::string& key, Args&&... args)
{
    return put(key, value(make_unique<T>(std::forward<Args>(args)...)));
}

}

#endif  
