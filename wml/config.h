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

#include <string>
#include <map>
#include <list>

namespace wml
{

class config
{
public:
    class attribute {
        friend class config;
        using value_type = std::tuple<long, double, std::string>;
        value_type value;
        std::string name;
    public:
        void clear();
        attribute& operator= ( const std::string& val );
        attribute& operator= ( double val );
        attribute& operator= ( long val );
        void set_if_empty ( const std::string& val );
        void set_if_empty ( double val );
        void set_if_empty ( long val );
        bool empty() const;
        std::string str() const;
        double to_double() const;
        long to_long() const;
        template <typename T>
        T as() const { return std::get<T>(value); }
        operator std::string() const { return as<std::string>(); }
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
