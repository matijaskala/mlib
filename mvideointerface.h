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

#include <mglobal.h>
#include <msize.h>
#include <list>

enum MVideoFlags {
    M_VIDEO_FLAGS_RESIZABLE = 1 << 0,
    M_VIDEO_FLAGS_FULLSCREEN = 1 << 1,
};

class MWindow;
class M_EXPORT MVideoInterface
{
protected:
    using getProcAddressFunc = void (*(const char* procname))();

public:
    MVideoInterface();
    virtual ~MVideoInterface();
    void test ();
    virtual bool init() = 0;
    virtual void fini() = 0;
    virtual void handleEvents();
    virtual MWindow* createWindow ( int width, int height, MVideoFlags flags = MVideoFlags{} ) = 0;
    virtual void destroyWindow ( MWindow* window );
    static std::list< MVideoInterface* >& interfaces();
    static MVideoInterface* get();
    getProcAddressFunc* getProcAddress;
};

#endif // MVIDEOINTERFACE_H
