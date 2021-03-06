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

#include "mglobal.h"

#include <nonstd/directory>
#include <MDL>
#include <MDebug>
#include "mvideointerface.h"

#include <alc.h>

using namespace non_std;

static bool initialized{false};

void MLib::init ( int& argc, char**& argv )
{
    (void)argc; (void)argv;
    if ( initialized ) {
        mDebug() << "MLib already initialized";
        return;
    }
    initialized = true;

    directory dir ( MLIB_LIBRARY_DIR );
    for ( directory::entry f: dir )
        if ( f.name()[0] != '.' )
            MDL::open(f.path() + f.name());

    auto device = alcOpenDevice ( nullptr );
    auto context = alcCreateContext ( device, nullptr );
    alcMakeContextCurrent ( context );
}

void MLib::quit ( int status )
{
    auto video = MVideoInterface::get();
    if (video)
        video->fini();

    auto context = alcGetCurrentContext ();
    auto device = alcGetContextsDevice ( context );

    alcMakeContextCurrent ( nullptr );
    alcDestroyContext ( context );
    alcCloseDevice ( device );

    exit ( status );
}

