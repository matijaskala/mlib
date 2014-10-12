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

#include "mimage.h"

#include <MDataLoader>

#include <map>

static std::map< std::string, MImage* > map;

MImage::MImage ( MSize size, bool alpha, uint8_t* data )
    : m_size{size}
    , m_alpha{alpha}
    , m_data{data}
{
}

MImage::~MImage()
{
    delete[] m_data;
}

bool MImage::load ( std::string file )
{
    for ( auto loader: MDataLoader::loaders() ) {
        if ( loader->type() != Image )
            continue;
        if ( !loader->valid ( file ) )
            continue;
        auto res = loader->load ( file );
        if ( !res )
            continue;
        auto image = dynamic_cast< MImage* > ( res );
        if ( !image ) {
            delete res;
            continue;
        }
        map[file] = image;
        return true;
    }
    return false;
}

void MImage::unload ( std::string file )
{
    auto i = map.find ( file );
    delete i->second;
    map.erase ( i );
}

void MImage::unload ( const MImage* image )
{
    for ( auto position: map )
        if ( position.second == image )
            map.erase ( position.first );
    delete image;
}

MImage* MImage::get ( std::string file )
{
    return map[file];
}
