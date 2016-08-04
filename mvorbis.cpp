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

#include <MResourceLoader>
#include <MAudioFile>
#include <MAudioStream>
#include <MDebug>

#include <vorbis/vorbisfile.h>
#include <fstream>

struct MVorbisStream : public MAudioStream
{
    static struct Interface : public MAudioStream::Interface
    {
        virtual bool valid ( std::istream* stream );
        virtual MAudioStream* create ( std::istream* stream );
    } iface;
    virtual ~MVorbisStream();
    virtual void read();
    virtual void seek ( double seconds );
    OggVorbis_File vorbisFile;
};

MVorbisStream::Interface MVorbisStream::iface;

bool MVorbisStream::Interface::valid ( std::istream* stream )
{
    stream->seekg(0);
    char header[5]{};
    stream->read ( header, 4 );
    return header == std::string{"OggS"};
}

MAudioStream* MVorbisStream::Interface::create ( std::istream* stream )
{
    auto vorbisStream = new MVorbisStream;
    ov_callbacks callbacks{
        [] (void *ptr, std::size_t size, std::size_t nmemb, void *datasource) -> std::size_t {
            auto stream = static_cast<std::istream*> ( datasource );
            if ( stream->eof() )
                return 0;
            auto buf = static_cast<char*> ( ptr );
            stream->read ( buf, size * nmemb );
            stream->clear();
            return stream->gcount() / size;
        },
        [] (void *datasource, ogg_int64_t offset, int whence) -> int {
            auto stream = static_cast<std::istream*> ( datasource );
            std::ios::seekdir dir;
            switch ( whence ) {
                case SEEK_SET:
                    dir = std::ios::beg;
                    break;
                case SEEK_CUR:
                    dir = std::ios::cur;
                    break;
                case SEEK_END:
                    dir = std::ios::end;
                    break;
                default:
                    return -1;
            }
            stream->seekg ( offset, dir );
            return stream->fail() ? -1 : 0;
        },
        [] (void *datasource) -> int {
            return 0;
        },
        [] (void *datasource) -> long {
            auto stream = static_cast<std::istream*> ( datasource );
            return stream->tellg();
        },
    };
    stream->seekg(0);
    if ( ov_open_callbacks ( stream, &vorbisStream->vorbisFile, nullptr, 0, callbacks ) < 0 ) {
        delete vorbisStream;
        return nullptr;
    }
    auto info = ov_info ( &vorbisStream->vorbisFile, -1 );
    vorbisStream->setFreq ( info->rate );
    vorbisStream->setStereo ( info->channels > 1 );
    return vorbisStream;
}

MVorbisStream::~MVorbisStream()
{
    ov_clear(&vorbisFile);
}

void MVorbisStream::seek ( double seconds )
{
    ov_time_seek(&vorbisFile, seconds);
}

void MVorbisStream::read()
{
    buffer_size = 0;
    long read;
    do {
        read = ov_read ( &vorbisFile, buffer + buffer_size, sizeof(buffer) - buffer_size, 0, 2, 1, nullptr );
        if ( read <= 0 )
            switch ( read ) {
                case 0:
                    return setEOF();
                case OV_HOLE:
                    mDebug(ERROR) << "OV_HOLE";
                    break;
            }
        else
            buffer_size += read;
    } while ( buffer_size + read < sizeof(buffer) );
}
