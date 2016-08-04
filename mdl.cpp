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

#include "mdl.h"

#include <MDebug>

#include <unordered_map>

using namespace std;

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

static unordered_map<string,void*> modules;

bool MDL::open ( string file )
{
    if ( file.empty() )
        return true;
    void*& module = modules[file];
    if ( module ) {
        mDebug() << "Module '" << file << "' already loaded";
        return true;
    }
#ifdef _WIN32
    string mspath;
    for ( char c: file )
        mspath += ( c == '/' ) ? '\\' : c;
    module = LoadLibraryExA ( (LPSTR) mspath.c_str(), NULL, 
                             LOAD_WITH_ALTERED_SEARCH_PATH );

    if ( !module ) {
        static TCHAR error_buffer[0x100];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                      MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                      error_buffer, sizeof(error_buffer), NULL);

        mDebug(ERROR) << error_buffer;
    }
#else
    dlerror();
    module = dlopen ( file.c_str(), RTLD_NOW | RTLD_LOCAL );
    if ( module == nullptr ) {
        char* error = dlerror();
        if ( error )
            mDebug(ERROR) << error;
    }
#endif
    return module;
}

bool MDL::close ( string file )
{
    if ( file.empty() )
        return true;
    void*& module = modules[file];
    if ( module == nullptr ) {
        mDebug() << "Module '" << file << "' was not loaded";
        return true;
    }
#ifdef _WIN32
    if ( !FreeLibrary ( static_cast<HMODULE> ( module ) ) ) {
        static TCHAR error_buffer[0x100];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                      MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                      error_buffer, sizeof(error_buffer), NULL);

        mDebug(ERROR) << error_buffer;
    }
#else
    dlerror();
    if ( dlclose(module) != 0 )
        mDebug() << dlerror();
#endif
    else {
        modules.erase(file);
        return true;
    }
    return false;
}

MDL MDL::get ( string file )
{
    if ( file.empty() )
        return MDL{nullptr};
    void* module = modules[file];
    if ( module == nullptr )
        mDebug() << "Module '" << file << "' not loaded";
    return MDL{module};
}

template<>
void* MDL::symbol ( string name )
{
#ifdef _WIN32
    FARPROC sym = GetProcAddress( static_cast<HMODULE> ( m_ptr ), name.c_str());
    return reinterpret_cast<void*> ( sym );
#else
    return dlsym(m_ptr, name.c_str());
#endif
}
