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

#ifdef _WIN32
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

#ifdef __GNUC__
#define M_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define M_WARN_UNUSED_RESULT
#endif

namespace MLib
{
    M_EXPORT void init ( int& argc, char**& argv );
    M_EXPORT void quit ( int status = 0 );
}

#endif // MGLOBAL_H
