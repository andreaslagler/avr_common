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

#ifndef STATIC_VECTOR_H
#define STATIC_VECTOR_H

#include <bits/c++config.h>
#include <bits/move.h>
#include <bits/new.h>
#include <type_traits.h>
#include <exception.h>

#include <initializer_list>
#include <stdbool.h>

/**
@brief Template class implementing a static vector of objects
@tparam T Type of vector elements
*/
template <typename T, size_t t_capacity>
class StaticVector
{
    public:
    
    template <bool t_const, bool t_reverse>
    class Iterator;
    
    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = Iterator<false, false>;
    using const_iterator         = Iterator<true, false>;
    using reverse_iterator       = Iterator<false, true>;
    using const_reverse_iterator = Iterator<true, true>;

    template <bool t_const, bool t_reverse>
    class Iterator
    {
        friend class StaticVector<value_type, t_capacity>;
        
        CXX20_CONSTEXPR Iterator(typename conditional<t_const, const value_type, value_type>::type* data, const size_type idx) : m_data(data), m_idx(idx)
        {}
        
        public:
        
        CXX20_CONSTEXPR Iterator(const Iterator& rhs) = default;
        
        CXX20_CONSTEXPR Iterator& operator=(const Iterator& rhs)
        {
            m_data = rhs.m_data;
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
            return m_data[m_idx];
        }
        
        constexpr bool operator!=(const Iterator& other) const
        {
            return m_idx != other.m_idx;
        }

        private:

        typename conditional<t_const, const value_type, value_type>::type* m_data = nullptr;
        size_type m_idx = 0;
    };    
    
     /**
    @brief Constructor.
    Constructs an empty container with the given allocator allocator
    */
    CXX20_CONSTEXPR explicit StaticVector() = default;    
    /**
    @brief Constructor.
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    */
    CXX20_CONSTEXPR explicit StaticVector(const size_type count)
    {
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        m_size = count;
        for (size_type idx = 0; idx < count; ++idx)
        {
            new (m_data + idx) value_type;
        }
    }
    
    /**
    @brief Constructor.
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    @param value the value to initialize elements of the container with
    */
    CXX20_CONSTEXPR explicit StaticVector(const size_type count, const value_type& value)
    {
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        m_size = count;
        for (size_type idx = 0; idx < count; ++idx)
        {
            new (m_data + idx) value_type(value);
        }
    }
    
