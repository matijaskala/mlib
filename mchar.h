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

#ifndef MCHAR_H
#define MCHAR_H

#include <istream>

typedef struct {
    typedef char16_t value_type;
    value_type value;
    bool operator== ( value_type __c ) {
        return value == __c;
    }
    bool operator!= ( value_type __c ) {
        return value != __c;
    }
    bool operator< ( value_type __c ) {
        return value < __c;
    }
    bool operator> ( value_type __c ) {
        return value > __c;
    }
    bool operator<= ( value_type __c ) {
        return value <= __c;
    }
    bool operator>= ( value_type __c ) {
        return value >= __c;
    }
} mchar_t;

inline bool operator< ( mchar_t __mc1, mchar_t __mc2 )
{
    return __mc1.value < __mc1.value;
}

inline bool operator> ( mchar_t __mc1, mchar_t __mc2 )
{
    return __mc1.value > __mc2.value;
}

inline bool operator<= ( mchar_t __mc1, mchar_t __mc2 )
{
    return __mc1.value <= __mc1.value;
}

inline bool operator>= ( mchar_t __mc1, mchar_t __mc2 )
{
    return __mc1.value >= __mc2.value;
}

inline void __mchar_helper(mchar_t& __mc, std::function< void ( char& ) > __a) {
    char* __ptr = reinterpret_cast<char*> ( &__mc.value );
    __a ( __ptr[0] );
    if ( __ptr[0] & 0x80 )
        __a ( __ptr[1] );
}

inline mchar_t getmchar()
{
    mchar_t __mc;
    __mc.value = 0;
    auto __f = [] ( char& __c ) { __c = std::getchar(); };
    __mchar_helper ( __mc, __f );
    return __mc;
}

inline mchar_t putmchar ( mchar_t __mc )
{
    mchar_t::value_type __c = __mc.value;
    do {
        if ( std::putchar ( __c ) == std::char_traits< char >::eof() )
            return { std::char_traits< mchar_t::value_type >::eof() };
        __c >>= 8;
    } while ( __c );
    return __mc;
}

template<typename _CharT, typename _Traits>
inline std::basic_istream<_CharT, _Traits>&
operator>> ( std::basic_istream<_CharT, _Traits>& __is, mchar_t& __mc )
{
    __mc.value = 0;
    auto __f = [&__is] ( _CharT&__c ) { __is >> __c; };
    __mchar_helper ( __mc, __f );
    return __is;
}

template<typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, mchar_t __mc) {
    auto __f = [&__os] ( _CharT& __c ) { __os << __c; };
    __mchar_helper ( __mc, __f );
    return __os;
}

template<typename _CharT, typename _Traits>
inline std::basic_string<_CharT, _Traits>&
operator+=(std::basic_string<_CharT, _Traits>& __s, mchar_t __mc) {
    auto __f = [&__s] ( _CharT& __c ) { __s += __c; };
    __mchar_helper ( __mc, __f );
    return __s;
}

template<typename _CharT, typename _Traits>
inline std::basic_string<_CharT, _Traits>
operator+(std::basic_string<_CharT, _Traits> __s, const mchar_t& __c) {
    __s += __c;
    return __s;
}

#endif // MCHAR_H
