/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "mfont.h"
#include "mfont_p.h"

#include <map>

static std::map< std::string, MFont* > map;

MFontPrivate::MFontPrivate ( const char* f )
    : font(f)
{
}

MFont::MFont ( std::string file )
    : d ( new MFontPrivate ( file.c_str() ) )
{

}

MFont::~MFont()
{
    delete d;

}

bool MFont::load ( const std::string& file )
{
    MFont* font = new MFont ( file );
    if ( font->d->font.Error() ) {
        delete font;
        return false;
    }
    else {
        map[file] = font;
        return true;
    }
}

void MFont::unload ( const std::string& file )
{
    auto i = map.find ( file );
    delete i->second;
    map.erase ( i );
}

void MFont::unload ( const MFont* texture )
{
    for ( auto position: map )
        if ( position.second == texture )
            map.erase ( position.first );
    delete texture;
}

MFont* MFont::get ( const std::string& file )
{
    return map[file];
}

bool MFont::setFaceSize ( std::uint16_t size, std::uint16_t res )
{
    return d->font.FaceSize ( size, res );
}

void MFont::render ( std::string text )
{
    d->font.Render ( text.c_str() );
}
