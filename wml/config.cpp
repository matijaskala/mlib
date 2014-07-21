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

using namespace std;
using namespace wml;

void config::attribute::clear()
{
    value = nullvalue;
}

config::attribute& config::attribute::operator= ( const string& val )
{
    clear();
    value.set<string> ( val );
    char* endptr;
    double d = strtod ( val.c_str(), &endptr );
    if ( !*endptr ) {
        value.set<double> ( d );
        long l = d;
        if ( l == d )
            value.set<long> ( l );
    }
    return *this;
}

config::attribute& config::attribute::operator= ( double val )
{
    clear();
    value.set<string> ( to_string ( val ) );
    value.set<double> ( val );
    long l = val;
    if ( l == val )
        value.set<long> ( l );
    return *this;
}

config::attribute& config::attribute::operator= ( long int val )
{
    clear();
    value.set<string> ( to_string ( val ) );
    value.set<double> ( val );
    value.set<long> ( val );
    return *this;
}

void config::attribute::set_if_empty ( const string& val )
{
    if ( !value.valid<string> () )
        *this = val;
}

void config::attribute::set_if_empty ( double val )
{
    if ( !value.valid<double> () )
        *this = val;
}

void config::attribute::set_if_empty ( long val )
{
    if ( !value.valid<long> () )
        *this = val;
}

string config::attribute::str() const
{
    if ( value.valid<string> () )
        return value.get<string> ();
    else
        return "";
}

double config::attribute::to_double() const
{
    if ( value.valid<double> () )
        return value.get<double> ();
    else
        return numeric_limits< double >::quiet_NaN();
}

long config::attribute::to_long() const
{
    if ( value.valid<long> () )
        return value.get<long> ();
    else
        return -1;
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
    static const attribute empty;
    return empty;
}

void config::clear()
{
    attributes.clear();
    children.clear();
}
