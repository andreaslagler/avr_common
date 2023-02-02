/*
Copyright (C) 2022  Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PAIR_H
#define PAIR_H

#include <bits/c++config.h>
#include <bits/move.h>

/**
@brief Binary tuple, i.e. a pair of values
Class template that provides a way to store two heterogeneous objects as a single unit.
@tparam T1, T2 the types of the elements that the pair stores.
*/
template<class T1, class T2>
struct Pair
{
    using first_type  = T1;
    using second_type = T2;
    
    T1 first;
    T2 second;
    
    /**
    @brief constructs new pair
    Default constructor. Value-initializes both elements of the pair, first and second.
    */
    constexpr Pair() = default;

    /**
    @brief constructs new pair
    Copy constructor is implicitly declared (until C++11)defaulted, and is constexpr if copying of both elements satisfies the requirements on constexpr functions
    */
    constexpr Pair(const Pair&) = default;

    /**
    @brief constructs new pair
    Move constructor is defaulted, and is constexpr if moving of both elements satisfies the requirements on constexpr functions.
    */
    constexpr Pair(Pair&&) = default;
    
    /**
    @brief constructs new pair
    Initializes first with x and second with y
    */
    constexpr Pair(const T1& x, const T2& y) : first(x), second(y)
    {}
    
    /**
    @brief constructs new pair
    Initializes first with std::forward<U1>(x) and second with std::forward<U2>(y).
    */
    template<class U1 = T1, class U2 = T2>
    constexpr Pair(U1&& x, U2&& y) : first(forward<U1>(x)), second(forward<U2>(y))
    {}
    
    /**
    @brief constructs new pair
    Initializes first with p.first and second with p.second.
    @note This constructor is defined as deleted if the initialization of first or second would bind a reference to temporary object.
    */
    template<class U1, class U2>
    constexpr Pair(Pair<U1, U2>& p) : first(p.first), second(p.second)
    {}
    
    /**
    @brief constructs new pair
    Initializes first with p.first and second with p.second.
    */
    template<class U1, class U2>
    constexpr Pair(const Pair<U1, U2>& p) : first(p.first), second(p.second)
    {}
    
    /**
    @brief constructs new pair
    Initializes first with std::forward<U1>(x) and second with std::forward<U2>(y).
    */
    template<class U1, class U2>
    constexpr Pair(Pair<U1, U2>&& p) : first(forward<U1>(p.first)), second(forward<U2>(p.second))
    {}
    
    /**
    @brief assigns the contents
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    */
    CXX20_CONSTEXPR Pair& operator=(const Pair& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }
    
    /**
    @brief assigns the contents
    Assigns other.first to first and other.second to second.
    */
    template<class U1, class U2>
    CXX20_CONSTEXPR Pair& operator=(const Pair<U1, U2>& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }
    
    /**
    @brief assigns the contents
    Move assignment operator. Replaces the contents with those of other using move semantics.
    */
    CXX20_CONSTEXPR Pair& operator=(Pair&& other)
    {
        first = forward<T1>(other.first);
        second = forward<T2>(other.second);
        return *this;
    }
    
    /**
    @brief assigns the contents
    Assigns std::forward<U1>(p.first) to first and std::forward<U2>(p.second) to second.
    */
    template<class U1, class U2>
    CXX20_CONSTEXPR Pair& operator=(Pair<U1, U2>&& other)
    {
        first = forward<U1>(other.first);
        second = forward<U2>(other.second);
        return *this;
    }
    
    /**
    @brief swaps the contents
    Swaps first with other.first and second with other.second, as if by swap(first, other.first); swap(second, other.second);.
    @note If either selected swap function call is ill-formed or does not swap the value of the member, the behavior is undefined.
    */
    CXX20_CONSTEXPR void swap(Pair& other)
    {
        ::swap(first, other.first);
        ::swap(second, other.second);
    }
};

/**
@brief creates a pair object of type, defined by the argument types
Creates a Pair object, deducing the target type from the types of arguments.
*/
template< class T1, class T2 >
CXX14_CONSTEXPR Pair<typename decay<T1>::type, typename decay<T2>::type> make_pair( T1&& t, T2&& u )
{
    return Pair<typename decay<T1>::type, typename decay<T2>::type>(t, u);
}

