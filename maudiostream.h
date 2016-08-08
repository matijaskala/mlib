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
#include <istream>
#include <list>
#include <thread>

class MAudioInterface;
class M_EXPORT MAudioStream
{
    friend class MAudioInterface;

public:
    template <typename _IStream>
    MAudioStream ( _IStream&& stream ) : m_stream{new _IStream{std::move(stream)}} { m_init(); }
    MAudioStream ( std::streambuf* sb ) : m_stream{new std::istream{sb}} { m_init(); }
    ~MAudioStream();

    bool eof() { return m_eof; }
    int freq() { return m_freq; }
    bool stereo() { return m_stereo; }
    bool valid() { return m_valid; }

    char buffer[0x4000];
    std::size_t buffer_size;

    void initRead();
    void waitRead();

    void seek ( std::chrono::duration < double > seconds );

private:
    bool m_eof = true;
    int m_freq;
    bool m_stereo;
    void* m_userdata = nullptr;
    bool m_valid = false;
    MAudioInterface* m_interface;
    std::istream* m_stream;
    std::thread m_thread{};
    void m_init();
};

struct M_EXPORT MAudioInterface {
    MAudioInterface();
    virtual ~MAudioInterface();
    virtual bool valid ( std::istream* stream ) const = 0;
    virtual void init ( MAudioStream* audioStream ) const = 0;
    virtual void fini ( MAudioStream* audioStream ) const = 0;
    virtual void read ( MAudioStream* audioStream ) const = 0;
    virtual void seek ( MAudioStream* audioStream, double seconds ) const = 0;

    static std::list<MAudioInterface*>& interfaces();

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
