/*
Copyright (C) 2023  Andreas Lagler

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

#ifndef STATIC_STRING_H
#define STATIC_STRING_H

#include <static_vector.h>

/**
@brief Template class implementing a static string
@tparam t_capacity Capacity of string, i.e. maximum number of characters the string can hold
*/
template <size_t t_capacity>
using StaticString = StaticVector<char, t_capacity>; // For the moment, use template alias

#endif