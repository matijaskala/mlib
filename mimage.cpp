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

#include <MResourceLoader>
#include <MTexture>

#include <map>
#include <GL/gl.h>

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

MTexture* MImage::createTexture() const
{
    auto texture = new MTexture;
    texture->bind();
    texture->setSize(size());
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    GLenum format = hasAlpha() ? GL_RGBA : GL_RGB;
    glTexImage2D ( GL_TEXTURE_2D, 0, format, size().width(), size().height(), 0, format, GL_UNSIGNED_BYTE, data() );
    return texture;
}

bool MImage::load ( std::string file )
{
    for ( auto loader: MResourceLoader::loaders() ) {
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