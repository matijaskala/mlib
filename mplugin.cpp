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
#include <dlfcn.h>

MPlugin* MPlugin::_load ( const std::string& file, std::string&& symbol )
{
    void* handle = dlopen ( file.c_str(), RTLD_NOW );
    if ( !handle ) {
        mDebug ( ERROR ) << dlerror();
        return NULL;
    }

    symbol.insert ( 0, "Load" );
    void* func = dlsym ( handle, symbol.c_str() );
    if ( !func ) {
        mDebug ( ERROR ) << dlerror();
        dlclose(handle);
        return NULL;
    }

    MPlugin* plugin = reinterpret_cast< MPlugin* ( * ) () > ( func ) ();
    if ( plugin )
        plugin->d = handle;
    else
        dlclose(handle);
    return plugin;
}

void MPlugin::unload(MPlugin* plugin)
{
    void* handle = plugin->d;
    delete plugin;
    dlclose ( handle );
}
