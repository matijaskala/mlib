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
#include <unordered_map>
#include <stdexcept>

#define reflected reflect ( this )
#define reflect(...) { MReflection::invoke_pretty ( __PRETTY_FUNCTION__, __VA_ARGS__ ); }

#define M_REFLECT_METHOD(CLASS,METHOD,ARGS) m_symbols[#METHOD #ARGS] = \
        static_cast<void(CLASS::*)ARGS>(&CLASS::METHOD)
#define M_REFLECT_FIELD(CLASS,METHOD) m_symbols[#METHOD] = &CLASS::METHOD

#define M_REFLECTION \
public: \
    static MReflection* getClass() { \
        std::string pretty_function_name = __PRETTY_FUNCTION__; \
        std::size_t a = pretty_function_name.rfind(' '); \
        std::size_t b = pretty_function_name.rfind("::"); \
        return MReflection::get ( pretty_function_name.substr ( a + 1, b - a - 1 ) ); \
    } \
    template<typename T> \
    T& access ( const std::string& field_name ) { \
        return getClass()->access<T>(this, field_name); \
    } \
    template< typename... _Args > \
    void invoke ( const std::string& function_name, _Args... __args ) { \
        getClass()->invoke(function_name, this, __args...); \
    } \
private:

class MReflection
{
    class Symbol {
        void* ptr;
    public:
        Symbol() : ptr{nullptr} {}
        template<typename T> Symbol ( T* ptr )
            : ptr { reinterpret_cast<void*&> ( ptr ) } {}
        template<typename O, typename T> Symbol ( T O::*ptr )
            : ptr { reinterpret_cast<void*&> ( ptr ) } {}
        template<typename R, typename O, typename... A> Symbol ( R (O::*ptr)(A...) )
            : ptr { reinterpret_cast<void*&> ( ptr ) } {}
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
            auto sym = symbol<void(_Args...)>(function_name);
            if ( sym )
                sym(__args...);
        } catch(std::exception) {
            throw std::runtime_error ( name + ": an error occurred while calling " + function_name );
        }
    }

    template< typename T, typename O >
    T& access ( O* obj, std::string field_name ) {
        return obj->*symbol<O,T> ( field_name );
    }

    void* createInstance() {
        auto create = symbol<void*()>("create");
        return create();
    }

    void destroyInstance(void* p) {
        auto destroy = symbol<void(void*)>("destroy");
        destroy(p);
    }

    std::string name;

    SymbolMap m_symbols;

private:
    template< typename T >
    T* symbol ( std::string name ) {
        return reinterpret_cast<T*&>(m_symbols[name]);
    }

    template< typename O, typename T >
    T O::* symbol ( std::string name ) {
        return reinterpret_cast<T O::*&>(m_symbols[name]);
    }
};

#endif // MREFLECTION_H
