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

#ifndef MOVE_H
#define MOVE_H

#include <bits/c++config.h>
#include <type_traits.h>

/**
@brief Forward an object
Forwards lvalues as either lvalues or as rvalues, depending on T
When t is a forwarding reference (a function argument that is declared as an rvalue reference to a cv-unqualified function template parameter), this overload forwards the argument to another function with the value category it had when passed to the calling function.
@tparam T
@param t The object to be forwarded
@result static_cast<T&&>(t)
*/
template<typename T>
CXX14_CONSTEXPR T&& forward(typename remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}

/**
@brief Forward an object
Forwards rvalues as rvalues and prohibits forwarding of rvalues as lvalues
This overload makes it possible to forward a result of an expression (such as function call), which may be rvalue or lvalue, as the original value category of a forwarding reference argument.
@tparam T
@param t The object to be forwarded
@result static_cast<T&&>(t)
*/
template<typename T>
CXX14_CONSTEXPR T&& forward(typename remove_reference<T>::type&& t)
{
    static_assert(!is_lvalue_reference<T>::value);
    return static_cast<T&&>(t);
}

/**
@brief Move an object
move is used to indicate that an object t may be "moved from", i.e. allowing the efficient transfer of resources from t to another object.
In particular, move produces an xvalue expression that identifies its argument t. It is exactly equivalent to a static_cast to an rvalue reference type.
@tparam T
@param t The object to be moved
@result static_cast<typename remove_reference<T>::type&&>(t)
*/
template<typename T>
CXX14_CONSTEXPR typename remove_reference<T>::type&& move(T&& t)
{
    return static_cast<typename remove_reference<T>::type&&>(t);
}

/**
@brief Swaps the values of two objects
Swaps the values a and b
@param a the values to be swapped
@param b the values to be swapped
*/
template <class T>
constexpr void swap(T& a, T& b)
{
    T tmp(move(a));
    a = move(b);
    b = move(tmp);
}

/**
@brief Swaps the values of two objects
Swaps the arrays a and b
@param a the values to be swapped
@param b the values to be swapped
*/
template< class T, uint8_t N >
constexpr void swap(T (&a)[N], T (&b)[N])
{
    T* ptrB = b;
    for (T & elemA : a)
    {
        swap(elemA, *ptrB++);
    }
}

#endif