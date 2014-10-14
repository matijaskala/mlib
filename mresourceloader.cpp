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

#include "mresourceloader.h"

#include <MDebug>
#include <fstream>

MResourceLoader::MResourceLoader()
{
    loaders().push_back ( this );
}

MResourceLoader::~MResourceLoader()
{
    loaders().remove ( this );
}

bool MResourceLoader::valid ( std::string file )
{
    std::ifstream stream ( file );
    if ( !stream.is_open() )
        mDebug() << file << ": No such file or directory.";
    return stream.good();
}

MResourceLoader* MResourceLoader::get ( std::string name )
{
    for ( MResourceLoader* loader: loaders() )
        if ( loader->name() == name )
            return loader;
    return nullptr;
}

std::list< MResourceLoader* >& MResourceLoader::loaders()
{
    static std::list< MResourceLoader* > loaders;
    return loaders;
}
