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

#include "melapsedtimer.h"

#include <chrono>

using namespace std::chrono;

struct MElapsedTimerPrivate {
    system_clock::time_point start;
};

MElapsedTimer::MElapsedTimer()
    : d ( new MElapsedTimerPrivate )
{
    reset();
}

MElapsedTimer::~MElapsedTimer()
{
    delete d;
}

std::int64_t MElapsedTimer::elapsed()
{
    system_clock::time_point now = system_clock::now();
    return duration_cast<milliseconds> ( now - d->start ).count();
}

int64_t MElapsedTimer::elapsedus()
{
    system_clock::time_point now = system_clock::now();
    return duration_cast<microseconds> ( now - d->start ).count();
}

void MElapsedTimer::reset()
{
    d->start = system_clock::now();
}
