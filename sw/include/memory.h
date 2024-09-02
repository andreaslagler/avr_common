/*
Copyright (C) 2024  Andreas Lagler

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

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <bits/new.h>
#include <bits/move.h>
#include <atomic.h>
#include <allocator.h>
#include <exception.h>
#include <utility.h>

template<typename T, typename... Args>
void construct_at(T* ptr, Args&&... args)
{
    // Just call placement-new
    new (ptr) T(forward<Args>(args)...);
}

template<typename T>
void destroy_at(T* ptr)
{
    ptr->~T();
}

template<typename T, typename U = T>
T exchange(T& obj, U&& new_value)
{
    T old_value = move(obj);
    obj = forward<U>(new_value);
    return old_value;
}

template <typename T, typename Allocator = HeapAllocator<>>
class SharedPtr
{
    public:

    // Default constructor
    constexpr SharedPtr() = default;

    // Default constructor
    constexpr SharedPtr(nullptr_t) : m_controlBlock(nullptr)
    {}

    // Initializing constructor
    template<typename... Args>
    constexpr explicit SharedPtr(in_place_type_t<T>, Args&&... args) : m_controlBlock(static_cast<ControlBlock<>*>(Allocator().allocate(sizeof(ControlBlock<>))))
    {
        if (m_controlBlock)
        {
            construct_at(m_controlBlock, forward<Args>(args)...);
        }
        else
        {
            throw_bad_alloc();
        }            
    }

    // Copy constructor
    constexpr SharedPtr(const SharedPtr& other) : m_controlBlock(other.m_controlBlock)
    {
        if (m_controlBlock)
        {
            m_controlBlock->addRef();
        }
    }

    // Move constructor
    constexpr SharedPtr(SharedPtr&& other) : m_controlBlock(other.m_controlBlock)
    {
        // Invalidate other
        other.m_controlBlock = nullptr;
    }

    // Destructor
    constexpr ~SharedPtr()
    {
        release();
    }

    // Copy assignment
    constexpr SharedPtr& operator=(const SharedPtr& other)
    {
        // Prevent assignment to self
        if (this != &other)
        {
            release();
            m_controlBlock = other.m_controlBlock;
            if (m_controlBlock)
            {
                m_controlBlock->addRef();
            }
        }
        return *this;
    }

    // Move assignment
    constexpr SharedPtr& operator=(SharedPtr&& other)
    {
        // Prevent assignment to self
        if (this != &other)
        {
            release();
            m_controlBlock = exchange(other.m_controlBlock, nullptr);
        }
        return *this;
    }

    // Dereference operators
    [[nodiscard]] constexpr T& operator*() const
     {
        if (!m_controlBlock)
        {
            throw_nullptr_error();
        }
        return *(m_controlBlock->getObject());
    }

    [[nodiscard]] constexpr T* operator->() const
    {
        return m_controlBlock->getObject();
    }

    // Get raw pointer
    [[nodiscard]] constexpr T* get() const
    {
        return m_controlBlock ? m_controlBlock->getObject() : nullptr;
    }

    public:
    
    template<typename TCount = uint8_t>
    class ControlBlock
    {
        public:
        
        template<typename... Args>
        constexpr ControlBlock(Args&&... args) : m_object(forward<Args>(args)...)
        {}

        constexpr void addRef()
        {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                ++m_refCount;
            }
        }

        constexpr TCount release()
        {
            TCount newCount;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                newCount = --m_refCount;
            }
            return newCount;
        }

        constexpr T* getObject()
        {
            return &m_object;
        }

        private:
        
        TCount m_refCount = 1;
        T m_object;
    };

    
    ControlBlock<>* m_controlBlock{nullptr};

    constexpr void release()
    {
        if (m_controlBlock && m_controlBlock->release() == 0)
        {
            destroy_at(m_controlBlock);
            Allocator().deallocate(m_controlBlock);
        }
    }
};

// Simplified make_shared function using the default allocator
template <typename T, typename... Args>
[[nodiscard]] SharedPtr<T> makeShared(Args&&... args)
{
    return SharedPtr<T>(in_place_type<T>, forward<Args>(args)...);
}




#endif