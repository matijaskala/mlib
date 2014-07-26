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

#include "mtextureloader.h"
#include <map>

static std::map< std::string, MTexture* > map;

MTexture::MTexture( MSize size, unsigned int tex )
{
    d = new MTexturePrivate;

    d->size = size;
    d->tex = tex;
}

MTexture::~MTexture()
{
    glDeleteTextures(1, &d->tex);
    delete d;
}

unsigned int MTexture::texture() const
{
    return d->tex;
}

const MSize& MTexture::size() const
{
    return d->size;
}

void MTexture::draw ( int x1, int y1, int x2, int y2 ) const
{
    glBindTexture(GL_TEXTURE_2D,texture());
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

bool MTexture::load ( const std::string& file )
{
    for ( auto loader: MDataLoader::loaders() ) {
        if ( dynamic_cast< MTextureLoader* > ( loader ) == nullptr )
            continue;
        auto texture = dynamic_cast< MTexture* > ( loader->load ( file ) );
        if ( !texture )
            continue;
        map[file] = texture;
        return true;
    }
    return false;
}

void MTexture::unload ( const std::string& file )
{
    auto i = map.find ( file );
    delete i->second;
    map.erase ( i );
}

void MTexture::unload ( const MTexture* texture )
{
    for ( auto position: map )
        if ( position.second == texture )
            map.erase ( position.first );
    delete texture;
}

MTexture* MTexture::get ( const std::string& file )
{
    return map[file];
}
