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

#include "mtexture.h"
#include "mtexture_p.h"

#include <GL/glext.h>

MTexture::MTexture ()
    : d{new MTexturePrivate}
{
    glGenTextures(1, &d->tex);
}

MTexture::~MTexture ()
{
    glDeleteTextures(1, &d->tex);
    delete d;
}

void MTexture::bind () const
{
    glBindTexture(GL_TEXTURE_2D, d->tex);
}

void MTexture::image2D ( MSize size, int format, void* data )
{
    setSize(size);
    bind();

    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    GLenum glFormat;
    switch ( format ) {
        case 1:
            glFormat = GL_RGB;
            break;
        case 2:
            glFormat = GL_ALPHA;
            break;
        case 3:
            glFormat = GL_RGBA;
            break;
    }
    glTexImage2D ( GL_TEXTURE_2D, 0, glFormat, size.width(), size.height(), 0, glFormat, GL_UNSIGNED_BYTE, data );
}

unsigned int MTexture::texture () const
{
    return d->tex;
}

const MSize& MTexture::size () const
{
    return d->size;
}

void MTexture::setSize ( MSize size )
{
    d->size = size;
}

void MTexture::draw ( int x1, int y1, int x2, int y2 ) const
{
    bind();
    glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex2f(x1,y1);
        glTexCoord2f(1,0);
        glVertex2f(x2,y1);
        glTexCoord2f(1,1);
        glVertex2f(x2,y2);
        glTexCoord2f(0,1);
        glVertex2f(x1,y2);
    glEnd();
}
