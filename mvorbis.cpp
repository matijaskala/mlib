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

#include <vorbis/vorbisfile.h>
#include <fstream>

class MVorbis : public MResourceLoader
{
    virtual bool valid ( std::string file );
    virtual MResource* load ( std::string file );
    virtual MResource::Type type() { return MResource::Audio; }
    virtual std::string name() { return "vorbis"; }
} vorbis;

bool MVorbis::valid ( std::string file )
{
    char header[4];
    std::ifstream stream ( file );
    if ( !stream.is_open() )
        return false;
    stream.read ( header, 4 );
    return header == std::string{"OggS"};
}

MResource* MVorbis::load ( std::string file )
{
    OggVorbis_File oggFile;
    FILE* f = fopen ( file.c_str(), "r" );
    if ( !f )
        return nullptr;
    if ( ov_open ( f, &oggFile, nullptr, 0 ) < 0 ) {
        fclose ( f );
        return nullptr;
    }
    auto res = new MAudioFile;
    auto info = ov_info ( &oggFile, -1 );
    res->stereo = info->channels > 1;
    res->freq = info->rate;
#define BUFFER_SIZE 8192
    char buffer[BUFFER_SIZE];
    for (;;) {
        auto bytes = ov_read ( &oggFile, buffer, BUFFER_SIZE, 0, 2, 1, nullptr );
        if ( bytes > 0 )
            res->buffer.insert ( res->buffer.end(), buffer, buffer + bytes );
        else
            break;
    }
    ov_clear ( &oggFile );
    return res;
}
