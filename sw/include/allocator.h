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
#include <type_traits> // DownCast

/**
@brief Pool allocator
Memory allocator using a fixed capacity linked list of available memory nodes each holding memory for one element of specified type
@tparam T Data type managed by allocator class
@tparam t_capacity Number of memory nodes
*/
template <typename T, size_t t_capacity>
class PoolAllocator
{
    public:
    
    using size_type = typename DownCast<t_capacity>::type;
    
    /**
    @brief Default constructor
    */
    CXX14_CONSTEXPR PoolAllocator()
    {
        // Build linked list of memory nodes
        for (size_t cnt = 0; cnt < t_capacity-1; ++cnt)
        {
            m_nodes[cnt].m_next = &m_nodes[cnt+1];
        }
        
        // Attach to list head
        m_head = m_nodes;
        
        // Terminate list with nullptr
        m_nodes[t_capacity-1].m_next = nullptr;
    }
    
    /**
    @brief Allocation of memory
    Allocates the memory of one memory node detached from the pool.
    @result Pointer to allocated memory for storing one element of type T
    @note If the allocator is out of memory, a nullptr is returned
    */
    constexpr void * allocate(const size_t size) const
    {
        Node* ptr = m_head;
        if (ptr == nullptr || sizeof(T) < size)
        {
            return nullptr;
        }
        m_head = ptr->m_next;
        return ptr;
    }

    /**
    @brief Deallocation of memory
    Deallocates a given pointer to memory and returns the corresponding memory node to the pool.
    @param Pointer to memory to be deallocated
    */
    constexpr void deallocate(void * ptr) const
    {
        if (nullptr != ptr)
        {
            Node* node = static_cast<Node*>(ptr);
            node->m_next = m_head;
            m_head = node;
        }
    }
    
    /**
    @brief Equality operator
    Check if allocator is equal to other
    @param other Allcoator to compare with
    @result true if allocators are equal, false otherwise
    */
    constexpr bool operator==(const PoolAllocator& other) const
    {
        return m_nodes == other.m_nodes;
    }
    
    /**
    @brief Allocator capacity
    Total number of memory nodes
    @result Allocator capacity
    */
    constexpr static size_type capacity()
    {
        return static_cast<size_type>(t_capacity);
    }    

    private:

    // Memory node
    union Node
    {
        char m_data[sizeof(T)]; // 8 bit AVR does not require any data alignment
        Node* m_next;
    };
    
    // Array of available memory nodes
    Node m_nodes[t_capacity];
    
    // List of available memory nodes
    // NB the list is declared volatile for interrupt-safety
    mutable Node * m_head = nullptr;
};


/**
@brief Free list allocator
Memory allocator using a linked list of available memory nodes of individual size
*/
class FreeListAllocator
{
    public:
    
    using size_type = size_t;
    
    /**
    @brief Constructor
    Constructs a free list allocator from a given data pointer and size
    @param memory Pointer to memory to allocate from
    @param size Number of bytes available
    */
    FreeListAllocator(void* memory = nullptr, size_type size = 0) : m_memory(memory), m_capacity(size - sizeof(Node))
    {
        m_head = reinterpret_cast<Node*>(memory);
        m_head->m_next = nullptr;
        m_head->m_size = size - sizeof(Node);
    }


    /**
    @brief Allocation of memory
    Allocates the memory of one memory node detached from the pool.
    @result Pointer to allocated memory for storing one element of type T
    @note If the allocator is out of memory, a nullptr is returned
    */
    CXX14_CONSTEXPR void* allocate(const size_type size) const
    {
        // Find first memory node of sufficient size
        Node* prevNode = nullptr;
        Node* currNode = m_head;
        while (nullptr != currNode)
        {
            if (currNode->m_size >= size)
            {
                // Current memory node has sufficient size

                // Split current node if size is sufficient
                if (currNode->m_size > size + sizeof(Node))
                {
                    // The new node is placed directly after the current node
                    currNode->m_size -= size + sizeof(Node);
                    Node * newNode = reinterpret_cast<Node*>(reinterpret_cast<char*>(currNode) + currNode->m_size + sizeof(Node));
                    newNode->m_size = size;
                    return reinterpret_cast<void*>(reinterpret_cast<char*>(newNode) + sizeof(Node));
                }

                // --> check its previous node
                if (nullptr != prevNode)
                {
                    // Previous node is not NULL, i.e. current node is not the head of the free list
                    // --> Remove current node from the list
                    prevNode->m_next = currNode->m_next;
                }
                else
                {
                    // Previous node is NULL, i.e. current node is the head of the free list
                    // --> Make node next to current node the new head of the free list
                    m_head = currNode->m_next;
               }                
                
                // return node memory
                return reinterpret_cast<void*>(reinterpret_cast<char*>(currNode) + sizeof(Node));
            }
            
            // Iterate to next memory node
            prevNode = currNode;
            currNode = currNode->m_next;
        }
        
        // No memory node found
        // --> return NULL
        return nullptr;
    }

