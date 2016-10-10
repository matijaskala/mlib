/*
 * This file is part of MLib
 * Copyright (C) 2016  Matija Skala <mskala@gmx.com>
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

#ifndef MVECTOR_H
#define MVECTOR_H

#include <complex>

template<typename N>
class MVector
{
public:
    typedef N* iterator;
    typedef const N* const_iterator;

private:
    std::size_t dim;
    N* data;

    explicit MVector ( std::size_t dim, N* data ) : dim{dim}, data{data} {}

#define foreach_d for ( std::size_t i = 0; i < dim; i++ )
#define d begin()[i]
public:
    MVector ( std::initializer_list<N> initializers )
    : MVector{initializers.size(), new N[dim]} {
        std::size_t i = 0;
        for ( const N& t: initializers )
            data[i++] = t;
    }
    static MVector fromData ( std::size_t dim, N* data ) {
        return MVector ( dim, data );
    }
    ~MVector () {
        if ( data )
            delete data;
    }
    MVector ( const MVector& other )
    : MVector{other.dimensions(), new N[dim]} {
        foreach_d
            d = other.d;
    }
    MVector ( MVector&& other )
    : MVector{other.dimensions(), other.data} {
        other.dim = 0;
        other.data = nullptr;
    }
    MVector& operator= ( const MVector& other ) {
        if ( dim < other.dimensions() )
            data = ( delete data, new N[other.dimensions()] );
        dim = other.dimensions();
        foreach_d
            d = other.d;
        return *this;
    }
    MVector& operator= ( MVector&& other ) {
        std::swap(dim, other.dim);
        std::swap(data, other.data);
        return *this;
    }

    template<typename O>
    MVector ( const MVector<O>& other )
    : dim{other.dimensions()}
    , data{new N[dim]} {
        foreach_d
            d = other.d;
    }
    template<typename O>
    MVector& operator= ( const MVector<O>& other ) {
        if ( dim < other.dimensions() )
            data = ( delete data, new N[other.dimensions()] );
        dim = other.dimensions();
        foreach_d
            d = other.d;
        return *this;
    }
    template<typename O>
    bool operator== ( const MVector<O>& other ) const {
        if ( dim != other.dimensions() )
            return false;
        foreach_d
            if ( d != other.d )
                return false;
        return true;
    }
    template<typename O>
    bool operator!= ( const MVector<O>& other ) const {
        if ( dim != other.dimensions() )
            return true;
        foreach_d
            if ( d != other.d )
                return true;
        return false;
    }
    template<typename O>
    const MVector& operator+= ( const MVector<O>& other ) {
        if ( !other.dimensions() )
            return *this;
        if ( !dim )
            return *this = +other;
        if ( dim != other.dimensions() )
            throw std::invalid_argument{"operator+(" + std::to_string(dim) + "-dimensional MVector, " + std::to_string(other.dimensions()) + "-dimensional MVector)"};
        foreach_d
            d += other.d;
        return *this;
    }
    template<typename O>
    const MVector& operator-= ( const MVector<O>& other ) {
        if ( !other.dimensions() )
            return *this;
        if ( !dim )
            return *this = -other;
        if ( dim != other.dimensions() )
            throw std::invalid_argument{"operator-(" + std::to_string(dim) + "-dimensional MVector, " + std::to_string(other.dimensions()) + "-dimensional MVector)"};
        foreach_d
            d -= other.d;
        return *this;
    }
    template<typename O>
    const MVector& operator*= ( O scalar ) {
        foreach_d
            d *= scalar;
        return *this;
    }
    template<typename O>
    const MVector& operator/= ( O scalar ) {
        foreach_d
            d /= scalar;
        return *this;
    }
    explicit operator bool () const {
        foreach_d
            if ( d )
                return true;
        return false;
    }
    template<typename O>
    auto operator+ ( const MVector<O>& other ) const {
        MVector<decltype(N{} + O{})> r = *this;
        return r += other;
    }
    template<typename O>
    auto operator- ( const MVector<O>& other ) const {
        MVector<decltype(N{} - O{})> r = *this;
        return r -= other;
    }
    template<typename O>
    auto operator* ( O scalar ) const {
        MVector<decltype(N{} * O{})> r = *this;
        return r *= scalar;
    }
    template<typename O>
    auto operator/ ( O scalar ) const {
        MVector<decltype(N{} / O{})> r = *this;
        return r /= scalar;
    }
    MVector operator+ () const {
        MVector r = *this;
        foreach_d
            r.d = +d;
        return r;
    }
    MVector operator- () const {
        MVector r = *this;
        foreach_d
            r.d = -d;
        return r;
    }

    std::size_t dimensions() const { return dim; }
    iterator begin () { return data; }
    iterator end () { return data + dim; }
    const_iterator begin () const { return data; }
    const_iterator end () const { return data + dim; }
#undef d
#undef foreach_d
};

template<typename N, typename O>
inline auto operator* ( N scalar, const MVector<O>& vector ) {
    return vector * scalar;
}

template<typename N, typename O>
inline auto operator/ ( N scalar, const MVector<O>& vector ) {
    return vector / scalar;
}

template<typename N, typename O>
auto scalar_multiplication(const MVector<N>& a, const MVector<O>& b)
{
    if(a.dimensions() != b.dimensions())
        throw std::invalid_argument{"scalar_multiplication(" + std::to_string(a.dimensions()) + "-dimensional MVector, " + std::to_string(b.dimensions()) + "-dimensional MVector)"};
    auto ret = N{} * O{};
    for(std::size_t i = 0; i < a.dimensions(); i++)
        ret += a.begin()[i] * std::conj(b.begin()[i]);
    return ret;
}

template<typename N, typename O>
auto vector_multiplication(const MVector<N>& a, const MVector<O>& b)
{
    if (a.dimensions() != 3 || b.dimensions() != 3)
        throw std::invalid_argument{"vector_multiplication(" + std::to_string(a.dimensions()) + "-dimensional MVector, " + std::to_string(b.dimensions()) + "-dimensional MVector)"};
    auto A = a.begin();
    auto B = b.begin();
    return MVector<decltype(N{} * O{} + N{} * O{})>{
        A[1] * B[2] - A[2] * B[1],
        A[2] * B[0] - A[0] * B[2],
        A[0] * B[1] - A[1] * B[0],
    };
}

template<typename N, typename C, typename T>
std::basic_istream<C, T>& operator>> ( std::basic_istream<C, T>& is, MVector<N>& vector ) {
    N x;
    C ch;
    is >> ch;
    if (ch != '(') {
        is.putback(ch);
        is >> x;
        vector = { x };
        return is;
    }
    std::size_t dim = 0;
    N* data = nullptr;
    is >> ch;
    while ( ch != ')' ) {
        is.putback(ch);
        is >> x >> ch;
        auto newData = new N[++dim];
        for ( auto i = dim - 2; i >= 0; i-- )
            newData[i] = std::move(data[i]);
        if ( data )
            delete data;
        data = newData;
        data[dim - 1] = x;
    }
    vector = MVector<N>::fromData ( dim, data );
    return is;
}

template<typename N, typename C, typename T>
std::basic_ostream<C, T>& operator<< ( std::basic_ostream<C, T>& os, const MVector<N>& vector ) {
    if (!vector.dimensions())
        return os << "()";
    std::basic_ostringstream<C, T> s;
    s.flags(os.flags());
    s.imbue(os.getloc());
    s.precision(os.precision());
    s << '(';
    auto i = vector.begin();
    s << *i;
    for ( i++; i != vector.end(); i++ )
        s << ',' << *i;
    s << ')';
    return os << s.str();
}

namespace M {

template<typename N>
auto abs ( const MVector<N>& vector ) {
    return std::sqrt(scalar_multiplication(vector, vector));
}
}

#endif // MVECTOR_H
