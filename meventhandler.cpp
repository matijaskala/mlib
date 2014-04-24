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

#include "meventhandler.h"

#include <cstdlib>

static MEventHandler* current_handler = nullptr;
static MEventHandler* fallback_handler = nullptr;

MEventHandler* MEventHandler::current()
{
    class FallbackHandler : public MEventHandler {
        virtual void quit() {
            std::exit ( 0 );
        }
    };
    if ( !current_handler )
        current_handler = fallback_handler = new FallbackHandler;

    return current_handler;
}

void MEventHandler::setCurrent ( MEventHandler* handler )
{
    if ( fallback_handler ) {
        delete fallback_handler;
        fallback_handler = nullptr;
    }

    current_handler = handler;
}
