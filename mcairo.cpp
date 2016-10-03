/*
 * This file is part of MLib
 * Copyright (C) 2016  Matija Skala <mskala@gmx.com>
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

#include <mcairo.h>

static std::uint8_t multiply_alpha ( std::uint8_t alpha, std::uint8_t color ) {
    int t = alpha * color + 0x80;
    return ((t + (t >> 8)) >> 8);
}
void mcairo_from_rgba ( MSize size, std::uint8_t* data, int stride, bool hasAlpha )
{
    if ( stride / size.width() != 4 )
        return;
    for ( int x = 0; x < size.width(); x++ )
        for ( int y = 0; y < size.height(); y++ ) {
            auto b = data + y * stride + x * 4;
            auto alpha = hasAlpha ? b[3] : 0xff;
            if ( !alpha )
                memset (b, 0, 4);
            else {
                if ( alpha != 0xff ) {
                    b[0] = multiply_alpha ( alpha, b[0] );
                    b[1] = multiply_alpha ( alpha, b[1] );
                    b[2] = multiply_alpha ( alpha, b[2] );
                }
                *reinterpret_cast<std::uint32_t*> ( b ) = b[0] << 16 | b[1] << 8 | b[2] << 0 | alpha << 24;
            }
        }
}

static std::uint8_t unpremultiply_alpha ( std::uint8_t alpha, std::uint8_t color )
{
    return (color * 0xff + alpha/2) / alpha;
}
void mcairo_to_rgba ( MSize size, std::uint8_t* data, int stride, bool hasAlpha )
{
    if ( stride / size.width() != 4 )
        return;
    for ( int x = 0; x < size.width(); x++ )
        for ( int y = 0; y < size.height(); y++ ) {
            auto b = data + y * stride + x * 4;
            auto pixel = *reinterpret_cast<std::uint32_t*> ( b );
            std::uint8_t alpha = hasAlpha ? pixel >> 24 : 0xff;
            if ( !alpha )
                memset ( b, 0, 4 );
            else {
                b[0] = unpremultiply_alpha ( alpha, (pixel & 0xff0000) >> 16 );
                b[1] = unpremultiply_alpha ( alpha, (pixel & 0x00ff00) >>  8 );
                b[2] = unpremultiply_alpha ( alpha, (pixel & 0x0000ff) >>  0 );
                b[3] = alpha;
            }
        }
}
