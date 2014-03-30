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

_Directory_iterator& _Directory_iterator::operator++() {
    dirent* entry = static_cast<dirent*> ( _M_data );
    DIR* directory = reinterpret_cast<DIR*> ( _M_directory );
    readdir_r ( directory, entry, &entry );
    _M_data = entry;
    return *this;
}

_Directory_iterator& _Directory_iterator::operator++(int) {
    dirent* entry = static_cast<dirent*> ( _M_data );
    DIR* directory = reinterpret_cast<DIR*> ( _M_directory );
    readdir_r ( directory, entry, &entry );
    _M_data = entry;
    return *this;
}

const char* _Directory_iterator::operator*() {
    dirent* entry = static_cast<dirent*> ( _M_data );
    return entry->d_name;
}

directory::directory ( const std::string& __s )
    : file ( __s ) {
    DIR* directory = opendir ( __s.c_str() );
    if ( !directory )
        throw "DIRECTORY DOESN'T EXIST";
    _M_directory = reinterpret_cast<__directory*> ( directory );
    _M_begin._M_directory = reinterpret_cast<__directory*> ( directory );
    _M_begin._M_data = readdir ( directory );
    _M_end._M_directory = reinterpret_cast<__directory*> ( directory );
    _M_end._M_data = nullptr;
}
