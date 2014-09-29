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

#include "module"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include "cxxabi"
#endif

using namespace non_std;

std::string non_std::module::prefix() {
#if defined _WIN32
    return "";
#else
    return "lib";
#endif
}

std::string non_std::module::suffix() {
#if defined _WIN32
    return ".dll";
#elif defined __APPLE__
    return ".dylib";
#else
    return ".so";
#endif
}

bool non_std::module::open ( std::string s )
{
    if ( is_open() )
        close();
#ifdef _WIN32
    if ( s.empty() )
        handle = GetModuleHandle ( NULL );
    else
    {
        std::string fileName;

        for ( char c: s )
        {
            if( !c )
                break;
            else if( c == '/' )
                fileName += '\\';
            else
                fileName += c;
        }

        fileName += suffix();

        handle = LoadLibraryExA ( (LPSTR) fileName.c_str(), NULL, 
                                 LOAD_WITH_ALTERED_SEARCH_PATH );
    }
#else
    handle = dlopen ( s.empty() ? nullptr : s.c_str(), RTLD_NOW | RTLD_GLOBAL );
#endif
    return is_open();
}

bool non_std::module::close()
{
    if ( !is_open() )
        return false;
    bool ret =
#ifdef _WIN32
    FreeLibrary ( handle );
#else
    dlclose ( handle ) == 0;
#endif
    handle = nullptr;
    return ret;
}

const char* non_std::module::last_error()
{
#ifdef _WIN32
    return GetLastError();
#else
    return dlerror();
#endif
}

void* non_std::module::symbol_private ( const char* s ) const
{
#ifdef _WIN32
    return GetProcAddress ( handle, s );
#else
    std::string str = s;
    bool mangle = false;
    for ( char c: str )
        if ( c != '_' && !isalnum(c) )
            mangle = true;
    if ( mangle )
        str = non_std::mangle_symbol(str);
    return dlsym ( handle, str.c_str() );
#endif
}
