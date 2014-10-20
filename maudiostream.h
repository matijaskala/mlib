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

#include <thread>
#include <list>

class MAudioStream
{
public:
    static MAudioStream* create ( std::istream* stream );
    virtual ~MAudioStream() = default;

    bool eof() { return m_eof; }
    int freq() { return m_freq; }
    bool stereo() { return m_stereo; }

    char buffer[0x4000];
    std::size_t buffer_size;

    void initRead() { m_thread = std::thread { [this] { read(); } }; }
    void waitRead() { m_thread.join(); }

protected:
    virtual void read () = 0;

    struct Interface {
        Interface();
        virtual ~Interface();
        virtual bool valid ( std::istream* stream ) = 0;
        virtual MAudioStream* create ( std::istream* stream ) = 0;
        using List = std::list<Interface*>;
    };

    void setEOF(bool eof = true) { m_eof = eof; }
    void setFreq(int freq) { m_freq = freq; }
    void setStereo(bool stereo) { m_stereo = stereo; }

private:
    std::thread m_thread{};
    bool m_eof{};
    int m_freq{};
    bool m_stereo{};

    static Interface::List interfaces();
};

#endif // MAUDIOSTREAM_H
