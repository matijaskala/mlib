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
    struct WrapperBase {
        virtual ~WrapperBase() = default;
    };

    template< typename... _Args >
    struct Wrapper : public WrapperBase {
        std::function< void ( MObject*, _Args... ) > __wrapper;
        Wrapper ( MObject* receiver, void ( MObject::* slot ) ( MObject*, _Args... ) )
            : __wrapper ( [slot, receiver] ( MObject* sender, _Args... __args ) {
                (receiver->*slot) ( sender, __args... );
            } ) {}
    };

    struct Connection {
        using Slot = void ( MObject::* ) ( );
        struct Data;
        Data* d;
        WrapperBase* __wrapper();
        template< typename... _Args >
        std::function< void ( MObject*, _Args... ) > wrapper () {
            return static_cast< Wrapper< _Args... >* > ( __wrapper() )->__wrapper;
        }
    };

protected:
    struct SignalBase {
        std::list< Connection* > connections;
        ~SignalBase();
    };

    template< typename... _Args >
    struct Signal : public SignalBase{};

public:
    template< typename _Object, typename... _Args >
    using _Method = void ( _Object::* ) ( _Args... );

    template< typename _Res = MObject, typename _Source, typename... _Args >
    static _Method< _Res, _Args... > method_cast ( void ( _Source::*source ) ( _Args... ) ) {
        return static_cast< _Method< _Res, _Args... > > ( source );
    }

#define classof(object) std::remove_pointer<decltype(object)>::type
#define emit(signal,...) _emit(&signal,__VA_ARGS__)
#define connect(sender,signal,receiver,slot) _connect(&(sender)->signal,receiver,method_cast(&classof(receiver)::slot))
#define disconnect(sender,signal,receiver,slot) _disconnect(&(sender)->signal,receiver,method_cast(&classof(receiver)::slot))

    template< typename... _Args >
    static void _connect ( Signal< _Args... >* signal, MObject* receiver, _Method< MObject, MObject*, _Args... > slot ) {
        receiver->connect_private ( signal, reinterpret_cast< Connection::Slot > ( slot ), new Wrapper< _Args... > ( receiver, slot ) );
    }

    template< typename... _Args >
    static void _disconnect ( Signal< _Args... >* signal, MObject* receiver, _Method< MObject, MObject*, _Args... > slot ) {
        receiver->disconnect_private ( signal, reinterpret_cast< Connection::Slot > ( slot ) );
    }

    template< typename... _Args >
    void _emit ( Signal< _Args... >* signal, _Args... __args ) {
        for ( Connection* connection: signal->connections )
            connection->wrapper< _Args... >() ( this, __args... );
    }

private:
    void connect_private ( SignalBase* signal, Connection::Slot slot, WrapperBase* wrapper );
    void disconnect_private ( SignalBase* signal, Connection::Slot slot );
    static void delete_connection ( Connection* connection );
    class Private;
    class Private* const d;
};

#endif // MOBJECT_H

