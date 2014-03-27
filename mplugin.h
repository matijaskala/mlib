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

#ifndef MPLUGIN_H
#define MPLUGIN_H

#include "mglobal.h"

#define M_PLUGIN_EXPORT(TYPE,NAME) \
extern "C" M_EXPORT \
MPlugin* __M_EXPORTED_##TYPE##_CREATE__() { \
    return new NAME; \
}

class MPlugin
{
public:
    template< typename _Plugin >
    static _Plugin* load ( const std::string& file ) {
        MPlugin* plugin = _load ( file, M::typeName< _Plugin > () );
        return dynamic_cast< _Plugin* > ( plugin );
    }
    static void unload ( MPlugin* plugin );
protected:
    MPlugin() = default;
    MPlugin ( const MPlugin& other ) = delete;
    virtual ~MPlugin() {}
private:
    static MPlugin* _load ( const std::string& file, const std::string& symbol );
};

#endif // MPLUGIN_H

