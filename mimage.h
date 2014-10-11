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

#ifndef MIMAGE_H
#define MIMAGE_H

#include <MDataFile>
#include <MSize>

class MImage : public MDataFile
{
public:
    MImage ( MSize size, bool alpha, std::uint8_t* data )
        : m_size{size}
        , m_alpha{alpha}
        , m_data{data}
        {}

    virtual ~MImage() { delete m_data; }

    const MSize& size() { return m_size; }
    bool hasAlpha() { return m_alpha; }
    std::uint8_t* data() { return m_data; }

private:
    MSize m_size;
    bool m_alpha;
    std::uint8_t* m_data;
};

#endif // MIMAGE_H
