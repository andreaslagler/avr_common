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

#ifndef UTILITY_H
#define UTILITY_H

#include <bits/pair.h>
#include <bits/move.h>

#if SINCE_CXX17

struct in_place_t
{
    explicit in_place_t() = default;
};
inline constexpr in_place_t in_place{};

template< class T >
struct in_place_type_t {
    explicit in_place_type_t() = default;
};
template< class T >
inline constexpr in_place_type_t<T> in_place_type{};

template< std::size_t I >
struct in_place_index_t {
    explicit in_place_index_t() = default;
};
template< std::size_t I >
inline constexpr in_place_index_t<I> in_place_index{};

#endif

#endif