/**
@brief specializes the swap algorithm
Swaps the contents of x and y. Equivalent to x.swap(y).
*/
template< class T1, class T2 >
CXX20_CONSTEXPR void swap(Pair<T1,T2>& x, Pair<T1,T2>& y)
{
    x.swap(y);
}

namespace pair_helper
{
    template<size_t I, typename T>
    struct tuple_element;

    template<size_t I, typename T1, typename T2>
    struct tuple_element<I, Pair<T1,T2> >
    {
        static_assert(I < 2, "Pair has only 2 elements!");
    };

    template<typename T1, typename T2>
    struct tuple_element<0, Pair<T1,T2> >
    {
        using type = T1;
    };

    template<typename T1, typename T2>
    struct tuple_element<1, Pair<T1,T2> >
    {
        using type = T2;
    };

    template<std::size_t _Int>
    struct pair_get;

    template<>
    struct pair_get<0>
    {
        template<typename T1, typename T2>
        static constexpr T1& get(Pair<T1, T2>& p)
        {
            return p.first;
        }

        template<typename T1, typename T2>
        static constexpr T1&& move_get(Pair<T1, T2>&& p)
        {
            return forward<T1>(p.first);
        }

        template<typename T1, typename T2>
        static constexpr const T1& const_get(const Pair<T1, T2>& p)
        {
            return p.first;
        }

        template<typename T1, typename T2>
        static constexpr const T1&& const_move_get(const Pair<T1, T2>&& p)
        {
            return forward<const T1>(p.first);
        }
    };

    template<>
    struct pair_get<1>
    {
        template<typename T1, typename T2>
        static constexpr T2& get(Pair<T1, T2>& p)
        {
            return p.second;
        }

        template<typename T1, typename T2>
        static constexpr T2&& move_get(Pair<T1, T2>&& p)
        {
            return forward<T2>(p.second);
        }

        template<typename T1, typename T2>
        static constexpr const T2& const_get(const Pair<T1, T2>& p)
        {
            return p.second;
        }

        template<typename T1, typename T2>
        static constexpr const T2&& const_move_get(const Pair<T1, T2>&& p)
        {
            return forward<const T2>(p.second);
        }
    };
}

/**
@brief accesses an element of a pair
Extracts an element from the pair using tuple-like interface.
@result Returns a reference to p.first if I==0 and a reference to p.second if I==1.
*/
template< size_t I, class T1, class T2 >
CXX14_CONSTEXPR typename pair_helper::tuple_element<I, Pair<T1,T2>>::type& get(Pair<T1, T2>& p)
{
    return pair_helper::pair_get<I>::get(p);
}

/**
@brief accesses an element of a pair
Extracts an element from the pair using tuple-like interface.
@result Returns a reference to p.first if I==0 and a reference to p.second if I==1.
*/
template< size_t I, class T1, class T2 >
CXX14_CONSTEXPR const typename pair_helper::tuple_element<I, Pair<T1,T2>>::type& get(const Pair<T1,T2>& p)
{
    return pair_helper::pair_get<I>::const_get(p);
}

/**
@brief accesses an element of a pair
Extracts an element from the pair using tuple-like interface.
@result Returns a reference to p.first if I==0 and a reference to p.second if I==1.
*/
template< size_t I, class T1, class T2 >
CXX14_CONSTEXPR typename pair_helper::tuple_element<I, Pair<T1,T2>>::type&& get(Pair<T1,T2>&& p)
{
    return pair_helper::pair_get<I>::move_get(move(p));
}

/**
@brief accesses an element of a pair
Extracts an element from the pair using tuple-like interface.
@result Returns a reference to p.first if I==0 and a reference to p.second if I==1.
*/
template< size_t I, class T1, class T2 >
CXX14_CONSTEXPR const typename pair_helper::tuple_element<I, Pair<T1,T2>>::type&& get(const Pair<T1,T2>&& p)
{
    return pair_helper::pair_get<I>::const_move_get(move(p));
}

#endif