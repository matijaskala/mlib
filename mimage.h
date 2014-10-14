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

#include <MResource>
#include <MSize>
#include <string>

class MTexture;
class MImage : public MResource
{
public:
    MImage ( MSize size, bool alpha, std::uint8_t* data );
    virtual ~MImage();

    const MSize& size() const { return m_size; }
    bool hasAlpha() const { return m_alpha; }
    std::uint8_t* data() const { return m_data; }

    MTexture* createTexture() const;

    static bool load ( std::string file );
    static void unload ( std::string file );
    static void unload ( const MImage* image );
    static MImage* get ( std::string file );

private:
    MSize m_size;
    bool m_alpha;
    std::uint8_t* m_data;
};

#endif // MIMAGE_H
