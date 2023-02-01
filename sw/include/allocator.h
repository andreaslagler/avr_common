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

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <bits/c++config.h>
#include <stddef.h> // size_t
#include "exception.h" // throw_bad_alloc()


/**
@brief Available-list allocator
Memory allocator using a fixed capacity list of available memory nodes each holding memory for one element of specified type
@tparam T Data type managed by allocator class
@tparam t_size Number of memory nodes
*/
template <typename T, size_t t_size>
class AvailableListAllocator
{
    public:
    
    /**
    @brief Default constructor
    */
    CXX14_CONSTEXPR AvailableListAllocator()
    {
        // Build linked list of memory nodes
        for (size_t cnt = 0; cnt < t_size-1; ++cnt)
        {
            m_nodes[cnt].next = &m_nodes[cnt+1];
        }
        
        // Attach to list head
        head = m_nodes;
        
        // Terminate list with nullptr
        m_nodes[t_size-1].next = nullptr;
    }
    
    /**
    @brief Allocation of memory
    Allocates the memory of one memory node detached from the list of available nodes.
    Throws bad alloc exception if requested allocation size exceeds node size.
    @result Pointer to allocated memory for storing one element of type T
    @note If the allocator is out of memory, a nullptr is returned
    */
    CXX14_CONSTEXPR void * alloc(const size_t size)
    {
        if (sizeof(T) < size)
        {
            throw_bad_alloc();
        }
        
        Node* ptr = head;
        
        if (nullptr != ptr)
        {
            head = ptr->next;
        }
                
        return ptr;
    }

    /**
    @brief Deallocation of memory
    Deallocates a given pointer to memory and attaches the corresponding memory node to the list of available nodes.
    @param Pointer to memory to be deallocated
    */
    CXX14_CONSTEXPR void dealloc(void * ptr)
    {
        if (nullptr != ptr)
        {
            Node* node = static_cast<Node*>(ptr);
            node->next = head;
            head = node;
        }
    }

    private:

    // Memory node
    union Node
    {
        char data[sizeof(T)]; // 8 bit AVR does not require any data alignment
        Node* next;
    };
    
    // Array of available memory nodes
    Node m_nodes[t_size];
    
    // List of available memory nodes
    // NB the list is declared volatile for interrupt-safety
    Node * volatile head = nullptr;
};

#endif