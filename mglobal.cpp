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

#include <MDirectory>
#include <MDL>
#include <MDebug>

#include <alc.h>

static bool initialized{false};

void M::init ( int& argc, char**& argv )
{
    if ( initialized ) {
        mDebug() << "MLib already initialized";
        return;
    }
    initialized = true;

    MDirectory dir ( MLIB_LIBRARY_DIR );
    for ( MDirectory::Entry f: dir )
        if ( f.name()[0] != '.' )
            MDL::open(f.path() + f.name());

    auto device = alcOpenDevice ( nullptr );
    auto context = alcCreateContext ( device, nullptr );
    alcMakeContextCurrent ( context );
}

void M::quit()
{
    auto context = alcGetCurrentContext ();
    auto device = alcGetContextsDevice ( context );

    alcMakeContextCurrent ( nullptr );
    alcDestroyContext ( context );
    alcCloseDevice ( device );

    exit(0);
}

