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

#include "mkeys.h"
#include "nonstd/signal"
#include "msize.h"

class MWindow
{
protected:
    MWindow ( int width, int height );
    virtual void makeCurrent () = 0;
    virtual void resize () = 0;
    virtual void swapBuffers () = 0;

public:
    void beginPaint();
    void endPaint();
    void resize( int width, int height );
    MSize size;

    non_std::signal<> quit{this};
    non_std::signal<MKey,std::uint32_t> keyPressed{this};
    non_std::signal<MKey,std::uint32_t> keyReleased{this};
};
