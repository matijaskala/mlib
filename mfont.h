/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include <MDataFile>
#include <string>

class MFont : public MDataFile
{
public:
    MFont ( std::string file );
    virtual ~MFont();
    static bool load ( const std::string& file );
    static void unload ( const std::string& file );
    static void unload ( const MFont* texture );
    static MFont* get ( const std::string& file );

private:
    class MFontPrivate* const d;
};

#endif // MFONT_H
