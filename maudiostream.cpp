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

#include "maudiostream.h"

MAudioStream* MAudioStream::create ( std::istream* stream )
{
    for ( auto iface: interfaces() )
        if ( iface->valid ( stream ) )
            return iface->create ( stream );

    return nullptr;
}

MAudioStream::Interface::List& MAudioStream::interfaces() {
    static Interface::List interfaces;
    return interfaces;
}

MAudioStream::Interface::Interface()
{
    interfaces().push_back(this);
}

MAudioStream::Interface::~Interface()
{
    interfaces().remove(this);
}
