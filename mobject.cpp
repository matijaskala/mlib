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

#include "mobject.h"

struct MObject::Private {
    MObject* parent = nullptr;
    std::forward_list<MObject*> children{};
};

MObject::MObject ( MObject* parent )
    : d{new Private}
{
    reparent ( parent );
}

MObject::~MObject()
{
    reparent(nullptr);
    delete d;
}

void MObject::reparent ( MObject* parent )
{
    if ( d->parent )
        d->parent->d->children.remove ( this );
    d->parent = parent;
    if ( d->parent )
        d->parent->d->children.push_front ( this );
}

const std::forward_list<MObject*>& MObject::children () const
{
    return d->children;
}
