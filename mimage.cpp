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

#include <mresourceloader.h>
#include <mtexture.h>

#include <map>
#include <GL/gl.h>
#include <GL/glext.h>
#include <cstring>

MImage::MImage ( MSize size, bool alpha, void* data )
    : m_size{size}
    , m_alpha{alpha}
    , m_data{data}
{
}

MImage* mCopy ( const MImage* image )
{
    if ( !image->data() )
        return new MImage{image->size(), image->hasAlpha(), nullptr};
    auto data = std::malloc( image->stride() * image->size().height());
    std::memcpy(data, image->data(), image->stride() * image->size().height());
    return new MImage{image->size(), image->hasAlpha(), data};
}

MImage::MImage ( MImage&& other )
    : m_size{other.m_size}
    , m_alpha{other.m_alpha}
    , m_data{other.m_data}
{
    other.m_data = nullptr;
}

MImage::~MImage()
{
    std::free ( m_data );
}

MTexture* MImage::createTexture() const
{
    auto texture = new MTexture;
    texture->setSize(size());
    texture->bind();

    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    GLenum format = hasAlpha() ? GL_RGBA : GL_RGB;
    glTexImage2D ( GL_TEXTURE_2D, 0, format, size().width(), size().height(), 0, format, GL_UNSIGNED_BYTE, data() );
    return texture;
}
