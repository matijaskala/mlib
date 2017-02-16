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
#include <mdebug.h>

static std::uint8_t multiply_alpha ( std::uint8_t alpha, std::uint8_t color ) {
    int t = alpha * color + 0x80;
    return ((t + (t >> 8)) >> 8);
}
void mcairo_from_rgba ( std::uint8_t* dest_data, std::uint8_t* src_data,
                        std::uint_fast16_t dest_stride, std::uint_fast16_t src_stride,
                        MSize size, bool hasAlpha )
{
    if ( dest_stride != size.width() * 4 ) {
        mDebug(ERROR) << "unimplemented case: dest_stride = " << dest_stride << ", size.width() = " << size.width();
        return;
    }
    for ( unsigned int y = 0; y < size.height(); y++ )
        for ( unsigned int x = 0; x < size.width(); x++ ) {
            auto src_b = src_data + y * src_stride + x * ( src_stride / size.width() );
            auto dest_b = dest_data + y * dest_stride + x * ( dest_stride / size.width() );
            auto alpha = hasAlpha ? src_b[3] : 0xff;
            if ( !alpha )
                memset ( dest_b, 0, dest_stride / size.width() );
            else {
                dest_b[0] = multiply_alpha ( alpha, src_b[0] );
                dest_b[1] = multiply_alpha ( alpha, src_b[1] );
                dest_b[2] = multiply_alpha ( alpha, src_b[2] );
                *reinterpret_cast<std::uint32_t*> ( dest_b ) = dest_b[0] << 16 | dest_b[1] << 8 | dest_b[2] << 0 | alpha << 24;
            }
        }
}

static std::uint8_t unpremultiply_alpha ( std::uint8_t alpha, std::uint8_t color )
{
    return (color * 0xff + alpha/2) / alpha;
}
void mcairo_to_rgba ( std::uint8_t* dest_data, std::uint8_t* src_data,
                      std::uint_fast16_t dest_stride, std::uint_fast16_t src_stride,
                      MSize size, bool hasAlpha )
{
    if ( src_stride != size.width() * 4 ) {
        mDebug(ERROR) << "unimplemented case: src_stride = " << src_stride << ", size.width() = " << size.width();
        return;
    }
    for ( unsigned int y = 0; y < size.height(); y++ )
        for ( unsigned int x = 0; x < size.width(); x++ ) {
            auto src_b = src_data + y * src_stride + x * ( src_stride / size.width() );
            auto dest_b = dest_data + y * dest_stride + x * ( dest_stride / size.width() );
            auto pixel = *reinterpret_cast<std::uint32_t*> ( src_b );
            std::uint8_t alpha = hasAlpha ? pixel >> 24 : 0xff;
            if ( !alpha )
                memset ( dest_b, 0, dest_stride / size.width() );
            else {
                dest_b[0] = unpremultiply_alpha ( alpha, (pixel & 0xff0000) >> 16 );
                dest_b[1] = unpremultiply_alpha ( alpha, (pixel & 0x00ff00) >>  8 );
                dest_b[2] = unpremultiply_alpha ( alpha, (pixel & 0x0000ff) >>  0 );
                dest_b[3] = alpha;
            }
        }
}
