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

#include "mdebug.h"

#include <nonstd/xterm>
#include <iostream>

using namespace non_std::xterm;

MDebug::MDebug ( MDebugLevel level )
    : level ( level )
    , stream ( std::cerr )
{
    if ( level == ERROR ) {
        stream << bold << fgcolor ( non_std::yellow ) << bgcolor ( non_std::red );
        stream << "ERROR: ";
        stream << fgcolor ( non_std::default_color );
    }
}

MDebug::MDebug ( const char* file, int line, MDebugLevel level )
    : level{level}
    , stream{std::cerr}
{
    stream << fgcolor ( non_std::magenta ) << file << fgcolor ( non_std::cyan ) << ':';
    stream << fgcolor ( non_std::green ) << line << fgcolor ( non_std::cyan ) << ':' << reset;
    if ( level == ERROR ) {
        stream << bold << fgcolor ( non_std::yellow ) << bgcolor ( non_std::red );
        stream << "ERROR: ";
        stream << fgcolor ( non_std::default_color );
    }
}

MDebug::~MDebug()
{
    stream << trace;
    stream << reset;
    stream << std::endl;
}

