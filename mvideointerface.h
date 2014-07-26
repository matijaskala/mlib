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

#ifndef MVIDEOINTERFACE_H
#define MVIDEOINTERFACE_H

#include <MSize>
#include <list>

class MVideoInterface
{
public:
    MVideoInterface();
    virtual ~MVideoInterface();
    MSize screen_size;
    void test ();
    virtual void beginPaint();
    virtual void endPaint();
    virtual bool init();
    virtual void fini() = 0;
    virtual void handleEvents() {}
    virtual bool setVideoMode ( int x, int y ) = 0;
    static std::list< MVideoInterface* >& interfaces();
};

#endif // MVIDEOINTERFACE_H
