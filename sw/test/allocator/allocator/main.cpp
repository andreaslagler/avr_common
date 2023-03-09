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

bool test_assert(const char * str, const bool flag)
{
    cout << str;
    if (flag)
    {
        cout << static_cast<const char *>("PASSED");
    }
    else
    {
        cout << static_cast<const char *>("FAILED");
    }
    return flag;
}


int main(void)
{
    bool allPassed = true;
    bool testPassed = true;

    // PoolAllocator
    {
        testPassed = true;
        
        // Pool allocator for up to 4 uint16_t
        constexpr size_t capacity = 4;
        PoolAllocator<uint16_t, capacity> allocator;

        // Allocate/deallocate different number of bytes
        void * ptr = nullptr;

        ptr = allocator.allocate(0);
        testPassed &= nullptr != ptr;
        allocator.deallocate(ptr);

        ptr = allocator.allocate(1);
        testPassed &= nullptr != ptr;
        allocator.deallocate(ptr);

        ptr = allocator.allocate(2);
        testPassed &= nullptr != ptr;
        allocator.deallocate(ptr);
        
        ptr = allocator.allocate(3); // Will return NULL
        testPassed &= nullptr == ptr;

        // Allocate multiple pointers
        void * ptr1 = allocator.allocate(1);
        testPassed &= nullptr != ptr1;

        void * ptr2 = allocator.allocate(1);
        testPassed &= nullptr != ptr2;

        void * ptr3 = allocator.allocate(1);
        testPassed &= nullptr != ptr3;

        void * ptr4 = allocator.allocate(1);
        testPassed &= nullptr != ptr4;

        void * ptr5 = allocator.allocate(1); // Will return NULL
        testPassed &= nullptr == ptr5;
        
        allocator.deallocate(ptr2);
        
        ptr5 = allocator.allocate(1);
        testPassed &= nullptr != ptr5;
        
        allocator.deallocate(ptr1);
        allocator.deallocate(ptr3);
        allocator.deallocate(ptr4);
        allocator.deallocate(ptr5);
        
        // Deallocate nullptr
        allocator.deallocate(nullptr);
        
        // Check capacity
        testPassed &= capacity == allocator.capacity();
    }
    allPassed &= test_assert("PoolAllocator", testPassed);

    
    
    // FreeListAllocator
    {
        testPassed = true;
        
        constexpr size_t capacity = 24;
        char memory[capacity];
        FreeListAllocator allocator(memory, capacity);
        
        // Allocate/deallocate different number of bytes
        {
        void * ptr = nullptr;

        ptr = allocator.allocate(0);
        testPassed &= nullptr != ptr;
        allocator.deallocate(ptr);

        ptr = allocator.allocate(1);
        testPassed &= nullptr != ptr;
        allocator.deallocate(ptr);

        ptr = allocator.allocate(20);
        testPassed &= nullptr != ptr;
        allocator.deallocate(ptr);
        
        ptr = allocator.allocate(21); // Will return NULL
        testPassed &= nullptr == ptr;
        }        

        // Allocate multiple pointers without fragmenting the memory
        {
            void * ptr1 = allocator.allocate(2);
        testPassed &= nullptr != ptr1;

        void * ptr2 = allocator.allocate(2);
        testPassed &= nullptr != ptr2;

        void * ptr3 = allocator.allocate(2);
        testPassed &= nullptr != ptr3;

        void * ptr4 = allocator.allocate(2);
        testPassed &= nullptr != ptr4;

        void * ptr5 = allocator.allocate(2); // Will return NULL
        testPassed &= nullptr == ptr5;
        
        allocator.deallocate(ptr2);
        
        ptr5 = allocator.allocate(2);
        testPassed &= nullptr != ptr5;

        allocator.deallocate(ptr1);
        allocator.deallocate(ptr3);
        allocator.deallocate(ptr4);
        allocator.deallocate(ptr5);
        }        
        
       // Allocate multiple pointers with fragmenting the memory
        {
            void * ptr1 = allocator.allocate(2);
        testPassed &= nullptr != ptr1;

        void * ptr2 = allocator.allocate(2);
        testPassed &= nullptr != ptr2;

        void * ptr3 = allocator.allocate(2);
        testPassed &= nullptr != ptr3;

        void * ptr4 = allocator.allocate(2);
        testPassed &= nullptr != ptr4;

        allocator.deallocate(ptr1);
        allocator.deallocate(ptr3);
        
        // Allocator has enough free memory, but memory is fragmented
        void * ptr5 = allocator.allocate(8);
        testPassed &= nullptr == ptr5;
        
        allocator.deallocate(ptr2);
        
        // Memory should be defragmented now
        ptr5 = allocator.allocate(8);
        testPassed &= nullptr != ptr5;
        
        allocator.deallocate(ptr4);
        allocator.deallocate(ptr5);
        }        
        
        // Deallocate nullptr
        allocator.deallocate(nullptr);
        
        // Check capacity
        //testPassed &= capacity == allocator.capacity();

    }
    allPassed &= test_assert("FreeListAllocator", testPassed);

    test_assert("Overall", allPassed);
    
    while (true)
    {
    }
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

