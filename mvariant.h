/*
 * This file is part of MLib
 * Copyright (C) 2014-2023  Matija Skala <mskala@gmx.com>
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

#include <variant>
#include <sstream>
#include <twoway-vector.hh>

class MVariant : std::variant<long double, std::string, twoway_vector<MVariant>>
{
    using LongDouble = long double;
    using String = std::string;
    using StringList = twoway_vector<String>;
    using List = twoway_vector<MVariant>;
    using Base = std::variant<LongDouble, String, List>;

public:
    MVariant ( std::nullptr_t = nullptr ) {}
    MVariant ( LongDouble x ) : Base{x} {}
    MVariant ( String x ) : Base{std::move(x)} {}
    MVariant ( List x ) : Base{std::move(x)} {}
    MVariant ( const char* s ) : MVariant(static_cast<String>(s)) {}
    MVariant ( std::initializer_list<MVariant> l ) : MVariant(static_cast<List>(l)) {}
    MVariant ( const MVariant& other ) : Base{static_cast<const Base&>(other)} {}
    MVariant ( MVariant&& other ) : Base{static_cast<Base&&>(other)} {}
    MVariant& operator= ( const MVariant& other ) {
        Base::operator=(static_cast<const Base&>(other));
        return *this;
    }
    MVariant& operator= ( MVariant&& other ) {
        Base::operator=(static_cast<Base&&>(other));
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
            else {
                std::stringstream ss1, ss2;
                ss1 << self;
                ss2 << other;
                return ss1.str() == ss2.str();
            }
        }, static_cast<const Base&>(*this), static_cast<const Base&>(other));
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
            else if constexpr (std::is_same_v<self_t, String>) {
                std::stringstream ss;
                long double r;
                ss << self;
                ss >> r;
                return r;
            }
            else
                return 0.0l;
        }, static_cast<const Base&>(*this));
    }
    String toString () const {
        return std::visit([] (auto&& self) {
            using self_t = std::decay_t<decltype(self)>;
            if constexpr (std::is_same_v<self_t, String>)
                return self;
            else if constexpr (std::is_same_v<self_t, long double>) {
                std::stringstream ss;
                String r;
                ss << self;
                ss >> r;
                return r;
            }
            else {
                String r;
                for (auto&& x: self)
                    r += x.toString();
                return r;
            }
        }, static_cast<const Base&>(*this));
    }
    List toList () const {
        return std::visit([] (auto&& self) {
            using self_t = std::decay_t<decltype(self)>;
            if constexpr (std::is_same_v<self_t, List>)
                return self;
            else
                return List{};
        }, static_cast<const Base&>(*this));
    }
};

#endif // MVARIANT_H
