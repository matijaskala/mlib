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

    std::list<MSignal<Args...>*> m_signals;

    void* m_data;

public:
    template< typename Functor >
    MSlot ( Functor func ) : m_func{func} {}
    ~MSlot();

    void operator() ( Args... args ) { m_func ( args... ); m_data = nullptr; }
};

template< typename... Args >
class MSignal : public MSlot<Args...>
{
    friend class MSlot<Args...>;
    using Slot = MSlot<Args...>;

    std::list<Slot*> m_slots;

public:
    MSignal ( void* data );
    ~MSignal();
    void connect ( Slot& slot ) { m_slots.push_back ( &slot ); slot.m_signals.push_back ( this ); }
    void disconnect ( Slot& slot ) { m_slots.remove ( &slot ); slot.m_signals.remove ( this ); }
};

template< typename... Args >
MSignal<Args...>::MSignal ( void* data )
    : Slot {
        [data,this] ( Args... args ) {
            for ( Slot* slot: m_slots ) {
                slot->m_data = data;
                (*slot) ( args... );
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
