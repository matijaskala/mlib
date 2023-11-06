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

#ifndef MAUDIOSTREAM_H
#define MAUDIOSTREAM_H

#include <mglobal.h>
#include <list>
#include <string>
#include <thread>

enum MAudioTag {
    M_AUDIO_TAG_TITLE,
    M_AUDIO_TAG_ARTIST,
    M_AUDIO_TAG_COMPOSER,
    M_AUDIO_TAG_ALBUM,
    M_AUDIO_TAG_DATE,
    M_AUDIO_TAG_COMMENT,
    M_AUDIO_TAG_GENRE,
    M_AUDIO_TAG_TRACK_NUMBER,
    M_AUDIO_TAG_DISC_NUMBER,
    M_AUDIO_TAG_TOTAL_TRACKS,
    M_AUDIO_TAG_TOTAL_DISCS,
};

class MAudioStreamInterface;
class M_EXPORT MAudioStream
{
    friend MAudioStreamInterface;

public:
    explicit MAudioStream ( std::istream* stream );
    explicit MAudioStream ( std::streambuf* streambuf );
    explicit MAudioStream ( const std::string& file );
    explicit MAudioStream ( const char* file );
    MAudioStream ( const MAudioStream& ) = delete;
    ~MAudioStream ();
    MAudioStream& operator= ( const MAudioStream& ) = delete;

    bool eof () { return m_eof; }
    int freq () { return m_freq; }
    bool stereo () { return m_stereo; }
    bool valid () { return m_valid; }

    char buffer[0x4000];
    std::size_t buffer_size;

    void initRead();
    void waitRead();

    void seek ( std::chrono::duration < double > seconds );
    std::chrono::duration < double > tell ();

    std::list<std::string> getTag ( MAudioTag tag );
    std::list<std::string> getTitle () { return getTag(M_AUDIO_TAG_TITLE); }
    std::list<std::string> getArtist () { return getTag(M_AUDIO_TAG_ARTIST); }
    std::list<std::string> getComposer () { return getTag(M_AUDIO_TAG_COMPOSER); }
    std::list<std::string> getAlbum () { return getTag(M_AUDIO_TAG_ALBUM); }
    std::list<std::string> getDate () { return getTag(M_AUDIO_TAG_DATE); }
    std::list<std::string> getComment () { return getTag(M_AUDIO_TAG_COMMENT); }
    std::list<std::string> getGenre () { return getTag(M_AUDIO_TAG_GENRE); }
    std::list<std::string> getTrackNumber () { return getTag(M_AUDIO_TAG_TRACK_NUMBER); }
    std::list<std::string> getDiscNumber () { return getTag(M_AUDIO_TAG_DISC_NUMBER); }
    std::list<std::string> getTotalTracks () { return getTag(M_AUDIO_TAG_TOTAL_TRACKS); }
    std::list<std::string> getTotalDiscs () { return getTag(M_AUDIO_TAG_TOTAL_DISCS); }

private:
    bool m_eof = true;
    int m_freq;
    bool m_stereo;
    void* m_userdata = nullptr;
    bool m_valid = false;
    MAudioStreamInterface* m_interface;
    std::istream* m_stream;
    std::thread m_thread{};
};

class M_EXPORT MAudioStreamInterface {
public:
    MAudioStreamInterface();
    virtual ~MAudioStreamInterface();
    virtual bool valid ( std::istream* stream ) const = 0;
    virtual void init ( MAudioStream* audioStream ) const = 0;
    virtual void fini ( MAudioStream* audioStream ) const = 0;
    virtual void read ( MAudioStream* audioStream ) const = 0;
    virtual void seek ( MAudioStream* audioStream, double seconds ) const = 0;
    virtual double tell ( MAudioStream* audioStream ) const = 0;
    virtual std::list<std::string> getTag ( MAudioStream* audioStream, MAudioTag tag ) const;

    static std::list<MAudioStreamInterface*>& interfaces();

protected:
    static void setEOF ( MAudioStream* audioStream, bool eof = true ) { audioStream->m_eof = eof; }
    static void setFreq ( MAudioStream* audioStream, int freq ) { audioStream->m_freq = freq; }
    static void setStereo ( MAudioStream* audioStream, bool stereo ) { audioStream->m_stereo = stereo; }
    static void setUserData ( MAudioStream* audioStream, void* userdata ) { audioStream->m_userdata = userdata; }
    static void setValid ( MAudioStream* audioStream, bool valid = true ) { audioStream->m_valid = valid; }
    template < typename _Data >
    static _Data& userdata ( MAudioStream* audioStream ) { return *static_cast<_Data*> ( audioStream->m_userdata ); }
    static std::istream* stream ( MAudioStream* audioStream ) { return audioStream->m_stream; }
};

#endif // MAUDIOSTREAM_H
