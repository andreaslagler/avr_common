/*
Copyright (C) 2023 Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "allocator.h"
#include "..\..\common\debug_print.h"

// Test for AvailableListAllocator
int main(void)
{
    // Allocator for up to 4 uint16_t
    AvailableListAllocator<uint16_t, 4> allocator;

    // Allocate/deallocate different number of bytes
    void * ptr;

    ptr = allocator.alloc(0);
    cout << ptr;
    allocator.dealloc(ptr);

    ptr = allocator.alloc(1);
    cout << ptr;
    allocator.dealloc(ptr);

    ptr = allocator.alloc(2);
    cout << ptr;
    allocator.dealloc(ptr);
    
    //ptr = allocator.alloc(3); // Error: bad alloc 

    // Allocate multiple pointers
    void * ptr1 = allocator.alloc(1);
    cout << ptr1;

    void * ptr2 = allocator.alloc(1);
    cout << ptr2;

    void * ptr3 = allocator.alloc(1);
    cout << ptr3;

    void * ptr4 = allocator.alloc(1);
    cout << ptr4;

    void * ptr5 = allocator.alloc(1); // This will return nullptr
    cout << ptr5;
    
    allocator.dealloc(ptr2);
    
    ptr5 = allocator.alloc(1);
    cout << ptr5;
    
    allocator.dealloc(ptr1);
    allocator.dealloc(ptr3);
    allocator.dealloc(ptr4);
    allocator.dealloc(ptr5);
    
    // Deallocate nullptr
    allocator.dealloc(nullptr);
    
    cout << "END OF PROGRAM";
    
    while (true)
    {
    }
}

[[noreturn]] void throw_bad_alloc()
{
    while(true);
}


// Specialization of debugging output
template<>
struct debugPrinter<const char*>
{
    static void print(const char* arg)
    {
        // Put a tracepoint here and display {arg, s} in output window
        (void)arg;
    }
};

template<>
struct debugPrinter<void*>
{
    static void print(void* arg)
    {
        // Put a tracepoint here and display {arg} in output window
        (void)arg;
    }
};
