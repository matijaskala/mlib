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

#include <MDataLoader>
#include <MFont>

#include <ft2build.h>
#include FT_FREETYPE_H

static class MType : public MDataLoader
{
public:
    MType();
    virtual ~MType();
    virtual MDataFile::Type type() override { return MDataFile::Font; }

private:
    virtual MDataFile* load ( std::string file );
    virtual std::string name();

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

MDataFile* MType::load ( std::string file )
{
    FT_Face face;
    if ( FT_New_Face ( library, file.c_str(), 0, &face ) )
        return nullptr;
    if ( FT_Set_Char_Size ( face, 0, 1280, 0, 0 ) ) {
        FT_Done_Face ( face );
        return nullptr;
    }
    return new MFont{face};
}

std::string MType::name()
{
    return "ttf";
}
