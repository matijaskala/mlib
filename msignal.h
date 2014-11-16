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

#ifndef MSIGNAL_H
#define MSIGNAL_H

#include <list>
#include <functional>

template< typename... >
class MSignal;

template< typename... Args >
class MSlot
{
    friend class MSignal<Args...>;
    using Signal = MSignal<Args...>;

    std::function<void(Args...)> m_func;

    std::list<Signal*> m_signals;

    void* m_data{nullptr};

    template< class Object >
    struct simple_bind {
        void (Object::*method) (Args...);
        Object* obj;
        simple_bind(void (Object::*method) (Args...), Object* obj) : method{method}, obj{obj} {}
        void operator() (Args... args) { (obj->*method) ( std::forward<Args> (args)... ); }
    };

public:
    template< typename Functor >
    MSlot ( Functor func ) : m_func{func} {}

    template< typename Object >
    MSlot ( void (Object::*method) (Args...), Object* obj ) : m_func{simple_bind<Object>{method,obj}} {}

    ~MSlot();

    template<typename... Args2>
    void operator() ( Args2&&... args ) { m_func ( std::forward<Args2> ( args )... ); }

    void* data() { return m_data; }
};

template< typename... Args >
class MSignal : public MSlot<Args...>
{
    friend class MSlot<Args...>;
    using Slot = MSlot<Args...>;

    std::list<Slot*> m_slots;

public:
    MSignal ( void* data = nullptr );
    ~MSignal();
    void connect ( Slot& slot ) { m_slots.push_back ( &slot ); slot.m_signals.push_front ( this ); }
    void disconnect ( Slot& slot ) { m_slots.remove ( &slot ); slot.m_signals.remove ( this ); }
};

template< typename... Args >
MSignal<Args...>::MSignal ( void* data )
    : Slot {
        [data,this] ( Args&&... args ) {
            for ( Slot* slot: m_slots ) {
                slot->m_data = data;
                (*slot) ( std::forward<Args> ( args )... );
                slot->m_data = nullptr;
            }
        }
    } {}

template< typename... Args >
MSlot<Args...>::~MSlot()
{
    for ( Signal* signal: m_signals )
        signal->m_slots.remove ( this );
}

template< typename... Args >
MSignal<Args...>::~MSignal()
{
    for ( Slot* slot: m_slots )
        slot->m_signals.remove ( this );
}

#endif // MSIGNAL_H
