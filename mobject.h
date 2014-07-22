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
    template< typename _Object, typename... _Args >
    using _Method = void ( _Object::* ) ( _Args... );

    template< typename _Res = MObject, typename _Source, typename... _Args >
    static _Method< _Res, _Args... > method_cast ( void ( _Source::*source ) ( _Args... ) ) {
        return static_cast< _Method< _Res, _Args... > > ( source );
    }

#define classof(object) std::remove_pointer<decltype(object)>::type
#define emit _emit
#define connect(sender,signal,receiver,slot) _connect(&(sender)->signal,receiver,method_cast(&classof(receiver)::slot))
#define disconnect(sender,signal,receiver,slot) _disconnect(&(sender)->signal,receiver,method_cast(&classof(receiver)::slot))

    template< typename... _Args >
    static void _connect ( Signal< _Args... >* signal, MObject* receiver, _Method< MObject, MObject*, _Args... > slot ) {
        new Slot< _Args... > ( signal, receiver, slot );
    }

    template< typename... _Args >
    static void _disconnect ( Signal< _Args... >* signal, MObject* receiver, _Method< MObject, MObject*, _Args... > slot );

    template< typename... _Args >
    void _emit ( Signal< _Args... > signal, _Args... __args ) {
        for ( Slot< _Args... >* slot: signal._slots )
            slot->call ( this, __args... );
    }

private:
    void disconnect_private ( class MObjectSignalBase* signal, _Method<MObject> slot );
    class MObjectPrivate* const d;
};

#include "msignal.tcc"

#endif // MOBJECT_H

