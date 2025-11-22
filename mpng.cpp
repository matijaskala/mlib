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

#include <mresourceloader.h>
#include <mdebug.h>
#include <mimage.h>
#include <fstream>
#include <png.h>

class MPNG : public MResourceLoader {
    virtual bool valid ( std::string file ) override;
    virtual MResource* load ( std::string file ) override;
    virtual MResource::Type type() override { return MResource::Image; }
    virtual std::string name() override { return "png"; }
};

M_EXPORT MPNG png;

bool MPNG::valid ( std::string file )
{
    png_byte header[8];
    std::ifstream stream { file, std::ios::binary };
    if ( !stream.is_open() )
        mDebug() << file << ": No such file or directory.";
    stream.read ( reinterpret_cast<char*> ( header ), 8 );
    return stream.good() && !png_sig_cmp ( header, 0, 8 );
}

static void readData ( png_structp png, png_bytep data, png_size_t length )
{
    png_voidp a = png_get_io_ptr ( png );
    std::ifstream* stream = static_cast<std::ifstream*> ( a );
    stream->read ( reinterpret_cast<char*> ( data ), length );
}

MResource* MPNG::load ( std::string file )
{
    png_structp png = png_create_read_struct ( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if ( !png )
        return nullptr;
    png_infop info = png_create_info_struct ( png );
    if ( !info ) {
        png_destroy_read_struct ( &png, nullptr, nullptr );
        return nullptr;
    }
    if ( setjmp ( png_jmpbuf ( png ) ) ) {
        png_destroy_read_struct ( &png, &info, nullptr );
        return nullptr;
    }
    std::ifstream stream { file, std::ios::binary };
    png_set_read_fn ( png, &stream, readData );
    png_read_info ( png, info );
    png_uint_32 width, height;
    int bitDepth, colorType, interlaceMethod;
    png_get_IHDR ( png, info, &width, &height, &bitDepth, &colorType, &interlaceMethod, nullptr, nullptr );
    png_size_t stride = png_get_rowbytes ( png, info );
    switch ( colorType ) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb ( png );
            break;
        case PNG_COLOR_TYPE_GRAY:
            png_set_expand_gray_1_2_4_to_8 ( png );
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb ( png );
            break;
    }
    if ( png_get_valid ( png, info, PNG_INFO_tRNS ) )
        png_set_tRNS_to_alpha ( png );
    if ( bitDepth == 16 )
        png_set_strip_16 ( png );
    if ( bitDepth < 8 )
        png_set_packing ( png );
    if ( interlaceMethod != PNG_INTERLACE_NONE )
        png_set_interlace_handling ( png );
    png_set_filler ( png, 0xff, PNG_FILLER_AFTER );
    png_read_update_info ( png, info );
    bitDepth = png_get_bit_depth ( png, info );
    stride = png_get_rowbytes ( png, info );
    png_byte channels = png_get_channels ( png, info );
    png_bytep data = static_cast<png_bytep>( calloc ( height * stride, sizeof *data ) );
    png_bytepp rowPointers = new png_bytep[height];
    for ( png_uint_32 i = 0; i < height; i++ ) {
        rowPointers[i] = data + i * stride;
    }
    png_read_image ( png, rowPointers );
    png_read_end ( png, info );
    delete[] rowPointers;
    png_destroy_read_struct ( &png, &info, nullptr );
    return new MImage{{width,height}, bitDepth * channels == 32, data};
}

