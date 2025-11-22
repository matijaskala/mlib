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

#include <mresourceloader.h>
#include <mdebug.h>
#include <mimage.h>
#include <fstream>
#include <cstring>
#include <setjmp.h>
#define boolean boolean__
#include <jpeglib.h>

class MJPG : public MResourceLoader {
    virtual bool valid ( std::string file ) override;
    virtual MResource* load ( std::string file ) override;
    virtual MResource::Type type() override { return MResource::Image; }
    virtual std::string name() override { return "jpg"; }
};

M_EXPORT MJPG jpg;

bool MJPG::valid ( std::string file )
{
    JOCTET magic[4];
    bool inScan = false;
    std::ifstream stream { file, std::ios::binary };
    if ( !stream.is_open() )
        mDebug() << file << ": No such file or directory.";
    stream.read ( reinterpret_cast<char*> ( magic ), 2 );
    if ( !stream.good() || magic[0] != 0xff || magic[1] != JPEG_EOI - 1 )
        return false;
    for (;;) {
        stream.read ( reinterpret_cast<char*> ( magic ), 2 );
        if ( !stream.good() || ( magic[0] != 0xff && !inScan ) )
            return false;
        if ( magic[0] != 0xff || magic[1] == 0xff ) {
            stream.seekg ( -1, std::ios_base::cur );
            continue;
        }
        if ( magic[1] == JPEG_EOI )
            return true;
        if ( inScan && !magic[1] )
            continue;
        if ( magic[1] >= JPEG_RST0 && magic[1] < JPEG_EOI )
            continue;
        stream.read ( reinterpret_cast<char*> ( magic + 2 ), 2 );
        if ( !stream.good() )
            return false;
        if ( !stream.seekg ( ( magic[2] << 8 ) + magic[3] - 2, std::ios_base::cur ) )
            return false;
        if ( magic[1] == JPEG_EOI + 1 )
            inScan = true;
    }
}

MResource* MJPG::load ( std::string file )
{
    struct source_mgr : jpeg_source_mgr {
        std::ifstream* stream;
        JOCTET buffer[0x1000];
    }* src;
    jpeg_decompress_struct cinfo;
    struct error_mgr : jpeg_error_mgr {
        jmp_buf escape;
    } jerr;
    cinfo.err = jpeg_std_error ( &jerr );
    jerr.error_exit = [] ( j_common_ptr cinfo ) { longjmp ( static_cast<error_mgr*> ( cinfo->err )->escape, 1 ); };
    jerr.output_message = [] ( j_common_ptr ) {};
    if ( setjmp ( jerr.escape ) ) {
        jpeg_destroy_decompress ( &cinfo );
        return nullptr;
    }
    jpeg_create_decompress ( &cinfo );
    if ( !cinfo.src ) {
        src = static_cast<source_mgr*> ( cinfo.mem->alloc_small ( reinterpret_cast<j_common_ptr> ( &cinfo ), JPOOL_PERMANENT, sizeof ( source_mgr ) ) );
        cinfo.src = src;
    }
    else
        src = static_cast<source_mgr*> ( cinfo.src );
    src->init_source = [] ( j_decompress_ptr ) {};
    src->fill_input_buffer = [] ( j_decompress_ptr cinfo ) {
        auto src = static_cast<source_mgr*> ( cinfo->src );
        src->stream->read ( reinterpret_cast<char*> ( src->buffer ), sizeof(src->buffer) );
        src->next_input_byte = src->buffer;
        if ( src->stream->good() )
            src->bytes_in_buffer = src->stream->gcount();
        else {
            src->buffer[0] = 0xff;
            src->buffer[1] = JPEG_EOI;
            src->bytes_in_buffer = 2;
        }
        return 1;
    };
    src->skip_input_data = [] ( j_decompress_ptr cinfo, long num_bytes ) {
        if ( num_bytes <= 0 )
            return;
        while ( num_bytes > static_cast<ssize_t> ( cinfo->src->bytes_in_buffer ) ) {
            num_bytes -= cinfo->src->bytes_in_buffer;
            cinfo->src->fill_input_buffer ( cinfo );
        }
        cinfo->src->next_input_byte += num_bytes;
        cinfo->src->bytes_in_buffer -= num_bytes;
    };
    src->resync_to_restart = jpeg_resync_to_restart;
    src->term_source = [] ( j_decompress_ptr ) {};
    src->stream = new std::ifstream { file, std::ios_base::binary };
    src->bytes_in_buffer = 0;
    src->next_input_byte = nullptr;
    jpeg_read_header ( &cinfo, true );
    bool raw = cinfo.num_components == 4;
    cinfo.out_color_space = raw ? JCS_CMYK : JCS_RGB;
    cinfo.quantize_colors = false;
    jpeg_calc_output_dimensions ( &cinfo );
    auto width = cinfo.output_width;
    auto height = cinfo.output_height;
    auto stride = (width * cinfo.num_components + 3) &~3;
    auto data = static_cast<std::uint8_t*> ( std::calloc ( height, stride ) );
    jpeg_start_decompress ( &cinfo );
    while ( cinfo.output_scanline < height ) {
        auto rowptr = static_cast<JSAMPROW> ( data ) + cinfo.output_scanline * stride;
        jpeg_read_scanlines ( &cinfo, &rowptr, static_cast<JDIMENSION> ( 1 ) );
    }
    jpeg_finish_decompress ( &cinfo );
    jpeg_destroy_decompress ( &cinfo );
    if ( raw ) {
        for ( JDIMENSION i = 0; i < width * height; i++ ) {
            auto pixel = data + i * 4;
            std::uint8_t r = pixel[2];
            std::uint8_t g = pixel[1];
            std::uint8_t b = pixel[0];
            pixel[0] = r;
            pixel[1] = g;
            pixel[2] = b;
        }
    }
    return new MImage{{width, height}, raw, data};
}

