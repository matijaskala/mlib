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

#include <forward_list>
#include <unordered_map>

namespace std {
template<>
struct hash<pair<string, non_std::tuple_index>> {
    auto operator() ( const pair<string, non_std::tuple_index>& a ) const noexcept {
        return hash<string>{}(a.first) + hash<non_std::tuple_index>{}(a.second);
    }
};
}

struct MObject::Private {
    MObject* parent = nullptr;
    std::list<MObject*> children{};
    std::unordered_map<std::string, void*> signals{};
    std::unordered_map<std::pair<std::string, non_std::tuple_index>, void*> named_slots{};
    std::unordered_map<void*, void*> method_slots{};
    std::forward_list<std::function<void()>> destructors{};
};

MObject::MObject ( MObject* parent )
    : d{new Private}
{
    reparent ( parent );
}

MObject::~MObject()
{
    for ( auto&& func: d->destructors )
        func();
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

const std::list<MObject*>& MObject::children () const
{
    return d->children;
}

void*& MObject::access_signal ( std::string signal_name )
{
    return d->signals[signal_name];
}

void*& MObject::access_slot ( std::string slot_name, non_std::tuple_index arg_types )
{
    return d->named_slots[{slot_name, arg_types}];
}

void*& MObject::access_slot ( void (MObject::*method) () )
{
    return d->method_slots[(void*&)method];
}

void MObject::push_destructor ( std::function<void()> func )
{
    d->destructors.push_front ( func );
}

extern "C" {
void m_object_connect ( MObject* sender, const char* signal, MObject* receiver, void (*slot) () )
{
#if defined __x86_64__ && defined __ILP32__
#define register_t long long
#else
#define register_t long
#endif
#define all_args \
            register_t r1, register_t r2, register_t r3, register_t r4, \
            register_t r5, register_t r6, register_t r7, register_t r8, \
            double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8
    void (MObject::*s) (all_args) = reinterpret_cast<void (MObject::*&) (all_args)> ( slot );
    if ( !receiver->access_slot ( s ) )
        receiver->make_slot ( s, static_cast<std::function<void(all_args)>> (
            [receiver,slot] ( all_args ) {
                auto s = reinterpret_cast<void(*)(all_args)> ( slot );
                s ( r1,r2,r3,r4,r5,r6,r7,r8,f1,f2,f3,f4,f5,f6,f7,f8 );
            }
        ) );
    MObject::connect ( sender, signal, receiver, s );
}

void m_object_disconnect ( MObject* sender, const char* signal, MObject* receiver, void (*slot) () )
{
    void (MObject::*s) (all_args) = reinterpret_cast<void (MObject::*&) (all_args)> ( slot );
    MObject::disconnect ( sender, signal, receiver, s );
}

void m_object_emit ( MObject* sender, const char* signal, all_args ) {
#undef register_t
#undef all_args
    sender->emit ( signal, r1,r2,r3,r4,r5,r6,r7,r8,f1,f2,f3,f4,f5,f6,f7,f8 );
}
}
