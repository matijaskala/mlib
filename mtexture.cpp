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

MTexture::MTexture()
    : d{new MTexturePrivate}
{
    glGenTextures(1, &d->tex);
}

MTexture::~MTexture()
{
    glDeleteTextures(1, &d->tex);
    delete d;
}

void MTexture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, d->tex);
}

unsigned int MTexture::texture() const
{
    return d->tex;
}

const MSize& MTexture::size() const
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
