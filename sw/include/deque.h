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

#ifndef DEQUE_H
#define DEQUE_H

#include <bits/c++config.h>
#include <bits/move.h>
#include <bits/new.h>
#include <type_traits>
#include <exception.h>

#include <initializer_list>
#include <allocator.h>
#include <stdbool.h>

/**
@brief Template class implementing a double-ended static queue of objects with compile-time fixed capacity
@tparam T Type of deque elements
*/
template <typename T, typename Allocator = HeapAllocator<>>
class Deque
{
    public:
    
    template <bool t_const, bool t_reverse>
    class Iterator;
    
    using value_type             = T;
    using allocator_type         = Allocator;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = typename allocator_type::size_type;
    using difference_type        = ptrdiff_t;
    using iterator               = Iterator<false, false>;
    using const_iterator         = Iterator<true, false>;
    using reverse_iterator       = Iterator<false, true>;
    using const_reverse_iterator = Iterator<true, true>;

    template <bool t_const, bool t_reverse>
    class Iterator
    {
        friend class Deque<value_type, allocator_type>;
        
        CXX20_CONSTEXPR Iterator(typename conditional<t_const, const Deque<value_type, allocator_type>, Deque<value_type, allocator_type>>::type* deque, const size_type idx) : m_deque(deque), m_idx(idx)
        {}
        
        public:
        
        CXX20_CONSTEXPR Iterator(const Iterator& rhs) = default;
        
        CXX20_CONSTEXPR Iterator& operator=(const Iterator& rhs)
        {
            m_deque = rhs.m_deque;
            m_idx = rhs.m_idx;
            return *this;
        }
        
        CXX20_CONSTEXPR ~Iterator() = default;
        
        CXX14_CONSTEXPR Iterator& operator++()
        {
            if CXX17_CONSTEXPR(t_reverse)
            {
                --m_idx;
            }
            else
            {
                ++m_idx;
            }
            return *this;
        }

        constexpr typename conditional<t_const, const value_type, value_type>::type& operator*() const
        {
            return m_deque->operator[](m_idx);
        }
        
        constexpr bool operator!=(const Iterator& other) const
        {
            return m_idx != other.m_idx;
        }

        private:

        typename conditional<t_const, const Deque<value_type, allocator_type>, Deque<value_type, allocator_type>>::type* m_deque = nullptr;
        size_type m_idx = 0;
    };
    
    /**
    @brief Constructor.
    Constructs an empty container with the given allocator allocator
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX20_CONSTEXPR explicit Deque(const allocator_type& allocator = allocator_type()) : m_allocator(allocator)
    {}
    
    /**
    @brief Constructor.
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX20_CONSTEXPR explicit Deque(const size_type count, const allocator_type& allocator = allocator_type()) : Deque(allocator)
    {
        m_data = allocate(count);
        m_capacity = count;
        m_size = count;
        for (; m_end < count; ++m_end)
        {
            new (m_data + m_end) value_type;
        }
    }
    
    /**
    @brief Constructor.
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    @param value the value to initialize elements of the container with
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX20_CONSTEXPR explicit Deque(const size_type count, const value_type& value, const allocator_type& allocator = allocator_type()) : Deque(allocator)
    {
        m_data = allocate(count);
        m_capacity = count;
        m_size = count;
        for (; m_end < count; ++m_end)
        {
            new (m_data + m_end) value_type(value);
        }
    }
    
    /**
    @brief Constructor.
    Constructs the container with the contents of the range [first, last).
    @param first, last the range to copy the elements from
    @param allocator allocator to use for all memory allocations of this container
    */
    template<class InputIt>
    CXX20_CONSTEXPR Deque(InputIt first, InputIt last, const allocator_type& allocator = allocator_type()) : Deque(allocator)
    {
        // Prefer fewer re-allocations and copies and determine the size before allocating the memory
        size_type count = 0;
        InputIt firstTemp = first;
        while (firstTemp != last)
        {
            ++count;
            ++firstTemp;
        }
        
        // Create the actual container
        m_data = allocate(count);
        m_capacity = count;
        m_size = count;
        for (; m_end < count; ++m_end)
        {
            new (m_data + m_end) value_type(*first);
            ++first;
        }
    }
    
