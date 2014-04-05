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
#include <stdexcept>

using namespace std;
using namespace non_std;

file::file ( const string& __s )
    : name ( __s )
    , path ( )
{
    refresh();
}

file::file ( const pair< string, string >& __s )
    : name ( __s.first )
    , path ( __s.second )
{
    refresh();
}

void file::refresh()
{
    struct stat st;
    string fullpath = ( path == "" ) ? name : path + "/" + name;

    lstat ( fullpath.c_str(), &st );
    mode = st.st_mode;
    if ( ( mode & type ) == symlink ) {
        stat ( fullpath.c_str(), &st );
        if ( ( st.st_mode & type ) == symlink )
        mode &= ~type;
    }
}

directory::directory ( const string& __s )
    : name ( __s ) {
    DIR* dir = opendir ( __s.c_str() );
    if ( !dir )
        throw runtime_error ( "Unknown directory." );
    while ( dirent* entry = readdir ( dir ) ) {
        insert ( { entry->d_name, name } );
    }
    closedir ( dir );
}
