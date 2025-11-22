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

#ifndef MFONT_H
#define MFONT_H

#include <mresource.h>
#include <cstdint>

class MTexture;
class M_EXPORT MFont : public MResource
{
public:
    virtual std::uint16_t getSize() = 0;
    virtual bool setSize ( std::uint16_t size, std::uint16_t res = 0 ) = 0;
    virtual MTexture* render ( std::wstring text ) = 0;
    std::uint16_t size();
    MTexture* render ( std::string text );
};

#endif // MFONT_H
