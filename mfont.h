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

#ifndef MFONT_H
#define MFONT_H

#include <mglobal.h>
#include <MImage>
#include <MResource>
#include <MSize>
#include <string>

class MTexture;
class M_EXPORT MFont : public MResource
{
public:
    class Glyph
    {
        struct { long x; long y; } m_advance;
        struct { long x1; long y1; long x2; long y2; } m_bounds;
    public:
        Glyph ( MSize size, std::uint8_t* data, decltype(m_advance)* advance = nullptr, decltype(m_bounds)* bounds = nullptr );
        ~Glyph();

        const MSize& size() const { return m_size; }
        std::uint8_t* data() const { return m_data; }
        const MTexture* texture();
        const decltype(m_advance)& advance() const { return m_advance; }
        const decltype(m_bounds)& bounds() const { return m_bounds; }

    private:
        MSize m_size;
        std::uint8_t* m_data;
        MTexture* m_texture;
    };

    MFont ( void* face );
    virtual ~MFont();

    static bool load ( std::string file );
    static void unload ( std::string file );
    static void unload ( const MFont* font );
    static MFont* get ( std::string file );

    bool setSize ( std::uint16_t size, std::uint16_t res = 0 );
    std::uint16_t size();
    MImage* render ( std::wstring text );
    MImage* render ( std::string text );
    Glyph* glyph ( wchar_t code, std::uint16_t size );

private:
    class MFontPrivate* const d;
};

#endif // MFONT_H
