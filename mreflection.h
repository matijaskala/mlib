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

#include <mglobal.h>
#include <string>
#include <unordered_map>
#include <stdexcept>

#define reflect(...) { getClass()->invoke_pretty ( __PRETTY_FUNCTION__, __VA_ARGS__ ); }

#define M_REFLECTION_HELPER_new(CLASS) static_cast<CLASS*(*)()>( [] { return new CLASS; } )
#define M_REFLECTION_HELPER_delete(CLASS) static_cast<void(*)(CLASS*)>( [] (CLASS* obj) { delete obj; } )

#define M_REFLECT_HELPER(CLASS,HELPER) m_symbols[#HELPER] = \
        M_REFLECTION_HELPER_##HELPER(CLASS)
#define M_REFLECT_METHOD(CLASS,METHOD,ARGS) m_symbols[#METHOD #ARGS] = \
        static_cast<void(CLASS::*)ARGS>(&CLASS::METHOD)
#define M_REFLECT_STATIC(CLASS,METHOD,ARGS) m_symbols[#METHOD #ARGS] = \
        static_cast<void(*)ARGS>(&CLASS::METHOD)
#define M_REFLECT_FIELD(CLASS,METHOD) m_symbols[#METHOD] = &CLASS::METHOD

#define M_REFLECTION \
public: \
    static std::string className() { \
        std::string pretty_function_name = __PRETTY_FUNCTION__; \
        std::size_t a = pretty_function_name.rfind(' '); \
        std::size_t b = pretty_function_name.rfind("::"); \
        return pretty_function_name.substr ( a + 1, b - a - 1 ); \
    } \
    static MReflection* getClass() { \
        return MReflection::get ( className() ); \
    } \
    template<typename T> \
    T& access ( const std::string& sym ) { \
        return getClass()->access<T>(this, sym); \
    } \
    template< typename... _Args > \
    void invoke ( const std::string& sym, _Args... __args ) { \
        getClass()->invoke(this, sym, __args...); \
    } \
    template< typename T > \
    T getField ( const std::string& sym ) { \
        return getClass()->getField<T>(this, sym); \
    } \
    template< typename... _Args > \
    void setField ( const std::string& sym, _Args... __args ) { \
        getClass()->setField(this, sym, __args...); \
    } \
private:

class M_EXPORT MReflection
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
    template< typename... _Args >
    void invoke_pretty ( std::string&& pretty_function_name, _Args... __args ) {
        std::size_t f_begin = pretty_function_name.find(' ');
        std::size_t b_begin = pretty_function_name.find('(', f_begin);
        std::size_t r_begin = pretty_function_name.rfind("::", b_begin) + 2;
        invoke_static ( pretty_function_name.substr( r_begin ), __args... );
    }

    static MReflection* get ( const std::string& name );

    template< typename R = void, typename... _Args >
    R invoke ( void* obj, const std::string& function_name, _Args... __args ) const {
        return invoke_static<R>(function_name, obj, __args...);
    }

    template< typename R = void, typename... _Args >
    R invoke_static ( const std::string& function_name, _Args... __args ) const {
        try {
            auto i = m_symbols.find(function_name);
            if (i == m_symbols.end())
                return (R)nullptr;
            return reinterpret_cast<R (* const&) (_Args...)> ( i->second ) (__args...);
        } catch(std::exception) {
            throw std::runtime_error ( "An error occurred while calling " + function_name );
        }
    }

    void* newInstance() const {
        return invoke_static<void*>("new");
    }

    void deleteInstance(void* p) const {
        invoke(p, "delete");
    }

    template< typename T >
    T getField ( void* obj, std::string field_name ) const {
        auto i = m_symbols.find(field_name);
        if (i == m_symbols.end())
            return T{};
        class _Obj{};
        return static_cast<_Obj*>(obj)->*reinterpret_cast<T _Obj::* const&> ( i->second );
    }

    template< typename T >
    void setField ( void* obj, std::string field_name, T val ) const {
        auto i = m_symbols.find(field_name);
        if (i == m_symbols.end())
            return;
        class _Obj{};
        static_cast<_Obj*>(obj)->*reinterpret_cast<T _Obj::* const&> ( i->second ) = val;
    }

    SymbolMap m_symbols;
};

#endif // MREFLECTION_H
