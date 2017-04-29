/*
 * This file is part of MLib
 * Copyright (C) 2014-2017  Matija Skala <mskala@gmx.com>
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

#include <opusfile.h>
#include <istream>

static struct OpusInterface : public MAudioStreamInterface
{
    virtual bool valid ( std::istream* stream ) const;
    virtual void init ( MAudioStream* audioStream ) const;
    virtual void fini ( MAudioStream* audioStream ) const;
    virtual void read ( MAudioStream* audioStream ) const;
    virtual void seek ( MAudioStream* audioStream, double seconds ) const;
    virtual double tell ( MAudioStream* audioStream ) const;
    virtual std::list<std::string> getTag ( MAudioStream* audioStream, MAudioTag tag ) const override;
} iface;

bool OpusInterface::valid ( std::istream* stream ) const
{
    stream->seekg(0);
    char header[5]{};
    stream->read ( header, 4 );
    return header == std::string{"OggS"};
}

void OpusInterface::init ( MAudioStream* audioStream ) const
{
    OpusFileCallbacks callbacks{
        [] (void *datasource, opus_uint8 *ptr, int size) -> int {
            auto audioStream = static_cast<MAudioStream*> ( datasource );
            if ( stream ( audioStream ) ->eof() ) {
                setEOF ( audioStream );
                return 0;
            }
            auto buf = reinterpret_cast<char*> ( ptr );
            stream ( audioStream ) ->read ( buf, size );
            stream ( audioStream ) ->clear();
            return stream ( audioStream ) ->gcount();
        },
        [] (void *datasource, opus_int64 offset, int whence) -> int {
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
        [] (void *datasource) -> opus_int64 {
            auto audioStream = static_cast<MAudioStream*> ( datasource );
            return stream ( audioStream ) ->tellg();
        },
        nullptr,
    };
    stream ( audioStream ) ->seekg(0);
    if ( auto opusFile = op_open_callbacks ( audioStream, &callbacks, nullptr, 0, nullptr ) ) {
        auto head = op_head ( opusFile, -1 );
        setEOF ( audioStream, false );
        setFreq ( audioStream, 48000 );
        setStereo ( audioStream, head->channel_count > 1 );
        setUserData ( audioStream, opusFile );
        setValid ( audioStream );
    }
}

void OpusInterface::fini ( MAudioStream* audioStream ) const
{
    op_free ( &userdata<OggOpusFile> ( audioStream ) );
}

void OpusInterface::seek ( MAudioStream* audioStream, double seconds ) const
{
    op_pcm_seek ( &userdata<OggOpusFile> ( audioStream ), seconds * 48000.0 );
}

double OpusInterface::tell ( MAudioStream* audioStream ) const
{
    return op_pcm_tell ( &userdata<OggOpusFile> ( audioStream ) ) / 48000.0;
}

void OpusInterface::read ( MAudioStream* audioStream ) const
{
    audioStream->buffer_size = 0;
    long read;
    do {
        read = op_read ( &userdata<OggOpusFile> ( audioStream ),
                         reinterpret_cast<opus_int16*> (audioStream->buffer + audioStream->buffer_size),
                         sizeof(audioStream->buffer) - audioStream->buffer_size, nullptr );
        if ( read <= 0 )
            switch ( read ) {
                case 0:
                    setEOF ( audioStream );
                    return;
                case OP_HOLE:
                    mDebug(ERROR) << "OP_HOLE";
                    break;
            }
        else
            audioStream->buffer_size += read;
    } while ( audioStream->buffer_size + read < sizeof(audioStream->buffer) );
}

std::list<std::string> OpusInterface::getTag ( MAudioStream* audioStream, MAudioTag tag ) const
{
    auto comment = op_tags ( &userdata<OggOpusFile> ( audioStream ), -1 );
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

    int count = opus_tags_query_count(comment, tagName);
    std::list<std::string> ret;
    for ( int i = 0; i < count; i++ )
        ret.push_back(opus_tags_query(comment, tagName, i));
    return ret;
}
