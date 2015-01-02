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

#ifndef MTEXTURE_H
#define MTEXTURE_H

#include <mglobal.h>
#include <MSize>

class M_EXPORT MTexture
{
public:
    MTexture();
    ~MTexture();
    void bind() const;
    unsigned int texture() const;
    const MSize& size() const;
    void setSize ( MSize size );
    void draw ( int x1, int y1, int x2, int y2 ) const;
private:
    class MTexturePrivate* d;
};

#endif // MTEXTURE_H
