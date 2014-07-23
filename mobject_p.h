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

#ifndef MOBJECTPRIVATE_H
#define MOBJECTPRIVATE_H

#include "mobject.h"

class MObject::Private
{
public:
    Private ( MObject* q );
    virtual ~Private();

    MObject* parent;
    std::list<MObject*> children;
    std::list<MObject::Connection*> signal_connections;

private:
    class MObject* const q;
};

struct MObject::Connection::Data {
    SignalBase* signal;
    MObject* receiver;
    Slot slot;
    WrapperBase* wrapper;
};

#endif // MOBJECTPRIVATE_H

