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

#include "mimageloader.h"
#include "mimage.h"
#include <list>

struct GenericImageLoader : MImageLoader {
    GenericImageLoader();
    virtual ~GenericImageLoader();
    virtual MImage* load ( const std::string& file ) override;
    virtual void unload ( MImage* image ) override;
    virtual bool valid ( const std::string& file ) override;
    std::list < MImagePlugin* > plugins;
};

GenericImageLoader::GenericImageLoader()
{
    MImagePlugin* plugin = MPlugin::load<MImagePlugin> ( "./libmpng" );
    plugins.push_front ( plugin );
}

GenericImageLoader::~GenericImageLoader()
{
    for ( MPlugin* plugin: plugins )
        MPlugin::unload ( plugin );
}

MImage* GenericImageLoader::load ( const std::string& file )
{
    for ( MImagePlugin* plugin: plugins )
        if ( plugin->valid ( file ) ) {
            MImage* image = plugin->load ( file );
            if ( image )
                return image;
        }
    return nullptr;
}

void GenericImageLoader::unload ( MImage* image )
{
    delete[] image->data;
    delete image;
}

bool GenericImageLoader::valid ( const std::string& file )
{
    for ( MImagePlugin* plugin: plugins )
        if ( plugin->valid ( file ) )
            return true;
    return false;
}

M_PLUGIN_EXPORT(MImageLoader, GenericImageLoader)
