/*
 * This file is part of MLib
 * Copyright (C) 2014-2025  Matija Skala <mskala@gmx.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MVARIANT_H
#define MVARIANT_H

#include <string>
#include <twoway-vector.hh>
#include <variant>

class MVariant
{
    using LongDouble = long double;
    using String = std::string;
    using StringList = twoway_vector<String>;
    using List = twoway_vector<MVariant>;
    std::variant<LongDouble, String, List> base;

public:
    MVariant ( std::nullptr_t = nullptr ) {}
    MVariant ( LongDouble x ) : base{x} {}
    MVariant ( String x ) : base{std::move(x)} {}
    MVariant ( List x ) : base{std::move(x)} {}
    MVariant ( const char* s ) : base(static_cast<String>(s)) {}
    MVariant ( std::initializer_list<MVariant> l ) : base(static_cast<List>(l)) {}
    MVariant ( const MVariant& other ) : base(other.base) {}
    MVariant ( MVariant&& other ) : base{std::move(other.base)} {}
    MVariant& operator= ( const MVariant& other ) {
        base = other.base;
        return *this;
    }
    MVariant& operator= ( MVariant&& other ) {
        base = std::move(other.base);
        return *this;
    }
    bool operator== ( const MVariant& other ) const {
        return std::visit([] (auto&& self, auto&& other) {
            using self_t = std::decay_t<decltype(self)>;
            using other_t = std::decay_t<decltype(other)>;
            if constexpr (std::is_same_v<self_t, other_t>)
                return self == other;
            else if constexpr (std::is_same_v<self_t, List> || std::is_same_v<other_t, List>)
                return false;
            else if constexpr (std::is_same_v<self_t, String>)
                return self == std::to_string(other);
            else if constexpr (std::is_same_v<other_t, String>)
                return std::to_string(self) == other;
            else
                static_assert(std::is_same_v<self_t, other_t>);
        }, base, other.base);
    }
    bool operator!= ( const MVariant& other ) const {
        return !(*this == other);
    }
    int toInt () const { return toLongDouble(); }
    long toLong () const { return toLongDouble(); }
    float toFloat () const { return toLongDouble(); }
    double toDouble () const { return toLongDouble(); }
    StringList toStringList () const {
        StringList l;
        for ( auto&& x: toList() )
            l.push_back(x.toString());
        return l;
    }
    LongDouble toLongDouble () const {
        return std::visit([] (auto&& self) {
            using self_t = std::decay_t<decltype(self)>;
            if constexpr (std::is_same_v<self_t, long double>)
                return self;
            else if constexpr (std::is_same_v<self_t, String>)
                return std::stold(self);
            else
                return 0.0l;
        }, base);
    }
    String toString () const {
        return std::visit([] (auto&& self) {
            using self_t = std::decay_t<decltype(self)>;
            if constexpr (std::is_same_v<self_t, String>)
                return self;
            else if constexpr (std::is_same_v<self_t, long double>)
                return std::to_string(self);
            else {
                String r;
                for (auto&& x: self) {
                    if (!r.empty())
                        r += ", ";
                    r += x.toString();
                }
                return r;
            }
        }, base);
    }
    List toList () const {
        return std::visit([] (auto&& self) {
            using self_t = std::decay_t<decltype(self)>;
            if constexpr (std::is_same_v<self_t, List>)
                return self;
            else
                return List{MVariant{self}};
        }, base);
    }
};

#endif // MVARIANT_H
