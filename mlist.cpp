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

#include "mlist.h"

#include <cstring>

static void* mmalloc ( std::size_t size ) {
    void* p = std::malloc(size);
    if ( !p ) {
        std::new_handler handler = std::get_new_handler();
        if ( !handler )
            throw std::bad_alloc{};
        handler();
    }
    return p;
}

static void* mrealloc ( void* ptr, std::size_t size ) {
    void* p = std::realloc(ptr, size);
    if ( !p ) {
        std::new_handler handler = std::get_new_handler();
        if ( !handler )
            throw std::bad_alloc{};
        handler();
    }
    return p;
}

struct MListDataPrivate
{
    std::size_t m_begin = 0;
    std::size_t m_end = 0;
    std::size_t m_capacity = 0;
    volatile std::size_t m_refcount = 1;
    void* m_ptr = nullptr;
};

MListData::MListData ( std::size_t n ) : d{new MListDataPrivate}
{
    if (n) {
        d->m_capacity = n;
        d->m_ptr = mmalloc(n * sizeof(void*));
    }
}

MListData::MListData ( MListData&& other ) : d{new MListDataPrivate}
{
    std::swap(*d, *other.d);
}

MListData::~MListData()
{
    if ( !--d->m_refcount ) {
        if ( d->m_ptr )
            free(d->m_ptr);
        delete d;
    }
}

MListData& MListData::operator= ( MListData&& other )
{
    std::swap(*d, *other.d);
    return *this;
}

MListData& MListData::ref()
{
    d->m_refcount++;
    return *this;
}

std::size_t MListData::refcount()
{
    return d->m_refcount;
}

std::size_t MListData::size() const
{
    return d->m_end - d->m_begin;
}

std::size_t MListData::capacity() const
{
    return d->m_capacity;
}

void MListData::erase ( std::size_t pos, std::size_t n )
{
    if ( pos + n == size() ) {
        d->m_end -= n;
        return;
    }
    if ( pos == 0 ) {
        d->m_begin += n;
        return;
    }
    if ( 2 * pos + n < size() ) {
        std::memmove(static_cast<void**>(d->m_ptr) + d->m_begin + n,
                     static_cast<void**>(d->m_ptr) + d->m_begin, pos * sizeof(void*));
        d->m_begin += n;
    }
    else {
        std::memmove(static_cast<void**>(d->m_ptr) + d->m_begin + pos,
                     static_cast<void**>(d->m_ptr) + d->m_begin + pos + n, (size() - pos - n) * sizeof(void*));
        d->m_end -= n;
    }
}

void MListData::reserve ( std::size_t n )
{
    if ( n == capacity() )
        return;
    if ( n < size() )
        n = size();
    if ( !n ) {
        free(d->m_ptr);
        d->m_ptr = nullptr;
        d->m_begin = d->m_end = 0;
    }
    else {
        if ( n < d->m_end ) {
            d->m_end -= d->m_begin;
            std::memmove(d->m_ptr, static_cast<void**>(d->m_ptr) + d->m_begin, d->m_end * sizeof(void*));
            d->m_begin = 0;
        }
        d->m_ptr = mrealloc(d->m_ptr, n * sizeof(void*));
    }
    d->m_capacity = n;
}

void MListData::reserve_front ( std::size_t n )
{
    if ( n <= d->m_begin )
        return;
    n -= d->m_begin;
    if ( n + d->m_end <= d->m_capacity ) {
        std::memmove(static_cast<void**>(d->m_ptr) + d->m_begin + n,
                     static_cast<void**>(d->m_ptr) + d->m_begin, size() * sizeof(void*));
        d->m_begin += n;
        d->m_end += n;
    }
    else {
        d->m_capacity = n + d->m_end;
        auto p = mmalloc(d->m_capacity * sizeof(void*));
        std::memcpy(static_cast<void**>(p) + d->m_begin + n,
                    static_cast<void**>(d->m_ptr) + d->m_begin, size() * sizeof(void*));
        std::free(d->m_ptr);
        d->m_begin += n;
        d->m_end += n;
        d->m_ptr = p;
    }
}

void MListData::reserve_back ( std::size_t n )
{
    if ( n <= d->m_capacity - d->m_end )
        return;
    n -= d->m_capacity - d->m_end;
    if ( n <= d->m_begin ) {
        std::memmove(static_cast<void**>(d->m_ptr) + d->m_begin - n,
                     static_cast<void**>(d->m_ptr) + d->m_begin, size() * sizeof(void*));
        d->m_begin -= n;
        d->m_end -= n;
    }
    else {
        d->m_capacity += n - d->m_begin;
        if ( d->m_begin ) {
            auto p = mmalloc(d->m_capacity * sizeof(void*));
            d->m_end -= d->m_begin;
            std::memcpy(p, static_cast<void**>(d->m_ptr) + d->m_begin, d->m_end * sizeof(void*));
            std::free(d->m_ptr);
            d->m_begin = 0;
            d->m_ptr = p;
        }
        else
            d->m_ptr = mrealloc(d->m_ptr, d->m_capacity * sizeof(void*));
    }
}

void MListData::reverse()
{
    for ( std::size_t i = 0; i < size()/2; i++ )
        std::swap(begin()[i], end()[-i]);
}

void MListData::push_front ( void* p )
{
    if ( d->m_begin )
        reserve_front(size() * 0.2 + 1);
    static_cast<void**>(d->m_ptr)[--d->m_begin] = p;
}

void MListData::push_back ( void* p )
{
    if ( d->m_end == d->m_capacity )
        reserve_back(size() * 0.2 + 1);
    static_cast<void**>(d->m_ptr)[d->m_end++] = p;
}

MListData::iterator MListData::begin()
{
    return static_cast<void**>(d->m_ptr) + d->m_begin;
}

MListData::iterator MListData::end()
{
    return static_cast<void**>(d->m_ptr) + d->m_end;
}

MListData::const_iterator MListData::begin() const
{
    return static_cast<void**>(d->m_ptr) + d->m_begin;
}

MListData::const_iterator MListData::end() const
{
    return static_cast<void**>(d->m_ptr) + d->m_end;
}