    /**
    @brief Constructor.
    Constructs the container with the contents of the range [first, last).
    @param first, last the range to copy the elements from
    */
    template<class InputIt>
    CXX20_CONSTEXPR StaticVector(InputIt first, InputIt last)
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
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        m_size = count;
        for (size_type idx = 0; idx < count; ++idx)
        {
            new (m_data + idx) value_type(*first);
            ++first;
        }
    }
    
    /**
    @brief move constructor.
    Copy constructor. Constructs the container with the copy of the contents of other.
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX20_CONSTEXPR StaticVector(const StaticVector& other)
    {
        const size_type count = other.size();
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        m_size = count;
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
    CXX20_CONSTEXPR StaticVector(StaticVector&& other)
    {
        const size_type count = other.size();
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        m_size = count;
        value_type* ptr = m_data;
        for (value_type& value : other)
        {
            new (ptr++) value_type(move(value));
        }
    }
    
    /**
    @brief Initializing constructor
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param init initializer list to initialize the elements of the container with
    */
    CXX20_CONSTEXPR StaticVector(std::initializer_list<value_type> init)
    {
        const size_type count = init.size();
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        m_size = count;
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
    CXX20_CONSTEXPR ~StaticVector()
    {
        clear();
    }
    
    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR StaticVector& operator=(const StaticVector& other)
    {
        if (this != &other)
        {
            const size_type count = other.size();
            if (count > capacity())
            {
                throw_bad_alloc();
            }
            clear();
            m_size = count;
            value_type* ptr = m_data;
            for (const value_type& value : other)
            {
                new (ptr++) value_type(value);
            }
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Move assignment operator. Replaces the contents with those of other using move semantics (i.e. the data in other is moved from other into this container).
    other is in a valid but unspecified state afterwards.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR StaticVector& operator=(StaticVector&& other)
    {
        if (this != &other)
        {
            const size_type count = other.size();
            if (count > capacity())
            {
                throw_bad_alloc();
            }
            clear();
            m_size = count;
            value_type* ptr = m_data;
            for (value_type& value : other)
            {
                new (ptr++) value_type(move(value));
            }
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Replaces the contents with those identified by initializer list init.
    @param init initializer list to use as data source
    */
    CXX14_CONSTEXPR StaticVector& operator=(std::initializer_list<value_type> init)
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
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        clear();
        m_size = count;
        for (size_type idx = 0; idx < count; ++idx)
        {
            new (m_data + idx) value_type(value);
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
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        clear();
        m_size = count;
        for (size_type idx = 0; idx < count; ++idx)
        {
            new (m_data + idx) value_type(*first);
            ++first;
        }
    }

    /**
    @brief Assignment
    Replaces the contents with the elements from the initializer list init.
    @param init initializer list to copy the values from
    */
    CXX14_CONSTEXPR void assign(std::initializer_list<T> init)
    {
        const size_type count = init.size();
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        clear();        
        m_size = count;
        auto first = init.begin();
        for (size_type idx = 0; idx < count; ++idx)
        {
            new (m_data + idx) value_type(*first);
            ++first;
        }
    }

    /**
    @brief Get const iterator pointing first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return const_iterator(m_data, 0);
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
        return iterator(m_data, 0);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return const_iterator(m_data, m_size);
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
        return iterator(m_data, m_size);
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(m_data, m_size-1);
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
        return reverse_iterator(m_data, m_size-1);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crend() const
    {
        return const_reverse_iterator(m_data, -1);
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
        return reverse_iterator(m_data, -1);
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
    @brief Returns the number of elements
    Returns the number of elements in the container
    @result The number of elements in the container.
    */
    constexpr size_type capacity() const
    {
        return t_capacity;
    }

    /**
    @brief Access specified element
    Returns a reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR reference operator[](const size_type pos)
    {
        return m_data[pos];
    }
    
    /**
    @brief Access specified element (read-only)
    Returns a const reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR const_reference operator[](const size_type pos) const
    {
        return m_data[pos];
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
        return m_data[0];
    }
    
    /**
    @brief access the first element (read-only)
    Returns a const reference to the frontPos element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR const_reference front() const
    {
        return m_data[0];
    }
    
    /**
    @brief Access the last element
    Returns a reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR reference back()
    {
        return m_data[m_size-1];
    }
    
    /**
    @brief Access the last element (read-only)
    Returns a const reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR const_reference back() const
    {
        return m_data[m_size-1];
    }

    /**
    @brief direct access to the underlying array
    Returns pointer to the underlying array serving as element storage.
    The pointer is such that range [data(), data() + size()) is always a valid range, even if the container is empty
    (data() is not dereferenceable in that case).
    @result Pointer to the underlying element storage. For non-empty containers, the returned pointer compares equal to the address of the first element.
    */
    CXX14_CONSTEXPR value_type* data()
    {
        return m_data;
    }
    
    /**
    @brief direct access to the underlying array
    Returns pointer to the underlying array serving as element storage.
    The pointer is such that range [data(), data() + size()) is always a valid range, even if the container is empty
    (data() is not dereferenceable in that case).
    @result Pointer to the underlying element storage. For non-empty containers, the returned pointer compares equal to the address of the first element.
    */
    CXX14_CONSTEXPR const value_type* data() const
    {
        return m_data;
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
        if (count > capacity())
        {
            throw_bad_alloc();
        }
        
        // Add new elements until count is reached
        while (m_size < count)
        {
            new (m_data + m_size) value_type;
            ++m_size;
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
        if (count > capacity())
        {
            throw_bad_alloc();
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
        m_size = 0;
    }

    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(const value_type& value)
    {
        if (size() == capacity())
        {
            throw_bad_alloc();
        }
        
        new (m_data + m_size) value_type(value);
        ++m_size;
    }

    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(value_type&& value)
    {
        if (size() == capacity())
        {
            throw_bad_alloc();
        }
        
        new (m_data + m_size) value_type(forward<value_type>(value));
        ++m_size;
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
        if (size() == capacity())
        {
            throw_bad_alloc();
        }
        
        value_type* newElem = new (m_data + m_size) value_type(forward<Args>(args)...);
        ++m_size;
        return *newElem;
    }

    /**
    @brief Removes the last element
    */
    CXX14_CONSTEXPR void popBack()
    {
        --m_size;
        (m_data + m_size)->~value_type();
    }
       
    private:

    uint8_t m_buffer[t_capacity][sizeof(value_type)];
    value_type* m_data = reinterpret_cast<value_type*>(m_buffer);
    size_type m_size = 0;
};

#endif