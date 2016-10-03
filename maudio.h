/*
 * This file is part of MLib
 * Copyright (C) 2016  Matija Skala <mskala@gmx.com>
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

#ifndef MAUDIO_H
#define MAUDIO_H

#include <mglobal.h>

namespace MAudio
{
    /**
     *  Sets audio volume.
     *  @param  volume New volume in the range of [0.0 - 1.0].
     */
    M_EXPORT void set_volume ( float volume );

    /**
     *  Returns current audio volume.
     *  @return  Current volume in the range of [0.0 - 1.0].
     */
    M_EXPORT float get_volume ();
}

#endif // MAUDIO_H
