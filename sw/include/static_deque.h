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

#ifndef STATIC_DEQUE_H
#define STATIC_DEQUE_H

#include <bits/c++config.h>
#include <bits/move.h>
#include <bits/new.h>
#include <type_traits>
#include <stdbool.h>
#include <exception.h>
#include <initializer_list>


/**
@brief Template class implementing a double-ended static queue of objects with compile-time fixed capacity
@tparam T Type of deque elements
*/
template <typename T, typename SizeT>
class StaticDequeBase
{
    public:
    
    template <typename U>
    class Iterator;
    
    template <typename U>
    class ReverseIterator;
    
    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = SizeT;
    using difference_type        = ptrdiff_t;
    using iterator               = Iterator<T>;
    using const_iterator         = Iterator<const T>;
    using reverse_iterator       = ReverseIterator<T>;
    using const_reverse_iterator = ReverseIterator<const T>;

    template <typename U>
    class Iterator
    {
        public:
        
        CXX20_CONSTEXPR Iterator(U* const base, const size_type bufferSize, const size_type idx) : m_base(base), m_bufferSize(bufferSize), m_idx(idx)
        {}
        
        CXX20_CONSTEXPR Iterator(const Iterator& rhs) = default;
        
        CXX20_CONSTEXPR Iterator& operator=(const Iterator& rhs)
        {
            m_base = rhs.m_base;
            m_idx = rhs.m_idx;
            return *this;
        }
        
        CXX20_CONSTEXPR ~Iterator() = default;
        
        CXX14_CONSTEXPR Iterator& operator++()
        {
            ++m_idx;
            if (m_idx >= m_bufferSize)
            {
                m_idx -= m_bufferSize;
            }
            return *this;
        }

        constexpr U& operator*() const
        {
            return m_base[m_idx];
        }
        
        constexpr bool operator!=(const Iterator& other)
        {
            return m_idx != other.m_idx;
        }

        protected:
                
        U* m_base = nullptr;
        size_type m_bufferSize = 0;
        size_type m_idx = 0;
    };

    template <typename U>
    class ReverseIterator : public Iterator<U>
    {
        public:
        
        CXX20_CONSTEXPR ReverseIterator(U* const base, const size_type bufferSize, const size_type idx) : Iterator<U>(base, bufferSize, idx)
        {}

        CXX20_CONSTEXPR ReverseIterator(const ReverseIterator& rhs) : Iterator<U>(rhs)
        {}
        
        CXX20_CONSTEXPR ReverseIterator(const Iterator<U>& rhs) : Iterator<U>(rhs)
        {}
                
        CXX20_CONSTEXPR ~ReverseIterator() = default;
        
        CXX14_CONSTEXPR ReverseIterator& operator++()
        {
            --Iterator<U>::m_idx;
            if (Iterator<U>::m_idx >= Iterator<U>::m_bufferSize)
            {
                Iterator<U>::m_idx += Iterator<U>::m_bufferSize;
            }
            return *this;
        }
    };
    
    /**
    @brief Constructor.
    Constructs the container with count copies of elements with value value
    */
    CXX20_CONSTEXPR StaticDequeBase(T* ptr, size_type bufferSize) : m_data(ptr), m_bufferSize(bufferSize)
    {}      
           
    /**
    @brief move constructor.
    Constructs the container with the contents of other using move semantics.
    */
    CXX20_CONSTEXPR StaticDequeBase(StaticDequeBase&& other)
    {
        swap(other);
    }
        
    /**
    @brief Destructor.
    Destructs the deque. The destructors of the elements are called and the used storage is deallocated.
    Note, that if the elements are pointers, the pointed-to objects are not destroyed.
    */
    CXX20_CONSTEXPR ~StaticDequeBase()
    {
        clear();
    }    
        
    /**
    @brief Assignment
    Replaces the contents with count copies of value value
    */
    CXX14_CONSTEXPR void assign(size_type count, const T& value = T())
    {
        clear();
        while (count--)
        {
            pushBack(value);
        }
    }
    
    /**
    @brief Assignment
    Replaces the contents with copies of those in the range [first, last). The behavior is undefined if either argument is an iterator into *this.
    */
    // TODO restrict overload resolution
    template <typename InputIt>
    CXX14_CONSTEXPR void assign(InputIt first, InputIt last)
    {
        clear();
        while (first != last)
        {
            pushBack(*first);
            ++first;
        }
    }

    /**
    @brief Assignment
    Replaces the contents with the elements from the initializer list ilist.
    */
    CXX14_CONSTEXPR void assign(std::initializer_list<T> init)
    {
        assign(init.begin(), init.end());
    }

