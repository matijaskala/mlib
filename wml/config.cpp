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

using namespace std;
using namespace wml;

config::attribute& config::attribute::operator= ( string val )
{
    char* endptr;
    double d = strtod ( val.c_str(), &endptr );
    if ( *endptr || val.empty() )
        value = make_tuple(-1, numeric_limits<double>::quiet_NaN(), move(val));
    else
        value = make_tuple(d, d, move(val));
    return *this;
}

config::attribute& config::attribute::operator= ( double d )
{
    value = make_tuple(d, d, to_string(d));
    return *this;
}

config::attribute& config::attribute::operator= ( long int l )
{
    value = make_tuple(l, l, to_string(l));
    return *this;
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
