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
    for ( auto connection: signal_connections ) {
        connection->signal->connections.remove ( connection );
        delete connection;
    }
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

void MObject::disconnect_private ( SignalBase* signal, _Method< MObject > slot )
{
    for ( auto connection: signal->connections )
        if ( connection->receiver == this && connection->slot == slot ) {
            signal->connections.remove ( connection );
            d->signal_connections.remove ( connection );
            delete connection;
            return;
        }
}


MObject::SignalBase::~SignalBase() {
    for ( auto connection: connections ) {
        connection->receiver->d->signal_connections.remove ( connection );
        delete connection;
    }
}

MObject::ConnectionBase::ConnectionBase ( SignalBase* signal, MObject* receiver, Slot slot )
    : signal ( signal )
    , receiver ( receiver )
    , slot ( slot ) {
        signal->connections.push_back ( this );
        receiver->d->signal_connections.push_back ( this );
    }

