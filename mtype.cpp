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

#include <cstring>
#include <MFont>
#include <MResourceLoader>
#include <MTexture>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/gl.h>
#include <GL/glext.h>

#define STIRIINSESTDESET 64

using namespace std;

static class MType : public MResourceLoader
{
    struct Font : MFont {
        virtual ~Font() override;
        virtual uint16_t getSize() override;
        virtual bool setSize ( uint16_t size, uint16_t res = 0 ) override;
        virtual MTexture* render ( wstring text ) override;
        FT_Face face;
    };

public:
    MType();
    virtual ~MType();
    virtual MResource::Type type() override { return MResource::Font; }

private:
    virtual MResource* load ( string file );
    virtual string name();

    FT_Library library;
} freetype;

MType::MType()
{
    FT_Init_FreeType ( &library );
}

MType::~MType()
{
    FT_Done_FreeType ( library );
}

MResource* MType::load ( string file )
{
    FT_Face face;
    if ( FT_New_Face ( library, file.c_str(), 0, &face ) )
        return nullptr;
    if ( FT_Set_Char_Size ( face, 0, 1280, 0, 0 ) ) {
        FT_Done_Face ( face );
        return nullptr;
    }
    Font* font = new Font;
    font->face = face;
    font->setSize ( 20 );
    return font;
}

string MType::name()
{
    return "ttf";
}

MType::Font::~Font()
{
    FT_Done_Face ( face );
}

uint16_t MType::Font::getSize()
{
    return face->size->metrics.x_ppem;
}

bool MType::Font::setSize ( uint16_t size_, uint16_t res )
{
    if ( size_ == size() )
        return true;
    return FT_Set_Char_Size ( face, 0, size_ * STIRIINSESTDESET, res, res ) == 0;
}

MTexture* MType::Font::render ( wstring text )
{
    int width{};
    int height{};
    int height2{};
    for ( auto c: text ) {
        FT_Load_Char ( face, c, FT_LOAD_RENDER );
        width += face->glyph->advance.x/STIRIINSESTDESET;
        int top = face->glyph->bitmap_top;
        int h = face->glyph->bitmap.rows - top;
        height = height > top ? height : top;
        height2 = height2 > h ? height2 : h;
    }
    uint8_t* data = (uint8_t*)calloc(width * (height + height2), 1);
    long off{};
    for ( auto c: text ) {
        FT_Load_Char ( face, c, FT_LOAD_RENDER );
        auto* glyph = face->glyph;
        for ( unsigned int i = 0; i < glyph->bitmap.rows; i++ )
            std::memcpy ( data + (height - glyph->bitmap_top + i) * width + off + glyph->bitmap_left,
                          glyph->bitmap.buffer + i * glyph->bitmap.width, glyph->bitmap.width );
//        g->texture()->draw ( off + g->bounds().x1/STIRIINSESTDESET, g->bounds().y2/STIRIINSESTDESET,
//                             off + g->bounds().x2/STIRIINSESTDESET, g->bounds().y1/STIRIINSESTDESET );
        off += glyph->advance.x/STIRIINSESTDESET;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);

    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

    auto tex = new MTexture;
    tex->setSize( { width, height + height2 } );
    tex->bind();

    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height + height2, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);

    glPopClientAttrib();

    glPopAttrib();

    std::free(data);

    return tex;
}
