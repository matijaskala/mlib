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

#ifndef MREFLECTION_H
#define MREFLECTION_H

#include <string>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include <typenameid>

#define reflected reflect ( this )
#define reflect(...) { MReflection::invoke_pretty ( __PRETTY_FUNCTION__, __VA_ARGS__ ); }
#define reflect_func(FUNC_NAME,FUNC_TYPE) funcs< void FUNC_TYPE >() [FUNC_NAME] = [] FUNC_TYPE

#define M_LAMBDA(...) _Caster<void(__VA_ARGS__)>{}%[](__VA_ARGS__)

class MReflection
{
    template< typename _Var >
    class Map : public std::unordered_map< std::string, _Var > {};
    template< typename _Func >
    class FunctionMap : public Map< std::function< _Func> > {};
    struct Symbol {
        void* ptr;
        template<typename T = void> Symbol ( T* ptr = nullptr )
            : ptr { reinterpret_cast<void*&> ( ptr ) } {}
        template<typename R, typename O, typename... A> Symbol ( R (O::*ptr)(A...) )
            : ptr { reinterpret_cast<void*&> ( ptr ) } {}
        template<typename T> T* as()
            { return reinterpret_cast<T*&> ( ptr ); }
    };
    using SymbolMap = std::unordered_map< std::string, Symbol >;

public:
    template< typename _Instance, typename... _Args >
    static void invoke_pretty ( std::string&& pretty_function_name, _Instance* _This, _Args... __args ) {
        size_t f_begin = pretty_function_name.find(' ');
        size_t b_begin = pretty_function_name.find('(', f_begin);
        size_t r_begin = pretty_function_name.rfind("::", b_begin) + 2;
        _This->invoke ( pretty_function_name.substr( r_begin ), __args... );
    }

    static MReflection* get ( const std::string& name );

    template< typename... _Args >
    void invoke ( const std::string& function_name, _Args... __args ) {
        try {
            symbol<void(_Args...)>(function_name)(__args...);
        } catch(std::bad_function_call) {
            throw std::runtime_error ( name + ": an error occurred while calling " + function_name + "<" + typename_id<void ( _Args... )>{}.name() + ">" );
        }
    }

    template< typename _Class = void >
    _Class* newInstance() {
        return symbol<_Class*()>("new")();
    }

    ~MReflection();

    std::string name;

protected:
    template < typename _Class >
    MReflection ( _Class* ) : MReflection ( typename_id<_Class> ().name() ) {
        m_symbols["new"] = _Caster<_Class*()>{}% [] () { return new _Class; };
    }
    Map< int* > variables;
    template< typename _Func >
    [[gnu::__deprecated__]] FunctionMap < _Func >& funcs() {
        return reinterpret_cast<FunctionMap< _Func >&> (m_pool);
    }

    template< typename T >
    T* symbol ( std::string name ) {
        return m_symbols[name].as<T> ();
    }

    template<typename T> struct _Caster { template<typename L> T* operator% ( L l ) { return l; } };

    SymbolMap m_symbols;

private:
    MReflection ( std::string&& name );
    FunctionMap< void() > m_pool;
};

#endif // MREFLECTION_H
