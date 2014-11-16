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

#ifndef MEVENTS_H
#define MEVENTS_H

#include <MSignal>

enum class MKey;
namespace MEvents
{
    extern MSignal<> quit;
    extern MSignal<MKey,std::uint32_t> keyPressed;
    extern MSignal<MKey,std::uint32_t> keyReleased;
}

#endif // MEVENTS_H
