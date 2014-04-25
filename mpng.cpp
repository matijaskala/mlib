#include "mimageloader.h"
#include "mdebug.h"
#include "mimage.h"
#include <fstream>
#include <png.h>

class MPNG : public MImagePlugin {
    virtual bool valid ( const std::string& file ) override;
    virtual MImage* load ( const std::string& file ) override;
};

M_PLUGIN_EXPORT ( MImagePlugin, MPNG )

bool MPNG::valid ( const std::string& file )
{
    png_byte header[8];
    std::ifstream stream ( file );
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

MImage* MPNG::load ( const std::string& file )
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
    std::ifstream stream ( file );
    png_set_read_fn ( png, &stream, readData );
    png_read_info ( png, info );
    png_uint_32 width = png_get_image_width ( png, info );
    png_uint_32 height = png_get_image_height ( png, info );
    png_byte bitDepth = png_get_bit_depth ( png, info );
    png_byte colorType = png_get_color_type ( png, info );
    png_size_t stride = png_get_rowbytes ( png, info );
    if ( bitDepth == 16 )
        png_set_strip_16 ( png );
    switch ( colorType ) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb ( png );
            break;
        case PNG_COLOR_TYPE_GRAY:
            if ( bitDepth < 8 )
                png_set_expand_gray_1_2_4_to_8 ( png );
            break;
    }
    bitDepth = png_get_bit_depth ( png, info );
    png_byte channels = png_get_channels ( png, info );
    png_bytepp rowPointers = new png_bytep[height];
    png_bytep data = new png_byte[height * stride];
    for ( png_uint_32 i = 0; i < height; i++ ) {
        rowPointers[i] = data + i * stride;
    }
    png_read_image ( png, rowPointers );
    delete[] rowPointers;
    png_destroy_read_struct ( &png, &info, nullptr );
    MImage* image = new MImage;
    image->size = MSize ( width, height );
    image->depth = bitDepth * channels;
    image->stride = stride;
    image->data = data;
    image->rMask = 0x0000ff;
    image->gMask = 0x00ff00;
    image->bMask = 0xff0000;
    return image;
}
