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

#include <iostream>

using namespace std;

std::string xterm_escape ( std::string s ) {
    static char* TERM = getenv ( "TERM" );
    return TERM ? "["s + s : "";
}

MDebug::MDebug ( MDebugLevel level )
    : level ( level )
    , stream ( std::cerr )
{
    if ( level == ERROR ) {
        stream << xterm_escape("1m"s) << xterm_escape("33m"s) << xterm_escape("41m"s);
        stream << "ERROR: "s;
        stream << xterm_escape("39m"s);
    }
}

MDebug::MDebug ( const char* file, int line, MDebugLevel level )
    : level{level}
    , stream{std::cerr}
{
    stream << xterm_escape("35m"s) << file << xterm_escape("36m"s) << ':';
    stream << xterm_escape("32m"s) << line << xterm_escape("36m"s) << ':' << xterm_escape("m"s);
    if ( level == ERROR ) {
        stream << xterm_escape("1m"s) << xterm_escape("33m"s) << xterm_escape("41m"s);
        stream << "ERROR: "s;
        stream << xterm_escape("39m"s);
    }
}

MDebug::~MDebug()
{
    stream << trace;
    stream << xterm_escape("m"s);
    stream << std::endl;
    stream.flush();
}

