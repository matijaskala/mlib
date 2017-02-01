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

#include "mdl.h"

#include <MDebug>

#include <unordered_map>
#include <nonstd/dl>

using namespace std;

static unordered_map<string,void*> modules;

bool MDL::open ( string file )
{
    if ( file.empty() )
        return true;
    void*& module = modules[file];
    if ( module ) {
        mDebug() << "Module '" << file << "' already loaded";
        return true;
    }
    module = dl::open ( file.c_str() );
    if ( module == nullptr )
        mDebug(ERROR) << dl::error();
    return module;
}

bool MDL::close ( string file )
{
    if ( file.empty() )
        return true;
    void*& module = modules[file];
    if ( module == nullptr ) {
        mDebug() << "Module '" << file << "' was not loaded";
        return true;
    }
    if ( !dl::close(module) )
        mDebug() << dl::error();
    else {
        modules.erase(file);
        return true;
    }
    return false;
}

MDL MDL::get ( string file )
{
    if ( file.empty() )
        return MDL{nullptr};
    void* module = modules[file];
    if ( module == nullptr )
        mDebug() << "Module '" << file << "' not loaded";
    return MDL{module};
}

template<>
void* MDL::symbol ( string name )
{
    return dl::sym(m_ptr, name.c_str());
}
