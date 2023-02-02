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

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <bits/c++config.h>
#include <utility.h>

#include <stdint.h>
#include <stdbool.h>

/**
@brief Applies a function to a range of elements
Applies the given function object f to the result of dereferencing every iterator in the range [first, last), in order.
@param first the range to apply the function to
@param last the range to apply the function to
@param f function object, to be applied to the result of dereferencing every iterator in the range [first, last)
*/
template<class InputIt, class UnaryFunction>
CXX20_CONSTEXPR UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f)
{
    for (; first != last; ++first)
    {
        f(*first);
    }    
    
    return f; // implicit move since C++11
}

/**
@brief Applies a function to a range of elements
Returns true if the range [first1, last1) is equal to the range [first2, first2 + (last1 - first1)), and false otherwise.
@param first1, last1 the first range of the elements to compare
@param first2 - the second range of the elements to compare
@result If the elements in the two ranges are equal, returns true. Otherwise returns false.
*/
template <class InputIt1, class InputIt2>
CXX20_CONSTEXPR bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            return false;
        }
    }
    return true;
}

/**
@brief Applies a function to a range of elements
Returns true if the range [first1, last1) is equal to the range [first2, first2 + (last1 - first1)), and false otherwise.
@param first1, last1 the first range of the elements to compare
@param first2 the second range of the elements to compare
@param p binary predicate which returns ​true if the elements should be treated as equal.
@result If the elements in the two ranges are equal, returns true. Otherwise returns false.
*/
template <class InputIt1, class InputIt2, class BinaryPredicate>
CXX20_CONSTEXPR bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p)
{
    for (; first1 != last1; ++first1, ++first2)
    {
        if (!p(*first1, *first2))
        {
            return false;
        }
    }
    return true;
}


#endif