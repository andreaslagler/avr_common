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
#include <bits/move.h>

/**
@brief Pool allocator
Memory allocator using a fixed capacity linked list of available memory nodes each holding a fixed size block of memory
*/
class PoolAllocator
{
    public:
    
    using size_type = size_t;
    
    /**
    @brief Default constructor
    */
    CXX14_CONSTEXPR PoolAllocator() = default;
    
    
    CXX14_CONSTEXPR PoolAllocator(void* memory, size_type memorySize, size_type nodeSize) : m_nodeSize(nodeSize > sizeof(Node) ? nodeSize : sizeof(Node))
    {
        m_head = nullptr;
        while (memorySize >= m_nodeSize)
        {
            Node* node = reinterpret_cast<Node*>(memory);
            node->m_next = m_head;
            m_head = node;
            memory = reinterpret_cast<uint8_t*>(memory) + m_nodeSize;
            memorySize -= m_nodeSize;
        }
    }
    
        /**
    @brief Copy constructor
    There cannot be two copies of the one allocator managing the same memory
    @param other Allocator to copy from
    */
    PoolAllocator(const PoolAllocator& other) = delete;

    /**
    @brief move constructor
    Constructs a free list allocator from another free list allocator using move semantics
    @param other Allocator to move from
    */
    CXX14_CONSTEXPR PoolAllocator(PoolAllocator&& other)
    {
        if (&other != this)
        {
            swap(other);
        }        
    }

    /**
    @brief Copy assignment
    There cannot be two copies of the one allocator managing the same memory
    @param other Allocator to copy from
    */
    PoolAllocator& operator=(const PoolAllocator& other) = delete;

    /**
    @brief Move assignment
    Assigns a free list allocator using move semantics
    @param other Allocator to move from
    */
    CXX14_CONSTEXPR PoolAllocator& operator=(PoolAllocator&& other)
    {
        if (&other != this)
        {
            swap(other);
        }
        return *this;
    }
    
    /**
    @brief Allocation of memory
    Allocates the memory of one memory node detached from the pool.
    @result Pointer to allocated memory for storing one element of type T
    @note If the allocator is out of memory, a nullptr is returned
    */
    CXX14_CONSTEXPR void * allocate(const size_t size)
    {
        Node* ptr = m_head;
        if (ptr == nullptr || m_nodeSize < size)
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
    CXX14_CONSTEXPR void deallocate(void * ptr)
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
    @param other Allocator to compare with
    @result true if allocators are equal, false otherwise
    */
    constexpr bool operator==(const PoolAllocator& other)
    {
        // Since there are no copies of FreeListAllocator allowed, two equal objects must be the same object
        return this == &other;
    }
       
    /**
    @brief Swap allocators
    @param other Allocator to swap with
    */
    CXX14_CONSTEXPR void swap(PoolAllocator& other)
    {
        ::swap(m_nodeSize, other.m_nodeSize);
        ::swap(m_head, other.m_head);
    }
    
    private:

    // Memory node
    struct Node
    {
        Node* m_next;
    };
    
    size_type m_nodeSize = sizeof(Node);
    Node * m_head = nullptr;
};


/**
@brief Free list allocator
Memory allocator using a linked list of available memory nodes of individual size
*/
class FreeListAllocator
{
    public:
    
    using size_type = size_t;
    
    CXX14_CONSTEXPR FreeListAllocator() = default;
    
    /**
    @brief Constructor
    Constructs a free list allocator from a given data pointer and capacity
    @param memory Pointer to memory to allocate from
    @param size Number of bytes available
    */
    FreeListAllocator(void* memory, size_type capacity)
    {
        m_head = reinterpret_cast<Node*>(memory);
        m_head->m_next = nullptr;
        m_head->m_size = capacity - sizeof(Node);
    }

    /**
    @brief Copy constructor
    There cannot be two copies of the one allocator managing the same memory
    @param other Allocator to copy from
    */
    FreeListAllocator(const FreeListAllocator& other) = delete;

    /**
    @brief move constructor
    Constructs a free list allocator from another free list allocator using move semantics
    @param other Allocator to move from
    */
    CXX14_CONSTEXPR FreeListAllocator(FreeListAllocator&& other)
    {
        if (&other != this)
        {
            swap(other);
        }        
    }

    /**
    @brief Copy assignment
    There cannot be two copies of the one allocator managing the same memory
    @param other Allocator to copy from
    */
    FreeListAllocator& operator=(const FreeListAllocator& other) = delete;

