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

#include "mfont.h"

#include <map>
#include <nonstd/casts>
#include <MDebug>
#include <MResourceLoader>
#include <MTexture>

using namespace std;

static map< string, MFont* > fonts;

bool MFont::load ( string file )
{
    for ( auto loader: MResourceLoader::loaders() ) {
        if ( loader->type() != Font )
            continue;
        MResource* res = loader->load(file);
        if ( !res )
            continue;
        MFont* font = dynamic_cast<MFont*> ( res );
        if ( !font ) {
            delete res;
            continue;
        }
        fonts[file] = font;
        return true;
    }
    return false;
}

void MFont::unload ( string file )
{
    auto i = fonts.find ( file );
    delete i->second;
    fonts.erase ( i );
}

void MFont::unload ( const MFont* font )
{
    for ( auto i = fonts.begin(); i != fonts.end(); i++ )
        if ( i->second == font )
            fonts.erase ( i );
    delete font;
}

MFont* MFont::get ( string file )
{
    return fonts[file];
}

uint16_t MFont::size()
{
    return getSize();
}

MTexture* MFont::render ( string text )
{
    return render(lexical_cast<wstring> ( text ));
}
