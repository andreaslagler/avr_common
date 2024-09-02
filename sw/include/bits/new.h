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

#ifndef BITS_NEW_H
#define BITS_NEW_H

#include <bits/c++config.h>
#include <stddef.h> // size_t

// Placement new() operator
constexpr inline void* operator new (size_t, void* ptr)
{
    return ptr;
}

// Placement new[]() operator
constexpr inline void* operator new[] (size_t, void* ptr)
{
    return ptr;
}


#endif