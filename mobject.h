/*
 * This file is part of MLib
 * Copyright (C) 2014-2017  Matija Skala <mskala@gmx.com>
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

#ifndef MOBJECT_H
#define MOBJECT_H

#include <mglobal.h>
#include <forward_list>

class M_EXPORT MObject
{
public:
    MObject ( MObject* parent = nullptr );
    virtual ~MObject();
    MObject ( const MObject& other ) = delete;
    MObject& operator= ( const MObject& other ) = delete;

    void reparent ( MObject* parent );
    const std::forward_list<MObject*>& children () const;

private:
    struct Private;
    Private* const d;

};

#endif // MOBJECT_H
