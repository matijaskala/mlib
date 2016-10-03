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
#include <istream>

static struct VorbisInterface : public MAudioStreamInterface
{
    virtual bool valid ( std::istream* stream ) const;
    virtual void init ( MAudioStream* audioStream ) const;
    virtual void fini ( MAudioStream* audioStream ) const;
    virtual void read ( MAudioStream* audioStream ) const;
    virtual void seek ( MAudioStream* audioStream, double seconds ) const;
    virtual double tell ( MAudioStream* audioStream ) const;
    virtual std::list<std::string> getTag ( MAudioStream* audioStream, MAudioTag tag ) const override;
} iface;

bool VorbisInterface::valid ( std::istream* stream ) const
{
    stream->seekg(0);
    char header[5]{};
    stream->read ( header, 4 );
    return header == std::string{"OggS"};
}

void VorbisInterface::init ( MAudioStream* audioStream ) const
{
    auto vorbisFile = new OggVorbis_File;
    ov_callbacks callbacks{
        [] (void *ptr, std::size_t size, std::size_t nmemb, void *datasource) -> std::size_t {
            auto audioStream = static_cast<MAudioStream*> ( datasource );
            if ( stream ( audioStream ) ->eof() ) {
                setEOF ( audioStream );
                return 0;
            }
            auto buf = static_cast<char*> ( ptr );
            stream ( audioStream ) ->read ( buf, size * nmemb );
            stream ( audioStream ) ->clear();
            return stream ( audioStream ) ->gcount() / size;
        },
        [] (void *datasource, ogg_int64_t offset, int whence) -> int {
            auto audioStream = static_cast<MAudioStream*> ( datasource );
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
            stream ( audioStream ) ->seekg ( offset, dir );
            return stream ( audioStream ) ->fail() ? -1 : 0;
        },
        [] (void *datasource) -> int {
            return 0;
        },
        [] (void *datasource) -> long {
            auto audioStream = static_cast<MAudioStream*> ( datasource );
            return stream ( audioStream ) ->tellg();
        },
    };
    stream ( audioStream ) ->seekg(0);
    if ( ov_open_callbacks ( audioStream, vorbisFile, nullptr, 0, callbacks ) < 0 ) {
        delete vorbisFile;
        return;
    }
    auto info = ov_info ( vorbisFile, -1 );
    setEOF ( audioStream, false );
    setFreq ( audioStream, info->rate );
    setStereo ( audioStream, info->channels > 1 );
    setUserData ( audioStream, vorbisFile );
    setValid ( audioStream );
}

void VorbisInterface::fini ( MAudioStream* audioStream ) const
{
    ov_clear ( &userdata<OggVorbis_File> ( audioStream ) );
}

void VorbisInterface::seek ( MAudioStream* audioStream, double seconds ) const
{
    ov_time_seek ( &userdata<OggVorbis_File> ( audioStream ), seconds );
}

double VorbisInterface::tell ( MAudioStream* audioStream ) const
{
    return ov_time_tell ( &userdata<OggVorbis_File> ( audioStream ) );
}

void VorbisInterface::read ( MAudioStream* audioStream ) const
{
    audioStream->buffer_size = 0;
    long read;
    do {
        read = ov_read ( &userdata<OggVorbis_File> ( audioStream ), audioStream->buffer + audioStream->buffer_size,
                         sizeof(audioStream->buffer) - audioStream->buffer_size, 0, 2, 1, nullptr );
        if ( read <= 0 )
            switch ( read ) {
                case 0:
                    setEOF ( audioStream );
                    return;
                case OV_HOLE:
                    mDebug(ERROR) << "OV_HOLE";
                    break;
            }
        else
            audioStream->buffer_size += read;
    } while ( audioStream->buffer_size + read < sizeof(audioStream->buffer) );
}

std::list<std::string> VorbisInterface::getTag ( MAudioStream* audioStream, MAudioTag tag ) const
{
    auto comment = ov_comment ( &userdata<OggVorbis_File> ( audioStream ), -1 );
    const char* tagName = nullptr;
    switch ( tag ) {
        case M_AUDIO_TAG_TITLE:
            tagName = "TITLE";
            break;
        case M_AUDIO_TAG_ARTIST:
            tagName = "ARTIST";
            break;
        case M_AUDIO_TAG_COMPOSER:
            tagName = "COMPOSER";
            break;
        case M_AUDIO_TAG_ALBUM:
            tagName = "ALBUM";
            break;
        case M_AUDIO_TAG_DATE:
            tagName = "DATE";
            break;
        case M_AUDIO_TAG_COMMENT:
            tagName = "COMMENT";
            break;
        case M_AUDIO_TAG_GENRE:
            tagName = "GENRE";
            break;
        case M_AUDIO_TAG_TRACK_NUMBER:
            tagName = "TRACKNUMBER";
            break;
        case M_AUDIO_TAG_DISC_NUMBER:
            tagName = "DISCNUMBER";
            break;
        case M_AUDIO_TAG_TOTAL_TRACKS:
            tagName = "TRACKTOTAL";
            break;
        case M_AUDIO_TAG_TOTAL_DISCS:
            tagName = "DISCTOTAL";
            break;
    }
    if ( !tagName )
        return {};

    int count = vorbis_comment_query_count(comment, tagName);
    std::list<std::string> ret;
    for ( int i = 0; i < count; i++ )
        ret.push_back(vorbis_comment_query(comment, tagName, i));
    return ret;
}
