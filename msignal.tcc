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



template< typename... _Args >
void MObject::_disconnect ( Signal< _Args... >* signal, MObject* receiver, _Method< MObject, MObject*, _Args... > slot ) {
    for ( auto i = signal->_slots.begin(); i != signal->_slots.end(); i++ )
        if ( (*i)->receiver == receiver && reinterpret_cast< _Method< MObject, MObject*, _Args... > > ( (*i)->slot_ptr ) == slot )
            signal->_slots.remove ( *i-- );
    receiver->disconnect_private ( signal, reinterpret_cast< _Method< MObject > > ( slot ) );
}

struct MObjectSignalBase {};

struct MObjectSlotBase {
    using _SlotPtr = void ( MObject::* ) ( );
    MObjectSignalBase* signal;
    MObject* receiver;
    _SlotPtr slot_ptr;
    MObjectSlotBase ( MObjectSignalBase* signal, MObject* receiver, _SlotPtr slot_ptr )
        : signal ( signal )
        , receiver ( receiver )
        , slot_ptr ( slot_ptr ) {}
};

template< typename... _Args >
class MObject::Slot : public MObjectSlotBase {
    using _Wrapper = std::function< void ( MObject*, _Args...) >;
    _Wrapper __wrapper;
public:
    Slot ( Signal< _Args... >* signal, MObject* receiver, void ( MObject::* slot ) ( MObject*, _Args... ) )
        : MObjectSlotBase ( signal, receiver, reinterpret_cast< MObjectSlotBase::_SlotPtr > ( slot ) )
        , __wrapper ( [slot, receiver] ( MObject* sender, _Args... __args ) { (receiver->*slot) ( sender, __args... ); } ) {
            signal->_slots.push_back ( this );
        }
    void call ( MObject* sender, _Args... __args ) { __wrapper ( sender, __args... ); }
};

template< typename... _Args >
struct MObject::Signal : public MObjectSignalBase {
    using _Slot = Slot< _Args... >;
    std::list< _Slot* > _slots;
};

