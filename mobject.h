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

private:
    template< typename... _Args >
    class Slot {
        using _Slot = std::function< void ( MObject*, _Args...) >;
        _Slot slot;
        MObject* receiver;
    public:
        Slot ( _Slot slot, MObject* receiver ) : slot ( slot ), receiver ( receiver ) {}
        void call ( MObject* sender, _Args... __args ) { slot ( sender, __args... ); }
    };

protected:
    template< typename... _Args >
    struct Signal {
        using _Slot = Slot< _Args... >;
        std::list< _Slot* > _slots;
    };

public:
#define classof(object) std::remove_pointer<decltype(object)>::type
#define emit _emit
#define connect(sender,signal,receiver,slot) _connect((sender)->signal,receiver,&classof(receiver)::slot)
    template< typename _Receiver, typename... _Args >
    static void _connect ( Signal< _Args... > signal, _Receiver* receiver, void ( _Receiver::*slot ) ( MObject*, _Args... ) ) {
        auto wrapper = [slot, receiver] ( MObject* sender, _Args... __args ) { (receiver->*slot) ( sender, __args... ); };
        signal._slots.push_back ( new Slot< _Args... > ( wrapper, receiver ) );
    }

    template< typename... _Args >
    void _emit ( Signal< _Args... > signal, _Args... __args ) {
        for ( Slot< _Args... >* slot: signal._slots )
            slot->call ( this, __args... );
    }

private:
    class MObjectPrivate* const d;
};

#endif // MOBJECT_H

