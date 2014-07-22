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

#ifndef MOBJECT_H
#define MOBJECT_H

#include <list>
#include <functional>

class MObject
{
public:
    MObject ( MObject* parent = nullptr );
    virtual ~MObject();
    MObject ( const MObject& other ) = delete;

    void reparent ( MObject* parent );
    const std::list<MObject*>& children ();

protected:
    template< typename... _Args >
    struct Signal;

private:
    template< typename... _Args >
    class Slot;

public:
#define classof(object) std::remove_pointer<decltype(object)>::type
#define emit _emit
#define connect(sender,signal,receiver,slot) _connect(&(sender)->signal,receiver,&classof(receiver)::slot)
#define disconnect(sender,signal,receiver,slot) _disconnect(&(sender)->signal,receiver,&classof(receiver)::slot)
    template< typename _Receiver, typename... _Args >
    static void _connect ( Signal< _Args... >* signal, MObject* receiver, void ( _Receiver::*slot ) ( MObject*, _Args... ) ) {
        new Slot< _Args... > ( signal, receiver, static_cast< void (MObject::*)(MObject*, _Args...) > ( slot ) );
    }

    template< typename _Receiver, typename... _Args >
    static void _disconnect ( Signal< _Args... >* signal, MObject* receiver, void ( _Receiver::*slot ) ( MObject*, _Args... ) ) {
        for ( Slot< _Args... >* _slot: signal->_slots )
            if ( _slot.receiver == receiver && _slot.slot_ptr == slot )
                signal->_slot.remove ( _slot );
    }

    template< typename... _Args >
    void _emit ( Signal< _Args... > signal, _Args... __args ) {
        for ( Slot< _Args... >* slot: signal._slots )
            slot->call ( this, __args... );
    }

private:
    class MObjectPrivate* const d;
};

#include "msignal.tcc"

#endif // MOBJECT_H

