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

#ifndef MLIST_H
#define MLIST_H

#include <mglobal.h>
#include <iterator>

class M_EXPORT MListData
{
public:
    using iterator = void**;
    using const_iterator = void const* const*;

    explicit MListData ( std::size_t n );
    MListData ( const MListData& ) = delete;
    MListData ( MListData&& other );
    ~MListData();
    MListData& operator= ( const MListData& ) = delete;
    MListData& operator= ( MListData&& other );
    MListData& ref();
    std::size_t refcount();
    std::size_t size() const;
    std::size_t capacity() const;
    void erase ( std::size_t pos, std::size_t n );
    void reserve ( std::size_t n );
    void reserve_front ( std::size_t n );
    void reserve_back ( std::size_t n );
    void reverse();
    void push_front ( void* p );
    void push_back ( void* p );
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    struct MListDataPrivate* const d;
};

template <typename _Tp, typename _ListData = MListData>
class MList
{
    struct Node {
        _Tp x;
    };
    template <typename _List>
    using require_compatible_list = typename std::enable_if_t<std::is_convertible<
                                    typename std::conditional_t<std::is_reference<_List>{},
                                                 decltype(          *std::declval<_List>().begin()),
                                                 decltype(std::move(*std::declval<_List>().begin()))>, _Tp>{}>;

public:
    class iterator {
        typename _ListData::iterator i;
        friend MList;
        iterator ( typename _ListData::iterator&& i ) : i{i} {}
    public:
        using iterator_category = typename std::iterator_traits<decltype(i)>::iterator_category;
        using value_type = _Tp;
        using difference_type = typename std::iterator_traits<decltype(i)>::difference_type;
        using pointer = typename std::add_pointer_t<value_type>;
        using reference = typename std::add_lvalue_reference_t<value_type>;

        bool operator== ( const iterator& other ) const { return i == other.i; }
        bool operator!= ( const iterator& other ) const { return i != other.i; }
        bool operator<  ( const iterator& other ) const { return i <  other.i; }
        bool operator<= ( const iterator& other ) const { return i <= other.i; }
        bool operator>  ( const iterator& other ) const { return i >  other.i; }
        bool operator>= ( const iterator& other ) const { return i >= other.i; }
        difference_type operator- ( const iterator& other ) const { return std::distance(other.i, i); }
        reference operator* () const { return static_cast<Node*>(*i)->x; }
        pointer operator-> () const { return std::addressof(**this); }
        iterator& operator++ () { ++i; return *this; }
        iterator operator++ (int) { iterator ret = *this; i++; return ret; }
        iterator& operator-- () { --i; return *this; }
        iterator operator-- (int) { iterator ret = *this; i--; return ret; }
        iterator& operator+= ( difference_type n ) { std::advance(i, +n); return *this; }
        iterator operator+ ( difference_type n ) const { iterator ret = *this; return ret += n; }
        iterator& operator-= ( difference_type n ) { std::advance(i, -n); return *this; }
        iterator operator- ( difference_type n ) const { iterator ret = *this; return ret -= n; }
    };

    class const_iterator {
        typename _ListData::const_iterator i;
        friend MList;
        const_iterator ( typename _ListData::const_iterator&& i ) : i{i} {}
    public:
        using iterator_category = typename std::iterator_traits<decltype(i)>::iterator_category;
        using value_type = const _Tp;
        using difference_type = typename std::iterator_traits<decltype(i)>::difference_type;
        using pointer = typename std::add_pointer_t<value_type>;
        using reference = typename std::add_lvalue_reference_t<value_type>;

