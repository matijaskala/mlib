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

#include "mresource.h"

#include <MResourceLoader>
#include <MTexture>

#include <map>

static std::map< std::string, MResource* > map;

bool MResource::load ( std::string file )
{
    for ( auto loader: MResourceLoader::loaders() ) {
        if ( !loader->valid ( file ) )
            continue;
        auto res = loader->load ( file );
        if ( !res )
            continue;
        map[file] = res;
        return true;
    }
    return false;
}

void MResource::unload ( std::string file )
{
    auto i = map.find ( file );
    delete i->second;
    map.erase ( i );
}

void MResource::unload ( const MResource* res )
{
    for ( auto i = map.begin(); i != map.end(); i++ )
        if ( i->second == res )
            map.erase ( i );
    delete res;
}

template<>
MResource* MResource::get ( std::string file )
{
    return map[file];
}
