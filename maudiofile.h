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

#ifndef MAUDIOFILE_H
#define MAUDIOFILE_H

#include <MResource>
#include <cstdint>
#include <vector>

class M_EXPORT MAudioFile : public MResource
{
    class Buffer {
    public:
        Buffer () : m_buffer{new std::vector<std::uint8_t>}, m_refcount{new int} {}
        Buffer ( const Buffer& other ) : m_buffer{other.m_buffer}, m_refcount{other.m_refcount} { ++*m_refcount; }
        ~Buffer () { if ( !--*m_refcount ) { delete m_buffer; delete m_refcount; } }
        Buffer& operator= ( const Buffer& ) = default;
        std::vector<std::uint8_t>* operator->() { return m_buffer; }

    private:
        std::vector<std::uint8_t>* m_buffer;
        volatile int* m_refcount{};
    };

public:
    bool stereo = false;
    int freq = 0;
    Buffer buffer{};
    void playSync ();
    void play ();

private:
    static void m_playSync ( bool stereo, int freq, Buffer buffer );
};

#endif // MAUDIOFILE_H
