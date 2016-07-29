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

#ifndef MDEBUG_H
#define MDEBUG_H

#include <mglobal.h>
#include <ostream>

#undef ERROR
enum MDebugLevel {
    LOG,
    ERROR,
};

class M_EXPORT MDebug
{
public:
    MDebug ( MDebugLevel level = LOG );
    MDebug ( const MDebug& other ) = delete;
    MDebug ( MDebug&& other ) = default;
    ~MDebug();

    template< typename _String >
    const MDebug& operator<< ( const _String& __n ) const {
        stream << __n;
        return *this;
    }

private:
    MDebugLevel level;
    std::ostream& stream;
    std::string trace;
};

template< typename... _Args >
MDebug mDebug ( _Args&&... __args )
{
    return MDebug ( __args... );
}

#endif // MDEBUG_H