    /**
    @brief Deallocation of memory
    Deallocates a given pointer to memory and returns the corresponding memory node to the pool.
    @param Pointer to memory to be deallocated
    */
    CXX14_CONSTEXPR void deallocate(void* ptr) const
    {
        if (nullptr == ptr)
        {
            return;
        }
        
        // Create a new memory node from the deallocated pointer
        Node* newNode = reinterpret_cast<Node*>(reinterpret_cast<char*>(ptr) - sizeof(Node));
        
        // Now add the new node to the free list in such a way that node pointers are sorted, so deallocated memory can be defragmented by joining adjacent nodes
        
        // Case 1: Free list is empty
        if (nullptr == m_head)
        {
            // If free list is empty, the new node will become the head of the list
            newNode->m_next = m_head;
            m_head = newNode;
            return;
        }
        
        // Case 2: Deallocated pointer is smaller than head, and the new node can be joined with the head of the free list
        if (reinterpret_cast<char*>(ptr) + newNode->m_size == reinterpret_cast<char*>(m_head))
        {
            // Join new node and head of the free list     
            newNode->m_size += m_head->m_size + sizeof(Node);
            newNode->m_next = m_head->m_next;
            m_head = newNode;
            return;
        }
        
        // Case 3: Deallocated pointer is smaller than head, and the new node cannot be joined with the head of the free list
        if (newNode < m_head)
        {
            // Make new node the head of the free list
            newNode->m_next = m_head;
            m_head = newNode;
            return;
        }
        
        // Case 4: New node will be placed between two adjacent nodes of the free list keeping the sort order
        Node* prevNode = m_head;
        Node* nextNode = prevNode->m_next;
        while (nextNode != nullptr)
        {
            // Find position of new node
            if (newNode > prevNode)
            {
                // Try to join new and previous node
                if (reinterpret_cast<char*>(prevNode) + prevNode->m_size + sizeof(Node) == reinterpret_cast<char*>(newNode))
                {
                    // Join nodes: Add size of new node to previous node
                    prevNode->m_size += newNode->m_size + sizeof(Node);
                    
                    // New and previous node are the same now
                    newNode = prevNode;
                }
                else
                {
                    // Nodes cannot be joined --> Append new node to previous node
                    prevNode->m_next = newNode;
                }
                
                // Try to join new and next node
                if (reinterpret_cast<char*>(newNode) + newNode->m_size + sizeof(Node) == reinterpret_cast<char*>(nextNode))
                {
                    // Join nodes: Add size of new node to next node
                    newNode->m_size += nextNode->m_size + sizeof(Node);
                    
                    // Append the successor of next node
                    newNode->m_next = nextNode->m_next;
                }
                else
                {                    
                    // Nodes cannot be joined --> Append next node to new node
                    newNode->m_next = nextNode;
                }
                
                // Either way, deallocation is done
                return;
            }

            // Iterate to next node
            prevNode = nextNode;
            nextNode = prevNode->m_next;
        }
        
        // Case 5: New node is the successor of the last node of the free list, and nodes can be joined
        if (reinterpret_cast<char*>(prevNode) + prevNode->m_size + sizeof(Node) == reinterpret_cast<char*>(newNode))
        {
            prevNode->m_size += newNode->m_size + sizeof(Node);
            return;
        }
        
        // Case 6: New node is the successor of the last node of the free list, and nodes cannot be joined
        prevNode->m_next = newNode;
        newNode->m_next = nullptr;
    }
    
    /**
    @brief Equality operator
    Check if allocator is equal to other
    @param other Allcoator to compare with
    @result true if allocators are equal, false otherwise
    */
    constexpr bool operator==(const FreeListAllocator& other) const
    {
        return m_memory == other.m_memory;
    }
    
    /**
    @brief Allocator capacity
    Total number of bytes available
    @result Allocator capacity
    */
    constexpr size_type capacity() const
    {
        return m_capacity;
    }
    
    private:
    
    struct Node
    {
        size_type m_size;
        Node* m_next;
    };

    void* m_memory = nullptr;
    size_t m_capacity = 0;
    mutable Node* m_head = nullptr;
};

#endif