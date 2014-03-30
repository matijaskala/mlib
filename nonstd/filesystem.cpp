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

#include "filesystem"

#include <dirent.h>
#include <sys/stat.h>
#include <iostream>

using namespace non_std;

file::file ( const std::string& __s )
    : name ( __s )
{
    struct stat st;
    stat ( __s.c_str(), &st );

    mode = st.st_mode;
}

directory::directory ( const std::string& __s )
    : name ( __s ) {
    DIR* dir = opendir ( __s.c_str() );
    if ( !dir )
        throw "DIRECTORY DOESN'T EXIST";
    while ( dirent* entry = readdir ( dir ) ) {
        insert(entry->d_name);
    }
    closedir ( dir );
}
