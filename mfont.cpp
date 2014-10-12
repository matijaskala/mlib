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
#include "mfont_p.h"

#include <MDebug>
#include <MTexture>
#include <locale>

#include FT_OUTLINE_H
#include <GL/gl.h>

#define STIRIINSESTDESET 64

using namespace std;

static map< string, MFont* > fonts;

MFont::Glyph::Glyph ( MSize size, uint8_t* data, decltype(m_advance)* advance, decltype(m_bounds)* bounds )
    : m_size{size}
    , m_data{data}
    , m_texture{}
    , m_advance{}
    , m_bounds{}
{
    if ( advance )
        m_advance = *advance;
    if ( bounds )
        m_bounds = *bounds;
}

MFont::Glyph::~Glyph()
{
    if ( m_texture )
        delete m_texture;
}

const MTexture* MFont::Glyph::texture()
{
    if ( m_texture )
        return m_texture;

    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, size().width(), size().height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, data());

    glPopClientAttrib();

    return m_texture = new MTexture{size(),tex};
}

MFont::MFont ( void* face )
    : d{new MFontPrivate}
{
    d->face = static_cast<FT_Face> ( face );

    setFaceSize ( 20 );
}

MFont::~MFont()
{
    FT_Done_Face ( d->face );
    for ( auto pair: d->glyphs )
        delete pair.second;

    delete d;
}

bool MFont::load ( string file )
{
    for ( auto loader: MDataLoader::loaders() ) {
        if ( loader->type() != Font )
            continue;
        MDataFile* res = loader->load(file);
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
    for ( auto position: fonts )
        if ( position.second == font )
            fonts.erase ( position.first );
    delete font;
}

MFont* MFont::get ( string file )
{
    return fonts[file];
}

bool MFont::setFaceSize ( uint16_t size, uint16_t res )
{
    if ( size == d->face->size->metrics.x_ppem )
        return true;
    return FT_Set_Char_Size ( d->face, 0, size * STIRIINSESTDESET, res, res ) == 0;
}

void MFont::render ( string text )
{
    wstring wstr;
    {
        locale l{""};
        using cvt_t = std::codecvt< wchar_t, char, mbstate_t >;
        const cvt_t& cvt = std::use_facet< cvt_t > ( l );
        wchar_t* pwc;
        const char* pc;
        auto dest = new wchar_t[text.length() + 1];
        auto state = mbstate_t{};
        switch ( cvt.in(state, text.c_str(), text.c_str() + text.length() + 1, pc, dest, dest + text.length() + 1, pwc) )
        {
            case cvt_t::ok:
                break;
            case cvt_t::error:
                mDebug() << "an error occurred while extracting characters from string '" << text << "'";
        }
        wstr = dest;
        delete[] dest;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);

    long off{};
    for ( wchar_t c: wstr ) {
        auto g = glyph(c, d->face->size->metrics.x_ppem);
        g->texture()->draw ( off + g->bounds().x1/STIRIINSESTDESET, g->bounds().y2/STIRIINSESTDESET,
                             off + g->bounds().x2/STIRIINSESTDESET, g->bounds().y1/STIRIINSESTDESET );
        off += g->advance().x/STIRIINSESTDESET;
    }

    glPopAttrib();
}

MFont::Glyph* MFont::glyph ( wchar_t code, uint16_t size )
{
    auto& glyph = d->glyphs[std::make_tuple(code,size)];
    if ( glyph )
        return glyph;
    setFaceSize(size);
    FT_Load_Char ( d->face, code, FT_LOAD_RENDER );
    FT_BBox bbox;
    FT_Outline_Get_CBox(&d->face->glyph->outline, &bbox);
    using advance_t = std::remove_cv<decltype(glyph->advance())>::type;
    using bounds_t = std::remove_cv<decltype(glyph->bounds())>::type;
    auto advance = (advance_t)d->face->glyph->advance;
    auto bounds = (bounds_t)bbox;
    auto& bitmap = d->face->glyph->bitmap;
    return glyph = new Glyph{{bitmap.width,bitmap.rows},bitmap.buffer,&advance,&bounds};
}
