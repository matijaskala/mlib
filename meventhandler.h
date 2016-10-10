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

#ifndef MEVENTHANDLER_H
#define MEVENTHANDLER_H

#include <mglobal.h>
#include <cstdint>
#include <stack>
#include <nonstd/signal>

enum MKey : std::uint32_t;
class MEventHandler
{
public:
    class M_EXPORT Stack {
        std::stack< MEventHandler* > pointers;
    public:
        Stack();
        template< class _Handler, typename... _Args >
        void push( _Args... __args ) {
            pointers.push ( new _Handler ( __args... ) );
        }
        void pop();
        MEventHandler& top();
        bool empty() const;

        non_std::slot<> quit = [this] { top().quit(); };
        non_std::slot<MKey,std::uint32_t> keyPressed = [this] ( MKey key, std::uint32_t mods ) { top().keyPressed ( key, mods ); };
        non_std::slot<MKey,std::uint32_t> keyReleased = [this] ( MKey key, std::uint32_t mods ) { top().keyReleased ( key, mods ); };
    };

protected:
    virtual ~MEventHandler() = default;

    virtual void quit() {}
    virtual void keyPressed ( MKey key, std::uint32_t mods ) {}
    virtual void keyReleased ( MKey key, std::uint32_t mods ) {}
};

#endif // MEVENTHANDLER_H
