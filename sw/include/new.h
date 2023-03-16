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

#ifndef NEW_H
#define NEW_H

#include <bits/c++config.h>
#include <bits/new.h> // placement-new()

#include <stddef.h> // size_t

#include <allocator.h>

[[nodiscard]] inline void* operator new(const size_t size)
{
    void* ptr = HeapAllocator<>::allocate(size);
    if (!ptr)
    {
        // Handle out-of-memory condition
    }
    return ptr;
}

[[nodiscard]] inline void* operator new[](const size_t size)
{
    void* ptr = HeapAllocator<>::allocate(size);
    if (!ptr)
    {
        // Handle out-of-memory condition
    }
    return ptr;
}

inline void operator delete(void* ptr)
{
    HeapAllocator<>::deallocate(ptr);
}

inline void operator delete[](void* ptr)
{
    HeapAllocator<>::deallocate(ptr);
}

inline void operator delete(void* ptr, size_t)
{
    HeapAllocator<>::deallocate(ptr);
}

inline void operator delete[](void* ptr, size_t)
{
    HeapAllocator<>::deallocate(ptr);
}

#endif