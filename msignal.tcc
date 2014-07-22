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
    template< typename _Receiver >
    Slot ( _Wrapper __wrapper, Signal< _Args... >* signal, MObject* receiver, void ( _Receiver::* slot_ptr ) ( MObject*, _Args... ) )
        : MObjectSlotBase ( signal, receiver, reinterpret_cast< MObjectSlotBase::_SlotPtr > ( slot_ptr ) )
        , __wrapper ( __wrapper ) {
            signal->_slots.push_back ( this );
        }
    void call ( MObject* sender, _Args... __args ) { __wrapper ( sender, __args... ); }
};

template< typename... _Args >
struct MObject::Signal : public MObjectSignalBase {
    using _Slot = Slot< _Args... >;
    std::list< _Slot* > _slots;
};

