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



struct MObject::SlotBase {
    using _SlotPtr = void ( MObject::* ) ( );
    SignalBase* signal;
    MObject* receiver;
    _SlotPtr slot_ptr;
    SlotBase ( SignalBase* signal, MObject* receiver, _SlotPtr slot_ptr );
};

struct MObject::SignalBase {
    std::list< SlotBase* > _slots;
    ~SignalBase();
};

template< typename... _Args >
struct MObject::Signal : public SignalBase {
};

template< typename... _Args >
class MObject::Slot : public SlotBase {
    using _Wrapper = std::function< void ( MObject*, _Args...) >;
    _Wrapper __wrapper;
public:
    Slot ( SignalBase* signal, MObject* receiver, void ( MObject::* slot ) ( MObject*, _Args... ) )
        : SlotBase ( signal, receiver, reinterpret_cast< SlotBase::_SlotPtr > ( slot ) )
        , __wrapper ( [slot, receiver] ( MObject* sender, _Args... __args ) {
            (receiver->*slot) ( sender, __args... );
        } ) {}
    void call ( MObject* sender, _Args... __args ) { __wrapper ( sender, __args... ); }
};

