/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "mfilesystem.h"

#include <sys/stat.h>
#include <dirent.h>

MFile::MFile ( std::string fullname )
{
    while ( fullname.back() == '/' )
        fullname.pop_back();

    auto l = fullname.rfind ( '/' );
    m_name = fullname.substr ( 0, l );
    m_path = fullname.substr ( l + 1 );

    refresh();
}

MFile::MFile ( std::string name, std::string path )
{
    while ( path.back() == '/' )
        path.pop_back();

    m_name = name;
    m_path = path;

    refresh();
}

void MFile::refresh()
{
    struct stat64 st;
    lstat64 ( fullname().c_str(), &st );

    m_mode = st.st_mode & 07777;
    m_type = static_cast<Type> ( st.st_mode >> 13 );
}

MDirectory::MDirectory ( std::string path )
{
    m_path = path;

    auto dir = opendir ( path.c_str() );
    if ( !dir )
        return;

    dirent64 entry, *ptr;
    while ( !readdir64_r ( dir, &entry, &ptr ) ) {
        if ( ptr )
            m_entries.insert ( {entry.d_name,path} );
        else
            break;
    }

    closedir ( dir );
}
