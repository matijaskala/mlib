/*
 * This file is part of MLib
 * Copyright (C) 2014  Matija Skala <mskala@gmx.com>
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

#ifndef WML_CONFIG_H
#define WML_CONFIG_H

#include <MGlobal>
#include <string>
#include <limits>
#include <list>
#include <map>

namespace wml
{

class M_EXPORT config
{
public:
    class M_EXPORT attribute {
        friend class config;
        using value_type = std::tuple<long, double, std::string>;
        value_type value = std::make_tuple(-1, std::numeric_limits<double>::quiet_NaN(), std::string{});
        std::string name = "";
        attribute ( const attribute& ) = delete;
    public:
        attribute() = default;
        void clear() { *this = ""; }
        attribute& operator= ( std::string val );
        attribute& operator= ( double val );
        attribute& operator= ( long val );
        template <typename T>
        void set_if_empty ( T&& val ) { if ( empty() ) *this = std::forward<T>(val); }
        bool empty() const { return str().empty(); }
        const std::string& str() const { return as<std::string>(); }
        double to_double() const { return as<double>(); }
        long to_long() const { return as<long>(); }
        template <typename T>
        const T& as() const { return std::get<T>(value); }
        operator const std::string&() const { return as<std::string>(); }
    };
    config ( std::string name = "" );
    ~config() { clear(); }
    config ( const config& other ) = delete;
    config ( config&& other ) = default;
    attribute& operator[] ( const std::string& key );
    const attribute& operator[] ( const std::string& key ) const;
    void clear();
    std::string name;
    std::list<config> children;
private:
    std::map<std::string, attribute> attributes;
};
}

#endif // WML_CONFIG_H