    /**
    @brief Access specified element
    Returns a reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR reference operator[](const size_type pos)
    {
        return m_data[incIndex(m_frontPos, pos)];
    }
    
    /**
    @brief Access specified element (read-only)
    Returns a const reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR const_reference operator[](const size_type pos) const
    {
        return m_data[incIndex(m_frontPos, pos)];
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
        return m_data[m_frontPos];
    }
    
    /**
    @brief access the first element (read-only)
    Returns a const reference to the frontPos element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR const_reference front() const
    {
        return m_data[m_frontPos];
    }
    
    /**
    @brief Access the last element
    Returns a reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR reference back()
    {
        return m_data[decIndex(m_endPos)];
    }
    
    /**
    @brief Access the last element (read-only)
    Returns a const reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR const_reference back() const
    {
        return m_data[decIndex(m_endPos)];
    }

    /**
    @brief Get const iterator pointing to frontPos element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return const_iterator(m_data, m_bufferSize, m_frontPos);
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator begin() const
    {
        return cbegin();
    }
    
    /**
    @brief Get iterator pointing to frontPos element of queue
    @result Begin iterator
    */
    CXX14_CONSTEXPR iterator begin()
    {
        return iterator(m_data, m_bufferSize, m_frontPos);
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return const_iterator(m_data, m_bufferSize, m_endPos);
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
        return iterator(m_data, m_bufferSize, m_endPos);
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(m_data, m_bufferSize, decIndex(m_endPos));
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator rbegin() const
    {
        return crbegin();
    }
    
    /**
    @brief Get iterator pointing to frontPos element of queue
    @result Begin iterator
    */
    CXX14_CONSTEXPR reverse_iterator rbegin()
    {
        return reverse_iterator(m_data, m_bufferSize, decIndex(m_endPos));
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crend() const
    {
        return const_reverse_iterator(m_data, m_bufferSize, decIndex(m_frontPos));
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rend() const
    {
        return crend();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue
    @result End iterator
    */
    CXX14_CONSTEXPR reverse_iterator rend()
    {
        return reverse_iterator(m_data, m_bufferSize, decIndex(m_frontPos));
    }

    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end()
    @result true if the container is empty, false otherwise
    */
    constexpr bool empty() const
    {
        return (m_frontPos == m_endPos);
    }

    /**
    @brief Returns the number of elements
    Returns the number of elements in the container
    @result The number of elements in the container.
    */
    constexpr size_type size() const
    {
        size_type nofElements = m_endPos - m_frontPos;
        if (nofElements > m_bufferSize)
        {
            nofElements += m_bufferSize;
        }
        return nofElements;
    }

    /**
    @brief Clears the contents
    Erases all elements from the container. After this call, size() returns zero.
    Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-end iterators are also invalidated.
    */
    CXX14_CONSTEXPR void clear()
    {
        while (!empty())
        {
            popFront();
        }
        m_frontPos = 0;
        m_endPos = 0;
    }

   /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(const T & value)
    {
        if (full())
        {
            throw_length_error();
        }
        
        const size_type endPos = m_endPos;
        m_endPos = incIndex(endPos);
        new (m_data + endPos) T(value);
    }

    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(T&& value)
    {
        if (full())
        {
            throw_length_error();
        }
        
        const size_type endPos = m_endPos;
        m_endPos = incIndex(endPos);
        new (m_data + endPos) T(forward<T>(value));
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
            throw_length_error();
        }

        const size_type endPos = m_endPos;
        m_endPos = incIndex(endPos);
        return *new (m_data + endPos) T(forward<Args>(args)...);
    }

    /**
    @brief Removes the last element
    */
    CXX14_CONSTEXPR void popBack()
    {
        if (!empty())
        {
            const size_type endPos = decIndex(m_endPos);
            (m_data + endPos)->~T();
            m_endPos = endPos;
        }
    }
            
    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    All iterators, including the past-the-end iterator, are invalidated. No references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(const T & value)
    {
        if (full())
        {
            throw_length_error();
        }
        
        const size_type frontPos =  decIndex(m_frontPos);
        m_frontPos = frontPos;
        new (m_data + frontPos) T(value);
    }

    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    All iterators, including the past-the-end iterator, are invalidated. No references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(T&& value)
    {
        if (full())
        {
            throw_length_error();
        }
        
        const size_type frontPos = decIndex(m_frontPos);
        m_frontPos = frontPos;
        new (m_data + frontPos) T(forward<T>(value));
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
            throw_length_error();
        }

        const size_type frontPos = decIndex(m_frontPos);
        m_frontPos = frontPos;
        return *new (m_data + frontPos) T(forward<Args>(args)...);
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
        if (!empty())
        {
            const size_type frontPos = m_frontPos;
            (m_data + frontPos)->~T();
            m_frontPos = incIndex(frontPos);
        }        
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
            throw_length_error();
        }
        
        while (count < size())
        {
            popBack();
        }
        
        while (count > size())
        {
            emplaceBack();
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
            throw_length_error();
        }
        
        while (count < size())
        {
            popBack();
        }
        
        while (count > size())
        {
            emplaceBack(value);
        }
    }
    
    protected:
    
    /**
    @brief Swaps the contents
    Exchanges the contents of the container with those of other.
    All iterators and references remain valid. The past-the-end iterator is invalidated.
    */
    CXX14_CONSTEXPR void swap(StaticDequeBase& other)
    {
        ::swap(m_data, other.m_data);
        ::swap(m_bufferSize, other.m_bufferSize);
        ::swap(m_frontPos, other.m_frontPos);
        ::swap(m_endPos, other.m_endPos);
    }

    private:

    constexpr bool full() const
    {
        return (m_frontPos == incIndex(m_endPos));
    }
    
    // Increment and roll over index
    constexpr size_type incIndex(size_type index, const size_type increment = 1) const 
    {
        index += increment;
        while (index >= m_bufferSize)
        {
            index -= m_bufferSize;
        }
        return index;
    }

    // Decrement and roll over index
    constexpr size_type decIndex(size_type index, const size_type decrement = 1) const
    {
        index -= decrement;
        while (index >= m_bufferSize)
        {
            index +=m_bufferSize;
        }
        return index;
    }

    constexpr size_type capacity() const
    {
        return m_bufferSize-1;
    }

    T* m_data;
    size_type m_bufferSize;
    size_type m_frontPos = 0;
    size_type m_endPos = 0;
};

template <typename T, size_t t_capacity>
class StaticDeque : public StaticDequeBase<T, typename DownCast<t_capacity+1>::type>
{
    using size_type = typename DownCast<t_capacity+1>::type;
    static constexpr size_type s_bufferSize = t_capacity + 1;
        
