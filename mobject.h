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
#include <nonstd/signal>
#include <nonstd/traits>
#include <nonstd/tupleid>

#define M_DECL_SIGNAL(...) \
    { MObject::emit_pretty ( __PRETTY_FUNCTION__, __VA_ARGS__ ); }
#define M_OBJECT_CONNECT(sender,signal,receiver,slot) \
    MObject::connect ( sender, signal, receiver, &__classof__(receiver)::slot )
#define M_OBJECT_DISCONNECT(sender,signal,receiver,slot) \
    MObject::disconnect ( sender, signal, receiver, &__classof__(receiver)::slot)
#define NON_STD_SIGNAL_CONNECT(sender,signal,receiver,slot) \
    (sender)->signal.connect ( receiver, &__classof__(receiver)::slot )
#define NON_STD_SIGNAL_DISCONNECT(sender,signal,receiver,slot) \
    (sender)->signal.disconnect ( receiver, &__classof__(receiver)::slot )

class MObject;
extern "C" void m_object_connect ( MObject* sender, const char* signal, MObject* receiver, void (*slot) () );

class M_EXPORT MObject
{
    friend void m_object_connect ( MObject* sender, const char* signal, MObject* receiver, void (*slot) () );
public:
    MObject ( MObject* parent = nullptr );
    virtual ~MObject();
    MObject ( const MObject& other ) = delete;
    MObject& operator= ( const MObject& other ) = delete;

    void reparent ( MObject* parent );
    const std::list<MObject*>& children () const;

    template< typename _Object, typename... _Args >
    static void connect ( MObject* sender, std::string signal_name,
                          _Object* receiver, void (_Object::*slot) (_Args...) ) {
        sender->get_signal<_Args...> (signal_name).connect(receiver, slot);
    }

    template< typename _Object, typename... _Args >
    static void disconnect ( MObject* sender, std::string signal_name,
                             _Object* receiver, void (_Object::*slot) (_Args...) ) {
        sender->get_signal<_Args...> (signal_name).disconnect(receiver, slot);
    }

    template< typename... _Args >
    void emit ( std::string signal_name, _Args&&... args ) {
        get_signal<dearrayize_t<_Args>...> ( signal_name ) ( std::forward<_Args> ( args )... );
    }

    template< typename... _Args >
    void invoke ( std::string slot_name, _Args&&... args ) {
        non_std_slot<_Args...> ( slot_name ) ( std::forward<_Args> ( args )... );
    }

    template< typename... _Args, typename _Object >
    void invoke ( void (_Object::*method) (_Args...),
                  std::conditional_t<0,void,_Args&&>... args ) {
        non_std_slot<_Args...> ( method ) ( std::forward<_Args> ( args )... );
    }

    template< typename... _Args >
    non_std::signal<_Args...>& get_signal ( std::string signal_name ) {
        auto& s = access_signal ( signal_name );
        if ( !s ) {
            auto* a = new non_std::signal<_Args...> ( this );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
        return *static_cast<non_std::signal<_Args...>*> ( s );
    }

    template< typename... _Args >
    non_std::slot<_Args...>& non_std_slot ( std::string slot_name ) {
        auto* s = access_slot ( slot_name, static_typeid<_Args...> () );
        if ( !s ) {
            auto& sig = get_signal<_Args...> ( slot_name );
            auto* a = new non_std::slot<_Args...> ( [&sig] (_Args... args) { sig(std::forward<_Args>(args)...); } );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
        return *static_cast<non_std::slot<_Args...>*> ( s );
    }

    template< typename... _Args, typename _Object >
    auto& non_std_slot ( void (_Object::*method) (_Args...) ) {
        auto& s = access_slot ( method );
        if ( !s ) {
            auto* self = dynamic_cast<_Object*> ( this );
            auto* a = new non_std::slot<_Args...> ( method, self );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
        return *static_cast<non_std::slot<_Args...>*> ( s );
    }

protected:
    template< typename... _Args >
    void emit_pretty ( std::string&& pretty_signal_name, _Args&&... args ) {
        size_t fun_begin = pretty_signal_name.find ( ' ' );
        size_t arg_begin = pretty_signal_name.find ( '(', fun_begin );
        size_t rev_begin = pretty_signal_name.rfind ( "::", arg_begin ) + 2;
        auto signal_name = pretty_signal_name.substr ( rev_begin );
        emit ( std::move ( signal_name ), std::forward<_Args> ( args )... );
    }

    template< typename _Object, typename... _Args >
    void make_slot ( std::string slot, void (_Object::*func) (_Args...) ) {
        auto*& s = access_slot ( slot, static_typeid<_Args...> () );
        auto* self = dynamic_cast<_Object*> ( this );
        if ( s ) {
            static_cast<non_std::slot<_Args...>*> ( s )->~slot();
            ::new ( s ) non_std::slot<_Args...> ( func, self );
        }
        else {
            auto* a = new non_std::slot<_Args...> ( func, self );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
    }

    template< typename... _Args >
    void make_slot ( std::string slot, std::function<void(_Args...)> func ) {
        auto*& s = access_slot ( slot, static_typeid<_Args...> () );
        if ( s ) {
            static_cast<non_std::slot<_Args...>*> ( s )->~slot();
            ::new ( s ) non_std::slot<_Args...> ( func );
        }
        else {
            auto* a = new non_std::slot<_Args...> ( func );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
    }

    template< typename _Object, typename... _Args >
    void make_slot ( void (_Object::*slot) (), void (_Object::*func) (_Args...) ) {
        auto*& s = access_slot ( slot );
        auto* self = dynamic_cast<_Object*> ( this );
        if ( s ) {
            static_cast<non_std::slot<_Args...>*> ( s )->~slot();
            ::new ( s ) non_std::slot<_Args...> ( func, self );
        }
        else {
            auto* a = new non_std::slot<_Args...> ( func, self );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
    }

    template< typename _Object, typename... _Args >
    void make_slot ( void (_Object::*slot) (_Args...), std::function<void(_Args...)> func ) {
        auto*& s = access_slot ( slot );
        if ( s ) {
            static_cast<non_std::slot<_Args...>*> ( s )->~slot();
            ::new ( s ) non_std::slot<_Args...> ( func );
        }
        else {
            auto* a = new non_std::slot<_Args...> ( func );
            s = a;
            push_destructor ( [a] { delete a; } );
        }
    }

private:
    void*& access_signal ( std::string signal_name );
    void*& access_slot ( std::string slot_name, non_std::tuple_index arg_types );
    void*& access_slot ( void (MObject::*method) () );
    template< typename _Object, typename... _Args >
    void*& access_slot ( void (_Object::*method) ( _Args... ) ) {
        return access_slot ( reinterpret_cast<void (MObject::*) ()> (method) );
    }

    void push_destructor ( std::function<void()> func );

    template< typename _Tp >
    using dearrayize_t = std::conditional_t<std::is_array<
                         std::remove_reference_t<_Tp>>{},
                         std::remove_extent_t<
                         std::remove_reference_t<_Tp>>*, _Tp>;

    struct Private;
    Private* const d;

};

#endif // MOBJECT_H
