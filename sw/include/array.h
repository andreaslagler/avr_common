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

#ifndef ARRAY_H
#define ARRAY_H

#include <bits/c++config.h>

#if SINCE_CXX11

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // ptrdiff_t
#include "exception.h"
#include "algorithm.h" // swap(), fill()
#include <type_traits> // DownCast

/**
@brief Static contiguous array
Array is a container that encapsulates fixed size arrays.
This container is an aggregate type with the same semantics as a struct holding a C-style array T[N] as its only non-static data member.
Unlike a C-style array, it doesn't decay to T* automatically. As an aggregate type, it can be initialized with aggregate-initialization
given at most N initializers that are convertible to T: Array<int, 3> a = {1,2,3};.
The struct combines the performance and accessibility of a C-style array with the benefits of a standard container, such as knowing its own size, supporting assignment, random access iterators, etc.
*/
template <typename T, size_t t_size>
struct Array
{
    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = typename DownCast<t_size>::type;
    using difference_type        = ptrdiff_t;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    //using reverse_iterator       = std::reverse_iterator<iterator>;
    //using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    /**
    @brief Access specified element
    Returns a reference to the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX17_CONSTEXPR reference operator[](const size_type pos)
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
    If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX17_CONSTEXPR reference at(const size_type pos)
    {
        if (!(pos < size()))
        {
            throw_out_of_range();
        }
        
        return m_data[pos];
    }
    
    /**
    @brief Access specified element (read-only) with bounds checking
    Returns a const reference to the element at specified location pos, with bounds checking.
    If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
    @param pos Position of the element to return
    @result Reference to the requested element.
    */
    CXX14_CONSTEXPR const_reference at(const size_type pos) const
    {
        if (!(pos < size()))
        {
            throw_out_of_range();
        }
        
        return m_data[pos];
    }
    
    /**
    @brief Access the first element
    Returns a reference to the first element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX17_CONSTEXPR reference front()
    {
        return m_data[0];
    }
    
    /**
    @brief Access the first element (read-only)
    Returns a const reference to the first element in the container.
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
    CXX17_CONSTEXPR reference back()
    {
        return m_data[size()-1];
    }
    
    /**
    @brief Access the last element (read-only)
    Returns a const reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR const_reference back() const
    {
        return m_data[size()-1];
    }
    
    /**
    @brief Direct access to the underlying array
    Returns pointer to the underlying array serving as element storage. The pointer is such that range [data();
    data()+size()) is always a valid range, even if the container is empty (data() is not dereferenceable in that case).
    @result Pointer to the underlying element storage. For non-empty containers, the returned pointer compares equal to the address of the first element.
    */
    CXX17_CONSTEXPR pointer data()
    {
        return m_data;
    }
    
    /**
    @brief Direct access to the underlying array (read-only)
    Returns pointer to the underlying array serving as element storage. The pointer is such that range [data();
    data()+size()) is always a valid range, even if the container is empty (data() is not dereferenceable in that case).
    @result Pointer to the underlying element storage. For non-empty containers, the returned pointer compares equal to the address of the first element.
    */
    CXX14_CONSTEXPR const_pointer data() const
    {
        return m_data;
    }
    
    /**
    @brief Get const iterator pointing to first character of string
    @result begin const iterator
    */
    CXX17_CONSTEXPR const_iterator cbegin() const
    {
        return m_data;
    }
    
    /**
    @brief Get const iterator pointing to first character of string
    @result begin const iterator
    */
    CXX17_CONSTEXPR const_iterator begin() const
    {
        return m_data;
    }
    
    /**
    @brief Get iterator pointing to first character of string
    @result Begin iterator
    */
    CXX17_CONSTEXPR iterator begin()
    {
        return m_data;
    }
    
    /**
    @brief Get const iterator pointing to last plus one character of string
    @result End const iterator
    */
    CXX17_CONSTEXPR const_iterator cend() const
    {
        return m_data + t_size;
    }
    
    /**
    @brief Get const iterator pointing to last plus one character of string
    @result End const iterator
    */
    CXX17_CONSTEXPR const_iterator end() const
    {
        return m_data + t_size;
    }
    
    /**
    @brief Get iterator pointing to last plus one character of string
    @result End iterator
    */
    CXX17_CONSTEXPR iterator end()
    {
        return m_data + t_size;
    }
    
    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end().
    @result true if the container is empty, false otherwise
    */
    static constexpr bool empty()
    {
        return 0 == size();
    }
    
