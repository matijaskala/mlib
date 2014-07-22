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

#include "mplugin.h"

#include "mdebug.h"
#include "nonstd/module"
#include <map>

static std::map < std::string, non_std::module > loaded;

MPlugin* MPlugin::_load ( const std::string& file, const std::string& symbol )
{
    non_std::module mod = loaded[file];
    if ( !mod.is_open() && !mod.open ( LIBDIR + mod.prefix() + file + mod.suffix() ) ) {
        mDebug ( ERROR ) << non_std::module::last_error();
        return NULL;
    }

    auto create = mod.symbol< MPlugin*() > ( "__M_EXPORTED_" + symbol + "_CREATE__" );
    if ( !create ) {
        mDebug ( ERROR ) << non_std::module::last_error();
        mod.close();
        return NULL;
    }

    MPlugin* plugin = create();
    if ( plugin )
        loaded[file] = mod;
    else
        mod.close();
    return plugin;
}

void MPlugin::unload(MPlugin* plugin)
{
    delete plugin;
}
