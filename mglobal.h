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

#ifndef MGLOBAL_H
#define MGLOBAL_H

#include <string>
#include <typeinfo>

#ifdef WIN32
#define M_NO_EXPORT
#define M_EXPORT __declspec(dllexport)
#define M_IMPORT __declspec(dllimport)
#define M_DEPRECATED __declspec(deprecated)
#else
#define M_NO_EXPORT __attribute__ ((visibility("hidden")))
#define M_EXPORT __attribute__ ((visibility("default")))
#define M_IMPORT __attribute__ ((visibility("default")))
#define M_DEPRECATED __attribute__ ((__deprecated__))
#endif

namespace M
{
    std::string demangle ( const char* symbol );
    template< typename _Class >
    std::string typeName() {
        return demangle ( typeid ( _Class ).name() );
    }
}

#endif // MGLOBAL_H
