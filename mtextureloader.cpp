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

#include "mtextureloader.h"
#include <MTexture>

#include <GL/gl.h>

MDataFile* MTextureLoader::load ( MSize size, std::uint16_t depth, std::uint8_t* data )
{
    GLuint tex;
    glGenTextures ( 1, &tex );
    glBindTexture ( GL_TEXTURE_2D, tex );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    GLenum format = depth == 24 ? GL_RGB : GL_RGBA;
    glTexImage2D ( GL_TEXTURE_2D, 0, format, size.width(), size.height(), 0, format, GL_UNSIGNED_BYTE, data );
    delete[] data;

    return new MTexture ( size, tex );
}
