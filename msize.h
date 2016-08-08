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

#ifndef MSIZE_H
#define MSIZE_H

#include <cstdint>

class MSize
{
public:
    MSize()
        : MSize ( 0,0 )
    {}
    template <typename _W, typename _H>
    MSize ( _W width, _H height )
        : m_width ( width )
        , m_height ( height )
    {}
    std::uint_fast16_t width() const {
        return m_width;
    }
    std::uint_fast16_t height() const {
        return m_height;
    }

    bool operator== ( const MSize& other ) const
    {
        return width() == other.width() && height() == other.height();
    }

    bool operator!= ( const MSize& other ) const
    {
        return width() != other.width() || height() != other.height();
    }

private:
    std::uint_fast16_t m_width;
    std::uint_fast16_t m_height;
};

#endif // MSIZE_H