        bool operator== ( const const_iterator& other ) const { return i == other.i; }
        bool operator!= ( const const_iterator& other ) const { return i != other.i; }
        bool operator<  ( const const_iterator& other ) const { return i <  other.i; }
        bool operator<= ( const const_iterator& other ) const { return i <= other.i; }
        bool operator>  ( const const_iterator& other ) const { return i >  other.i; }
        bool operator>= ( const const_iterator& other ) const { return i >= other.i; }
        difference_type operator- ( const const_iterator& other ) const { return std::distance(other.i, i); }
        reference operator* () const { return static_cast<const Node*>(*i)->x; }
        pointer operator-> () const { return std::addressof(**this); }
        const_iterator& operator++ () { ++i; return *this; }
        const_iterator  operator++ (int) { const_iterator ret = *this; i++; return ret; }
        const_iterator& operator-- () { --i; return *this; }
        const_iterator  operator-- (int) { const_iterator ret = *this; i--; return ret; }
        const_iterator& operator+= ( difference_type n ) { std::advance(i, +n); return *this; }
        const_iterator  operator+  ( difference_type n ) const { const_iterator ret =  *this; return ret += n; }
        const_iterator& operator-= ( difference_type n ) { std::advance(i, -n); return *this; }
        const_iterator  operator-  ( difference_type n ) const { const_iterator ret =  *this; return ret -= n; }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    MList ( ) : m_data{0} {}
    MList ( const MList& other ) : m_data{other.size()} { append(other); }
    MList ( MList&& other ) = default;
    template <typename _List, class = require_compatible_list<_List>>
    MList ( _List&& other ) : m_data{other.size()} { append(std::forward<_List>(other)); }
    template <typename _Tp0>
    MList ( const std::initializer_list<_Tp0>& other ) : m_data{other.size()} { append(std::move(other)); }
    template <typename _Tp0>
    MList ( std::initializer_list<_Tp0>&& other ) : m_data{other.size()} { append(std::move(other)); }
    ~MList () {
        for ( auto&& i: m_data )
            delete static_cast<Node*>(i);
    }
    MList& operator= ( const MList& other ) {
        if ( this == &other || *this == other )
            return *this;
        clear();
        return *this << other;
    }
    MList& operator= ( MList&& other ) = default;
    template <typename _List>
    MList& operator= ( _List&& other ) {
        if ( *this == other )
            return *this;
        clear();
        return *this << std::forward<_List>(other);
    }
    template <typename _List>
    bool operator!= ( const _List& other ) const {
        static_assert(std::is_same<decltype(other.begin()), decltype(other.end())>{}, "inconsistent begin/end types");
        for ( auto i = std::pair<const_iterator, decltype(other.begin())>{this->begin(), other.begin()},
                 end = std::pair<const_iterator, decltype(other.end())>{this->end(), other.end()};
                 i.first != end.first || i.second != end.second;  ++i.first,  ++i.second ) {
            if ( i.first == end.first || i.second == end.second || *i.first != *i.second )
                return true;
        }
        return false;
    }
    template <typename _List>
    bool operator== ( const _List& other ) const { return !(*this != other); }
    auto operator[] ( std::size_t n ) -> typename iterator::reference {
        return static_cast<Node*>(*std::next(m_data.begin(), n))->x;
    }
    auto operator[] ( std::size_t n ) const -> typename const_iterator::reference {
        return static_cast<const Node*>(*std::next(m_data.begin(), n))->x;
    }
    std::size_t size() const { return m_data.size(); }
    std::size_t capacity() const { return m_data.capacity(); }
    void erase ( std::size_t pos, std::size_t n ) {
        for ( auto i = std::next(m_data.begin(), pos), end = std::next(i, n); i != end; i++ )
            delete static_cast<Node*>(*i);
        m_data.erase(pos, n);
    }
    void clear() { erase(0, size()); }
    void remove ( std::size_t pos ) { erase(pos, 1); }
    void reserve ( std::size_t n ) { m_data.reserve(n); }
    void reverse() { m_data.reverse(); }
    template <typename _Tp0>
    auto push_front ( _Tp0&& x ) -> typename std::enable_if_t<std::is_convertible<_Tp0, _Tp>{}> {
        m_data.push_front(new Node{static_cast<_Tp>(std::forward<_Tp0>(x))});
    }
    template <typename _Tp0>
    auto push_back ( _Tp0&& x ) -> typename std::enable_if_t<std::is_convertible<_Tp0, _Tp>{}> {
        m_data.push_back(new Node{static_cast<_Tp>(std::forward<_Tp0>(x))});
    }
    template <typename... _Args>
    void emplace_front ( _Args&&... __args ) { push_front(_Tp(std::forward<_Args...>(__args)...)); }
    template <typename... _Args>
    void emplace_back ( _Args&&... __args ) { push_back(_Tp(std::forward<_Args...>(__args)...)); }
    template <typename _List>
    auto append ( _List&& list ) -> require_compatible_list<_List> {
        class input_list_wrapper {
            _List& list;
        public:
            explicit input_list_wrapper ( _List& list ) : list{list} {}
            typename std::conditional_t<std::is_reference<_List>{}, decltype         (list.begin()),
                                                 std::move_iterator<decltype         (list.begin())>>
                                                 begin  () { return decltype(begin()){list.begin()}; }
            typename std::conditional_t<std::is_reference<_List>{}, decltype         (list.end  ()),
                                                 std::move_iterator<decltype         (list.end  ())>>
                                                 end    () { return decltype(end  ()){list.end  ()}; }
        };
        m_data.reserve_back(std::distance(list.begin(), list.end()));
        for ( auto&& i: input_list_wrapper{list} )
            push_back(std::forward<decltype(i)>(i));
    }
    void append ( MList&& list ) {
        m_data.reserve_back(list.size());
        // Simply move the pointers instead of copying the actual objects
        for ( auto&& i: list.m_data )
            m_data.push_back(i);
        // Erase the pointers, so that the destructor doesn't free the data they point to
        list.m_data.erase(0, size());
    }
    template <typename _List>
    auto prepend ( _List&& list ) -> require_compatible_list<_List> {
        class input_list_reverse_wrapper {
            _List& list;
        public:
            explicit input_list_reverse_wrapper ( _List& list ) : list{list} {}
            std::reverse_iterator<typename std::conditional_t
                                 <std::is_reference<_List>{}, decltype         (list.end()),
                                           std::move_iterator<decltype         (list.end())>>>
                                           begin  () { return decltype(begin()){list.end()}; }
            std::reverse_iterator<typename std::conditional_t
                                 <std::is_reference<_List>{}, decltype       (list.begin()),
                                           std::move_iterator<decltype       (list.begin())>>>
                                           end    () { return decltype(end()){list.begin()}; }
        };
        m_data.reserve_front(std::distance(list.begin(), list.end()));
        for ( auto&& i: input_list_reverse_wrapper{list} )
            push_front(std::forward<decltype(i)>(i));
    }
    void prepend ( MList&& list ) {
        class list_data_reverse_wrapper {
            MList& list;
        public:
            explicit list_data_reverse_wrapper ( MList& list ) : list{list} {}
            std::reverse_iterator<typename _ListData::iterator> begin () { return list.m_data.end(); }
            std::reverse_iterator<typename _ListData::iterator> end () { return list.m_data.begin(); }
        };
        m_data.reserve_front(list.size());
        // Simply move the pointers instead of copying the actual objects
        for ( auto&& i: list_data_reverse_wrapper{list} )
            m_data.push_front(i);
        // Erase the pointers, so that the destructor doesn't free the data they point to
        list.m_data.erase(0, size());
    }
    template <typename T>
    auto operator<< ( T&& x ) -> typename std::enable_if_t<std::is_void<decltype(this->push_back(std::forward<T>(x)))>{}, MList&> {
        push_back(std::forward<T>(x));
        return *this;
    }
    template <typename T>
    auto operator<< ( T&& x ) -> typename std::enable_if_t<std::is_void<decltype(this->append(std::forward<T>(x)))>{}, MList&> {
        append(std::forward<T>(x));
        return *this;
    }
    iterator begin() { return m_data.begin(); }
    iterator end() { return m_data.end(); }
    reverse_iterator rbegin() { return end(); }
    reverse_iterator rend() { return begin(); }
    const_iterator begin() const { return m_data.begin(); }
    const_iterator end() const { return m_data.end(); }
    const_reverse_iterator rbegin() const { return end(); }
    const_reverse_iterator rend() const { return begin(); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    const_reverse_iterator crbegin() const { return end(); }
    const_reverse_iterator crend() const { return begin(); }

private:
    _ListData m_data;
};

#endif // MLIST_H
