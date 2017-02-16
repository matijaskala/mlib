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

#ifndef MCAIRO_H
#define MCAIRO_H

#include <mtexture.h>
#include <initializer_list>
#include <cstdlib>
#include <cstring>

inline int cairo_format_pick_mgl_format ( int format,
                                                   std::initializer_list<int> rgbaFormats,
                                                   std::initializer_list<int> rgbFormats,
                                                   std::initializer_list<int> aFormats ) {
    for ( int rgbaFormat: rgbaFormats )
        if ( format == rgbaFormat )
            return 3;
    for ( int rgbFormat: rgbFormats )
        if ( format == rgbFormat )
            return 1;
    for ( int aFormat: aFormats )
        if ( format == aFormat )
            return 2;
    return -1;
}

#define cairo_image_surface_pick_mgl_format(surface) \
    cairo_format_pick_mgl_format ( cairo_image_surface_get_format ( surface ), \
                                 { CAIRO_FORMAT_ARGB32 }, \
                                 { CAIRO_FORMAT_RGB24, CAIRO_FORMAT_RGB30 }, \
                                 { CAIRO_FORMAT_A8, CAIRO_FORMAT_A1 } )

template<typename CairoImageSurface>
inline void cairo_image_surface_format_bind_to_m_texture(CairoImageSurface surface, int format, MTexture* texture) {
    if ( format < 0 )
        return;
    MSize size{cairo_image_surface_get_width ( surface ), cairo_image_surface_get_height ( surface )};
    auto stride = cairo_image_surface_get_stride ( surface );
    auto dest_stride = stride;
    if ( format != 3 )
        dest_stride = ( ( ( format == 1 ) ? 3 : 1 ) * size.width() + 3 ) &~3;
    auto data = static_cast<std::uint8_t*> ( std::malloc ( dest_stride * size.height() ) );
    if ( format & 1 )
        mcairo_to_rgba ( data, cairo_image_surface_get_data ( surface ), dest_stride, stride, size, format & 2 );
    else
        std::memcpy ( data, cairo_image_surface_get_data ( surface ), stride * size.height() );
    texture->image2D ( size, format, data );
    std::free ( data );
}

#define cairo_image_surface_bind_to_m_texture(surface, texture) \
    cairo_image_surface_format_bind_to_m_texture ( surface, cairo_image_surface_pick_mgl_format ( surface ), texture )

M_EXPORT void mcairo_from_rgba ( std::uint8_t* dest_data, std::uint8_t* src_data,
                                 std::uint_fast16_t dest_stride, std::uint_fast16_t src_stride,
                                 MSize size, bool hasAlpha );
M_EXPORT void mcairo_to_rgba ( std::uint8_t* dest_data, std::uint8_t* src_data,
                               std::uint_fast16_t dest_stride, std::uint_fast16_t src_stride,
                               MSize size, bool hasAlpha );

#endif // MCAIRO_H
