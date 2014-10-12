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

#ifndef MFONTPRIVATE_H
#define MFONTPRIVATE_H

#include <map>
#include <string>
#include <MDataLoader>

#include <ft2build.h>
#include FT_FREETYPE_H

class MFontPrivate
{
public:
    FT_Face face;

    std::map<std::tuple<wchar_t,std::uint16_t>,MFont::Glyph*> glyphs;

private:
};

#endif // MFONTPRIVATE_H
