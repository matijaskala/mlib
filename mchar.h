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

inline mchar_t getmchar()
{
    mchar_t __mc;
    __mc.value = std::getchar();
    if ( __mc.value & 0x80 )
        __mc.value |= std::getchar() << 8;
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
    _CharT* input = reinterpret_cast<_CharT*> ( &__mc.value );
    __is >> input[0];
    if ( __mc.value & 0x80 )
        __is >> input[1];
    return __is;
}

template<typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, mchar_t __mc) {
    _CharT* output = reinterpret_cast<_CharT*> ( &__mc.value );
    return std::__ostream_insert ( __os, output, __mc.value & 0x80 ? 2 : 1 );
}

#endif // MCHAR_H
