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

#ifndef MDL_H
#define MDL_H

#include <string>

class MDL
{
    void* const m_ptr;

    MDL ( void* ptr ) : m_ptr(ptr) {  }

public:
    static bool open ( std::string file );

    static bool close ( std::string file );

    static MDL get ( std::string file );

    template< typename T >
    T* symbol ( std::string name ) {
        auto sym = symbol<void> ( name );
        return reinterpret_cast<T*&> ( sym );
    }

    static constexpr const char* libprefix =
#if defined _WIN32
        "";
#else
        "lib";
#endif

    static constexpr const char* libsuffix =
#if defined _WIN32
        ".dll";
#elif defined __APPLE__
        ".dylib";
#else
        ".so";
#endif
};

template<>
void* MDL::symbol ( std::string name );

#endif // MDL_H