    /**
    @brief Returns the number of elements
    Returns the number of elements in the container
    @result The number of elements in the container.
    */
    static constexpr size_type size()
    {
        return DownCast<t_size>::value;
    }

    /**
    @brief Returns the maximum possible number of elements
    Returns the maximum number of elements the container is able to hold due to system or library implementation limitations
    @result Number of characters
    @note Because each Array<T, N> is a fixed-size container, the value returned by max_size equals N (which is also the value returned by size)
    */
    static constexpr size_type max_size()
    {
        return DownCast<t_size>::value;
    }

    /**
    @brief Fill the container with specified value
    Assigns the value to all elements in the container
    @param value The value to assign to the elements
    */
    CXX20_CONSTEXPR void fill(const_reference value)
    {
        for (value_type & data : m_data)
        {
            data = value;
        }
    }

    /**
    @brief Swaps the contents
    Exchanges the contents of the container with those of other. Does not cause iterators and references to associate with the other container.
    @param other Container to exchange the contents with
    */
    CXX20_CONSTEXPR void swap(Array& other)
    {
        ::swap(m_data, other.m_data);
    }

    value_type m_data[DownCast<t_size>::value];
};


/**
@brief Extracts the Ith element element from the array.
I must be an integer value in range [0, N). This is enforced at compile time as opposed to at() or operator[].
@param a Array whose contents to extract
@result A reference to the Ith element of a.
*/
template <size_t I, class T, size_t N>
CXX14_CONSTEXPR T& get(Array<T,N>& a)
{
    static_assert(I < N, "Index out of range");
    return a[DownCast<I>::value];
}

/**
@brief Extracts the Ith element element from the array.
I must be an integer value in range [0, N). This is enforced at compile time as opposed to at() or operator[].
@param a Array whose contents to extract
@result A reference to the Ith element of a.
*/
template <size_t I, class T, size_t N>
CXX14_CONSTEXPR T&& get(Array<T,N>&& a)
{
    static_assert(I < N, "Index out of range");
    return a[DownCast<I>::value];
}

/**
@brief Extracts the Ith element element from the array.
I must be an integer value in range [0, N). This is enforced at compile time as opposed to at() or operator[].
@param a Array whose contents to extract
@result A const reference to the Ith element of a.
*/
template <size_t I, class T, size_t N>
CXX14_CONSTEXPR const T& get(const Array<T,N>& a)
{
    static_assert(I < N, "Index out of range");
    return a[DownCast<I>::value];
}


/**
@brief Extracts the Ith element element from the array.
I must be an integer value in range [0, N). This is enforced at compile time as opposed to at() or operator[].
@param a Array whose contents to extract
@result A const reference to the Ith element of a.
*/
template <size_t I, class T, size_t N>
CXX14_CONSTEXPR const T&& get(const Array<T,N>&& a)
{
    static_assert(I < N, "Index out of range");
    return a[DownCast<I>::value];
}

/**
@brief Exchanges the given values.
Specializes the swap algorithm for Array. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
*/
template <class T, size_t N>
CXX20_CONSTEXPR void swap(Array<T,N>& lhs, Array<T,N>& rhs)
{
    lhs.swap(rhs);
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the arrays are equal, false otherwise
*/
template <class T, size_t N>
CXX20_CONSTEXPR bool operator==(const Array<T,N>& lhs, const Array<T,N>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the arrays are not equal, false otherwise
*/
template <class T, size_t N>
CXX20_CONSTEXPR bool operator!=(const Array<T,N>& lhs, const Array<T,N>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a != b;});
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically less than the contents of rhs, false otherwise
*/
template <class T, size_t N>
CXX20_CONSTEXPR bool operator<(const Array<T,N>& lhs, const Array<T,N>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a < b;});
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically less than or equal to the contents of rhs, false otherwise
*/
template <class T, size_t N>
CXX20_CONSTEXPR bool operator<=(const Array<T,N>& lhs, const Array<T,N>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a <= b;});
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically greater than the contents of rhs, false otherwise
*/
template <class T, size_t N>
CXX20_CONSTEXPR bool operator>(const Array<T,N>& lhs, const Array<T,N>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a > b;});
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically greater than or equal to the contents of rhs, false otherwise
*/
template <class T, size_t N>
CXX20_CONSTEXPR bool operator>=(const Array<T,N>& lhs, const Array<T,N>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a >= b;});
}

#endif
#endif