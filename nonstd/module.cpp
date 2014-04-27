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

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace non_std;

std::string non_std::module::suffix() {
#if defined WIN32
    return ".dll";
#elif defined __APPLE__
    return ".dylib";
#else
    return ".so";
#endif
}

bool non_std::module::open ( const char* __s )
{
    if ( is_open() )
        close();
#ifdef WIN32
    std::string str = __s;
    if ( str.empty() )
        _M_handle = GetModuleHandle ( NULL );
    else
    {
        std::string fileName;

        for ( char c: str )
        {
            if( !c )
                break;
            else if( c == '/' )
                fileName += '\\';
            else
                fileName += c;
        }

        fileName += suffix();

        _M_handle = LoadLibraryExA ( (LPSTR) fileName.c_str(), NULL, 
                                 LOAD_WITH_ALTERED_SEARCH_PATH );
    }
#else
    _M_handle = dlopen ( __s, RTLD_NOW | RTLD_GLOBAL );
#endif
    return _M_handle;
}

bool non_std::module::close()
{
    if ( !is_open() )
        return false;
    __handle_type handle = _M_handle;
    _M_handle = nullptr;
#ifdef WIN32
    return FreeLibrary ( handle );
#else
    return dlclose ( handle ) == 0;
#endif
}

const char* non_std::module::last_error()
{
#ifdef WIN32
    return GetLastError();
#else
    return dlerror();
#endif
}

void* non_std::module::_M_symbol ( const char* __s ) const
{
#ifdef WIN32
    return GetProcAddress ( _M_handle, __s );
#else
    return dlsym ( _M_handle, __s );
#endif
}

abstract_function non_std::module::operator->* ( const char* __s )
{
    void* __sym = _M_symbol ( __s );
    return reinterpret_cast< int ( *& ) () > ( __sym );
}

non_std::abstract_function::abstract_function ( const char* __s )
{
    _M_set ( dlsym ( nullptr, __s ) );
}
