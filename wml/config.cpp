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

#include "config.h"

#include <limits>
#include <cstdlib>

using namespace std;
using namespace wml;

void config::attribute::clear()
{
    value = make_tuple(-1,numeric_limits< double >::quiet_NaN(),"");
}

config::attribute& config::attribute::operator= ( const string& val )
{
    if ( val.empty() ) {
        clear();
        return *this;
    }
    char* endptr;
    double d = strtod ( val.c_str(), &endptr );
    if ( *endptr )
        value = make_tuple(-1,numeric_limits< double >::quiet_NaN(),val);
    else
        value = make_tuple(d,d,val);
    return *this;
}

config::attribute& config::attribute::operator= ( double d )
{
    value = make_tuple(d,d,to_string(d));
    return *this;
}

config::attribute& config::attribute::operator= ( long int l )
{
    value = make_tuple(l,l,to_string(l));
    return *this;
}

void config::attribute::set_if_empty ( const string& val )
{
    if ( empty() )
        *this = val;
}

void config::attribute::set_if_empty ( double val )
{
    if ( empty() )
        *this = val;
}

void config::attribute::set_if_empty ( long val )
{
    if ( empty() )
        *this = val;
}

bool config::attribute::empty() const
{
    return str().empty();
}

string config::attribute::str() const
{
    return get<string>(value);
}

double config::attribute::to_double() const
{
    return get<double>(value);
}

long config::attribute::to_long() const
{
    return get<long>(value);
}


config::config ( string name )
    : name ( name )
{

}

config::attribute& config::operator[] ( const string& key )
{
    auto it = attributes.find ( key );
    if ( it == attributes.end() ) {
        attributes[key].name = name + '[' + key + ']';
        it = attributes.find ( key );
    }
    return it->second;
}

const config::attribute& config::operator[] ( const string& key ) const
{
    auto it = attributes.find ( key );
    if ( it != attributes.end() )
        return it->second;
    static const attribute empty{};
    return empty;
}

void config::clear()
{
    attributes.clear();
    children.clear();
}
