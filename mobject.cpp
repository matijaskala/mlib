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
struct hash<pair<string, tuple_index>> {
    auto operator() ( const pair<string, tuple_index>& a ) const noexcept {
        return hash<string>{}(a.first) + a.second.hash_code();
    }
};
}

struct MObject::Private {
    MObject* parent = nullptr;
    std::list<MObject*> children{};
    std::unordered_map<std::string, void*> signals{};
    std::unordered_map<std::pair<std::string, tuple_index>, void*> named_slots{};
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

void*& MObject::access_slot ( std::string slot_name, tuple_index arg_types )
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