    /**
    @brief move constructor.
    Copy constructor. Constructs the container with the copy of the contents of other.
    @param other another container to be used as source to initialize the elements of the container with
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX20_CONSTEXPR Deque(const Deque& other) : Deque(other.m_allocator)
    {
        const size_type count = other.size();
        m_data = allocate(count);
        m_capacity = count;
        m_size = count;
        m_end = count;
        value_type* ptr = m_data;
        for (const value_type& value : other)
        {
            new (ptr++) value_type(value);
        }
    }
    
    /**
    @brief move constructor.
    Move constructor. Constructs the container with the contents of other using move semantics. Allocator is obtained from the allocator belonging to other.
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX20_CONSTEXPR Deque(Deque&& other) : m_allocator(move(other.m_allocator))
    {
        if (m_allocator == other.m_allocator)
        {
            // Copy all members from other to this
            m_capacity = other.m_capacity;
            m_size = other.m_size;
            m_end = other.m_end;
            m_front = other.m_front;
            m_data = other.m_data;
            
            // Invalidate other object
            other.m_capacity = 0;
            other.m_size = 0;
            other.m_end = 0;
            other.m_front = 0;
            other.m_data = nullptr;
        }
        else
        {
            // Create a copy of the other container if allocators are not equal
            const size_type count = other.size();
            m_data = allocate(count);
            m_capacity = count;
            m_size = count;
            m_end = count;
            value_type* ptr = m_data;
            for (const value_type& value : other)
            {
                new (ptr++) value_type(value);
            }
        }
        
    }
    
    /**
    @brief Initializing constructor
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param init initializer list to initialize the elements of the container with
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX20_CONSTEXPR Deque(std::initializer_list<value_type> init, const allocator_type& allocator = allocator_type()) : Deque(allocator)
    {
        const size_type count = init.size();
        m_data = allocate(count);
        m_capacity = count;
        m_size = count;
        m_end = count;
        value_type* ptr = m_data;
        for (const value_type& value : init)
        {
            new (ptr++) value_type(value);
        }
    }

    /**
    @brief Destructor.
    Destructs the deque. The destructors of the elements are called and the used storage is deallocated.
    Note, that if the elements are pointers, the pointed-to objects are not destroyed.
    */
    CXX20_CONSTEXPR ~Deque()
    {
        clear();
        deallocate(m_data);
    }
    
    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR Deque& operator=(const Deque& other)
    {
        if (this != &other)
        {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Move assignment operator. Replaces the contents with those of other using move semantics (i.e. the data in other is moved from other into this container).
    other is in a valid but unspecified state afterwards.
    @param other another container to use as data source
    */
    constexpr Deque& operator=(Deque&& other)
    {
        if (this != &other)
        {
            // Check if other uses the same allocator object
            if (m_allocator == other.m_allocator)
            {
                // Swap all members from other to this
                swap(m_capacity, other.m_capacity);
                swap(m_size, other.m_size);
                swap(m_end, other.m_end);
                swap(m_front, other.m_front);
                swap(m_data, other.m_data);
            }
            else
            {
                // TODO use move semantic instead of copying the node data
                assign(other.begin(), other.end());
            }
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Replaces the contents with those identified by initializer list init.
    @param init initializer list to use as data source
    */
    CXX14_CONSTEXPR Deque& operator=(std::initializer_list<value_type> init)
    {
        assign(init);
        return *this;
    }

    /**
    @brief Assignment
    Replaces the contents with count copies of value value
    @param count the new size of the container
    @param value the value to initialize elements of the container with
    */
    CXX14_CONSTEXPR void assign(size_type count, const value_type& value = value_type())
    {
        clear();
        reallocate(count);
        m_size = count;
        for (; m_end < count; ++m_end)
        {
            new (m_data + m_end) value_type(value);
        }            
    }
    
    /**
    @brief Assignment
    Replaces the contents with copies of those in the range [first, last).
    The behavior is undefined if either argument is an iterator into *this.
    @param first, last the range to copy the elements from
    */
    // TODO restrict overload resolution
    template <typename InputIt>
    CXX14_CONSTEXPR void assign(InputIt first, InputIt last)
    {
        // Prefer fewer re-allocations and copies and determine the size before allocating the memory
        size_type count = 0;
        InputIt firstTemp = first;
        while (firstTemp != last)
        {
            ++count;
            ++firstTemp;
        }
        
        // Create the actual container
        clear();
        reallocate(count);
        for (; m_end < count; ++m_end)
        {
            new (m_data + m_end) value_type(*first);
            ++first;
        }
        m_size = m_end;
    }

    /**
    @brief Assignment
    Replaces the contents with the elements from the initializer list init.
    @param init initializer list to copy the values from
    */
    CXX14_CONSTEXPR void assign(std::initializer_list<T> init)
    {
        clear();        
        const size_type count = init.size();
        reallocate(count);
        auto first = init.begin();
        for (; m_end < count; ++m_end)
        {
            new (m_data + m_end) value_type(*first);
            ++first;
        }
        m_size = m_end;
    }
    
    /**
    @brief returns the associated allocator
    Returns the allocator associated with the container.
    @result The associated allocator.
    */
    constexpr allocator_type getAllocator() const
    {
        return m_allocator;
    }

    /**
    @brief Get const iterator pointing first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return const_iterator(this, 0);
    }
    
    /**
    @brief Get const iterator pointing to first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator begin() const
    {
        return cbegin();
    }
    
    /**
    @brief Get iterator pointing to first element of queue
    @result Begin iterator
    */
    CXX14_CONSTEXPR iterator begin()
    {
        return iterator(this, 0);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return const_iterator(this, m_size);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator end() const
    {
        return cend();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue
    @result End iterator
    */
    CXX14_CONSTEXPR iterator end()
    {
        return iterator(this, m_size);
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(this, m_size-1);
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rbegin() const
    {
        return crbegin();
    }
    
    /**
    @brief Get iterator pointing to first element of queue in reverse order
    @result reverse begin iterator
    */
    CXX14_CONSTEXPR reverse_iterator rbegin()
    {
        return reverse_iterator(this, m_size-1);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crend() const
    {
        return const_reverse_iterator(this, -1);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rend() const
    {
        return crend();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue in reverse order
    @result reverse end iterator
    */
    CXX14_CONSTEXPR reverse_iterator rend()
    {
        return reverse_iterator(this, -1);
    }

    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end()
    @result true if the container is empty, false otherwise
    */
    constexpr bool empty() const
    {
        return size() == 0;
    }

    /**
    @brief Returns the number of elements
    Returns the number of elements in the container
    @result The number of elements in the container.
    */
    constexpr size_type size() const
    {
        return m_size;
    }

    /**
    @brief Access specified element
    Returns a reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR reference operator[](const size_type pos)
    {
        size_type idx = pos + m_front; // TODO check for integer overflow
        if (idx >= m_capacity)
        {
            idx -= m_capacity;
        }
        return m_data[idx];
    }
    
    /**
    @brief Access specified element (read-only)
    Returns a const reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR const_reference operator[](const size_type pos) const
    {
        size_type idx = pos + m_front; // TODO check for integer overflow
        if (idx >= m_capacity)
        {
            idx -= m_capacity;
        }
        return m_data[idx];
    }
    
    /**
    @brief Access specified element with bounds checking
    Returns a reference to the element at specified location pos, with bounds checking.
    If pos is not within the range of the container, an exception of type out_of_range is thrown.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR reference at(const size_type pos)
    {
        if (!(pos < size()))
        {
            throw_out_of_range();
        }
        
        return this->operator[](pos);
    }
    
    /**
    @brief Access specified element (read-only) with bounds checking
    Returns a const reference to the element at specified location pos, with bounds checking.
    If pos is not within the range of the container, an exception of type out_of_range is thrown.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR const_reference at(const size_type pos) const
    {
        if (!(pos < size()))
        {
            throw_out_of_range();
        }
        
        return this->operator[](pos);
    }
    
    /**
    @brief Access the first element
    Returns a reference to the frontPos element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR reference front()
    {
        return m_data[m_front];
    }
    
    /**
    @brief access the first element (read-only)
    Returns a const reference to the frontPos element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR const_reference front() const
    {
        return m_data[m_front];
    }
    
    /**
    @brief Access the last element
    Returns a reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR reference back()
    {
        size_type idx = m_end;
        decIndex(idx);
        return m_data[idx];
    }
    
    /**
    @brief Access the last element (read-only)
    Returns a const reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR const_reference back() const
    {
        size_type idx = m_end;
        decIndex(idx);
        return m_data[idx];
    }
    
    /**
    @brief Changes the number of elements stored
    Resizes the container to contain count elements.
    If the current size is greater than count, the container is reduced to its first count elements.
    If the current size is less than count, additional default-inserted elements are appended
    @param count new size of the container
    */
    CXX14_CONSTEXPR void resize(const size_type count)
    {
        if (count > m_capacity)
        {
            reallocate(count);
        }
            
        // Add new elements until count is reached
        while (m_size < count)
        {
            ++m_size;
            new (m_data + m_end) value_type;
            incIndex(m_end);
        }
            
        // Delete elements until count is reached
        while (m_size > count)
        {
            popBack();
        }
    }
    
    /**
    @brief Changes the number of elements stored
    Resizes the container to contain count elements.
    If the current size is greater than count, the container is reduced to its first count elements.
    If the current size is less than count, additional copies of value are appended.
    @param count new size of the container
    @param value the value to initialize the new elements with
    */
    CXX14_CONSTEXPR void resize(const size_type count, const value_type& value)
    {
        if (count > m_capacity)
        {
            reallocate(count);
        }
           
        // Add new elements until count is reached
        while (m_size < count)
        {
            pushBack(value);
        }
            
        // Delete elements until count is reached
        while (m_size > count)
        {
            popBack();
        }
    }
     
    /**
    @brief reduces memory usage by freeing unused memory
    Requests the removal of unused capacity.
    All iterators and references are invalidated. end() iterator is also invalidated.
    */
    CXX14_CONSTEXPR void shrinkToFit()
    {
        reallocate(m_size);
    }    

    /**
    @brief Clears the contents
    Erases all elements from the container. After this call, size() returns zero.
    Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-end iterators are also invalidated.
    */
    CXX14_CONSTEXPR void clear()
    {
        for (value_type& value : *this)
        {
            value.~value_type();
        }
        m_front = 0;
        m_end = 0;
        m_size = 0;
    }

    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(const value_type& value)
    {
        if (full())
        {
            reallocate(m_capacity << 1);
        }
        
        ++m_size;
        new (m_data + m_end) value_type(value);
        incIndex(m_end);
    }

    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(value_type&& value)
    {
        if (full())
        {
            reallocate(m_capacity << 1);
        }
        
        ++m_size;
        new (m_data + m_end) value_type(forward<value_type>(value));
        incIndex(m_end);
    }

    /**
    @brief Constructs an element in-place at the end
    Appends a new element to the end of the container. The element is constructed through placement-new to construct the element in-place at the location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    All iterators, including the past-the-end iterator, are invalidated. No references are invalidated.
    @param arguments to forward to the constructor of the element
    */
    template <typename  ... Args>
    CXX14_CONSTEXPR reference emplaceBack(Args&& ... args)
    {
        if (full())
        {
            reallocate(m_capacity << 1);
        }

        ++m_size;
        value_type* newElem = new (m_data + m_end) value_type(forward<Args>(args)...);
        incIndex(m_end);
        return *newElem;
    }

    /**
    @brief Removes the last element
    */
    CXX14_CONSTEXPR void popBack()
    {
        --m_size;
        decIndex(m_end);
        (m_data + m_end)->~value_type();
    }
    
    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    All iterators, including the past-the-end iterator, are invalidated. No references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(const value_type& value)
    {
        if (full())
        {
            reallocate(m_capacity << 1);
        }
        
        ++m_size;
        decIndex(m_front);
        new (m_data + m_front) value_type(value);
    }

    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    All iterators, including the past-the-end iterator, are invalidated. No references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(value_type&& value)
    {
        if (full())
        {
            reallocate(m_capacity << 1);
        }
        
        ++m_size;
        decIndex(m_front);
        new (m_data + m_front) value_type(forward<T>(value));
    }

    /**
    @brief Constructs an element in-place at the beginning
    Inserts a new element to the beginning of the container. The element is constructed through  placement-new to construct the element in-place at the location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    All iterators, including the past-the-end iterator, are invalidated. No references are invalidated.
    @param arguments to forward to the constructor of the element
    */
    template <typename  ... Args>
    CXX14_CONSTEXPR reference emplaceFront(Args&& ... args)
    {
        if (full())
        {
            reallocate(m_capacity << 1);
        }

        ++m_size;
        decIndex(m_front);
        return *new (m_data + m_front) value_type(forward<Args>(args)...);
    }
    
    /**
    @brief Removes the first element
    Removes the first element of the container. If there are no elements in the container, the behavior is undefined.
    Iterators and references to the erased element are invalidated.
    If the element is the last element in the container, the past-the-end iterator is also invalidated.
    Other references and iterators are not affected.
    */
    CXX14_CONSTEXPR void popFront()
    {
        --m_size;
        (m_data + m_front)->~value_type();
        incIndex(m_front);
    }
       
    private:
    
    CXX14_CONSTEXPR void reallocate(const size_type count)
    {
        if (count != m_capacity)
        {
            // Allocate new memory
            value_type* data = allocate(count);

            // Size of the container after reallocation
            size_type newSize = m_size;
            if (count < newSize)
            {
                newSize = count;
            }
            
            // Copy-construct existing elements
            auto it = cbegin();
            for (size_type idx = 0; idx < newSize; ++idx)
            {
                new (data + idx) value_type(*it);
                ++it;
            }
                    
            // Deallocate old memory
            clear();
            deallocate(m_data);
                    
            // Assign members to new memory
            m_data = data;
            m_capacity = count;
            m_size = newSize;
            m_front = 0;
            m_end = newSize;
        }
    }
    
    constexpr void incIndex(size_type& idx) const
    {
        if (++idx == m_capacity)
        {
            idx = 0;
        }
    }

    constexpr void decIndex(size_type& idx) const
    {
        if (idx == 0)
        {
            idx = m_capacity;
        }
        --idx;
    }

    constexpr bool full() const
    {
        return m_size == m_capacity;
    }
    
    constexpr value_type* allocate(const size_type capacity)
    {
        value_type* ptr = static_cast<value_type*>(m_allocator.allocate(capacity * sizeof(value_type)));
        if (nullptr == ptr)
        {
            throw_bad_alloc();
        }
        return ptr;
    }

    constexpr void deallocate(value_type* ptr)
    {
        m_allocator.deallocate(ptr);
    }

    value_type* m_data = nullptr;
    size_type m_capacity = 0;
    size_type m_size = 0;
    size_type m_front = 0;
    size_type m_end = 0;
    allocator_type m_allocator;
};


#endif