    /**
    @brief Move assignment
    Assigns a free list allocator using move semantics
    @param other Allocator to move from
    */
    CXX14_CONSTEXPR FreeListAllocator& operator=(FreeListAllocator&& other)
    {
        if (&other != this)
        {
            swap(other);
        }
        return *this;
    }

    /**
    @brief Allocation of memory
    Allocates the memory of one memory node detached from the pool.
    @result Pointer to allocated memory for storing one element of type T
    @note If the allocator is out of memory, a nullptr is returned
    */
    CXX14_CONSTEXPR void* allocate(const size_type size)
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
    CXX14_CONSTEXPR void deallocate(void* ptr)
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
    @param other Allocator to compare with
    @result true if allocators are equal, false otherwise
    */
    constexpr bool operator==(const FreeListAllocator& other) const
    {
        // Since there are no copies of FreeListAllocator allowed, two equal objects must be the same object
        return this == &other;
    }
    
    /**
    @brief Swap allocators
    @param other Allocator to swap with
    */
    CXX14_CONSTEXPR void swap(FreeListAllocator& other)
    {
        ::swap(m_head, other.m_head);
    }
    
    private:
      
    struct Node
    {
        size_type m_size;
        Node* m_next;
    };
    
    Node* m_head = nullptr;
};

#ifndef HEAP_SIZE
#define HEAP_SIZE 1024
#endif

/**
@brief Heap allocator
Memory allocator using a linked list of available memory nodes of individual size
*/
template <size_t t_capacity = HEAP_SIZE>
class HeapAllocator
{
    public:
    
    using size_type = size_t;
    
    CXX14_CONSTEXPR HeapAllocator() = default;
    
    /**
    @brief Copy constructor
    There cannot be two copies of the one allocator managing the same memory
    @param other Allocator to copy from
    */
    CXX14_CONSTEXPR HeapAllocator(const HeapAllocator&)
    {
        // HeapAllocator is stateless --> nothing to do
    }

    /**
    @brief move constructor
    Constructs a free list allocator from another free list allocator using move semantics
    @param other Allocator to move from
    */
    CXX14_CONSTEXPR HeapAllocator(HeapAllocator&&)
    {
        // HeapAllocator is stateless --> nothing to do
    }

    /**
    @brief Copy assignment
    There cannot be two copies of the one allocator managing the same memory
    @param other Allocator to copy from
    */
    constexpr HeapAllocator& operator=(const HeapAllocator&)
    {
        // HeapAllocator is stateless --> nothing to do
        return *this;
    }

    /**
    @brief Move assignment
    Assigns a free list allocator using move semantics
    @param other Allocator to move from
    */
    constexpr HeapAllocator& operator=(HeapAllocator&&)
    {
        // HeapAllocator is stateless --> nothing to do
        return *this;
    }

    /**
    @brief Allocation of memory
    Allocates the memory of one memory node detached from the pool.
    @result Pointer to allocated memory for storing one element of type T
    @note If the allocator is out of memory, a nullptr is returned
    */
    CXX14_CONSTEXPR static void* allocate(const size_type size)
    {
        return s_allocator.allocate(size);
    }

    /**
    @brief Deallocation of memory
    Deallocates a given pointer to memory and returns the corresponding memory node to the pool.
    @param Pointer to memory to be deallocated
    */
    CXX14_CONSTEXPR static void deallocate(void* ptr)
    {
        s_allocator.deallocate(ptr);
    }
    
    /**
    @brief Equality operator
    Check if allocator is equal to other
    @param other Allocator to compare with
    @result true if allocators are equal, false otherwise
    */
    constexpr bool operator==(const HeapAllocator&) const
    {
        // Two allocators are identical if memory allocated by this can be deallocated by other and vice versa
        // This is always the case for any two HeapAllocator objects
        return true;
    }
    
    /**
    @brief Swap allocators
    @param other Allocator to swap with
    */
    constexpr void swap(HeapAllocator&)
    {
        // HeapAllocator is stateless --> nothing to do
    }
    
    private:

    static uint8_t s_memory[t_capacity];
    static FreeListAllocator s_allocator;
};

template <size_t t_capacity>
uint8_t HeapAllocator<t_capacity>::s_memory[t_capacity];

template <size_t t_capacity>
FreeListAllocator HeapAllocator<t_capacity>::s_allocator(HeapAllocator<t_capacity>::s_memory, t_capacity);


#endif