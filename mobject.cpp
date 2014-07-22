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

#include "mobject.h"
#include "mobject_p.h"

MObjectPrivate::MObjectPrivate ( MObject* q ) : q ( q )
{
    parent = nullptr;
}
MObjectPrivate::~MObjectPrivate()
{
}

MObject::MObject ( MObject* parent )
    : d ( new MObjectPrivate ( this ) )
{
    reparent ( parent );
}

MObject::~MObject()
{
    delete d;

}

void MObject::reparent ( MObject* parent )
{
    if ( d->parent )
        d->parent->d->children.remove ( this );
    d->parent = parent;
    if ( d->parent )
        d->parent->d->children.push_back ( this );
}

const std::list< MObject* >& MObject::children()
{
    return d->children;
}

void MObject::disconnect_private ( MObjectSignalBase* signal, _Method< MObject > slot )
{
    for ( MObjectSlotBase* _slot: d->signal_connections )
        if ( _slot->signal == signal && _slot->slot_ptr == slot )
            d->signal_connections.remove ( _slot );
}


