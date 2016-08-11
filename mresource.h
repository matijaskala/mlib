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

#ifndef MDATAFILE_H
#define MDATAFILE_H

#include <mglobal.h>
#include <string>

struct M_EXPORT MResource
{
    enum Type {
        Audio,
        Font,
        Image,
    };

    MResource() = default;
    MResource(const MResource&) = delete;
    virtual ~MResource() = 0;

    static bool load ( std::string file );
    static void unload ( std::string file );
    static void unload ( const MResource* res );
    template< typename Resource = MResource >
    static Resource* get ( std::string file ) {
        return dynamic_cast<Resource*> ( get ( file ) );
    }
};

inline MResource::~MResource() = default;

template<>
M_EXPORT MResource* MResource::get ( std::string file );

#endif // MDATAFILE_H