    public:
    
    /**
    @brief Default constructor.
    Constructs an empty container
    */
    CXX20_CONSTEXPR StaticDeque() : StaticDequeBase<T, size_type>(reinterpret_cast<T*>(&m_data[0]), s_bufferSize)
    {}
    
    /**
    @brief Constructor.
    Constructs the container with count copies of elements with value value
    */
    CXX20_CONSTEXPR StaticDeque(size_type count, const T& value = T()) : StaticDequeBase<T, size_type>(reinterpret_cast<T*>(&m_data[0]), s_bufferSize)
    {
        StaticDequeBase<T, size_type>::assign(count, value);
    }
    
    /**
    @brief Constructor.
    Constructs the container with the contents of the range [first, last)
    */
    // TODO restrict overload resolution
    template <typename InputIt>
    CXX20_CONSTEXPR StaticDeque(InputIt first, InputIt last) : StaticDequeBase<T, size_type>(reinterpret_cast<T*>(&m_data[0]), s_bufferSize)
    {
        StaticDequeBase<T, size_type>::assign(first, last);
    }
    
    /**
    @brief Copy constructor.
    Constructs the container with the copy of the contents of other
    */
    CXX20_CONSTEXPR StaticDeque(const StaticDeque& other) : StaticDeque(other.begin(), other.end())
    {}
    
    /**
    @brief move constructor.
    Constructs the container with the contents of other using move semantics.
    */
    CXX20_CONSTEXPR StaticDeque(StaticDeque&& other) : StaticDequeBase<T, size_type>(reinterpret_cast<T*>(&m_data[0]), s_bufferSize)
    {
        swap(other);
    }
    
    /**
    @brief Copy constructor.
    Constructs the container with the contents of the initializer list init
    */
    CXX20_CONSTEXPR StaticDeque(std::initializer_list<T> init) : StaticDeque(init.begin(), init.end())
    {}    
    
       /**
    @brief Copy assignment operator
    Replaces the contents with a copy of the contents of other.
    */
    CXX14_CONSTEXPR StaticDeque& operator=(const StaticDeque& other)
    {
        for (const T& t : other)
        {
            StaticDequeBase<T, size_type>::pushBack(t);
        }
        
        return *this;
    }

    /**
    @brief Move assignment operator
    Replaces the contents with those of other using move semantics (i.e. the data in other is moved from other into this container).
    */
    constexpr StaticDeque& operator=(StaticDeque&& other)
    {
        swap(other);
        return *this;
    }

    /**
    @brief Copy assignment operator
    Replaces the contents with those identified by initializer list ilist.
    */
    CXX14_CONSTEXPR StaticDeque& operator=(std::initializer_list<T> ilist)
    {
        StaticDequeBase<T, size_type>::assign(ilist);
        return *this;
    }
    
    /**
    @brief Returns the maximum possible number of elements
    Returns the maximum number of elements the container is able to hold due to system or library implementation limitations
    @result Maximum number of elements.
    */
    constexpr typename DownCast<t_capacity+1>::type max_size() const
    {
        return DownCast<t_capacity+1>::value-1;
    }

    /**
    @brief Swaps the contents
    Exchanges the contents of the container with those of other.
    All iterators and references remain valid. The past-the-end iterator is invalidated.
    */
    CXX14_CONSTEXPR void swap(StaticDeque& other)
    {
        //::swap(m_allocator, other.m_allocator);
        StaticDequeBase<T, size_type>::swap(other);
    }


    private:
    
    char m_data[s_bufferSize * sizeof(T)];
};


#endif