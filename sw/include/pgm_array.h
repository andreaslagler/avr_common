/*typedef
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

#ifndef PGM_ARRAY_H
#define PGM_ARRAY_H

#include <bits/c++config.h>

#if SINCE_CXX11

#include <stddef.h> // ptrdiff_t
#include <avr/pgmspace.h>
#include "exception.h"
#include "algorithm.h" // swap()

/**
@brief Contiguous array of type const T stored in program memory 
*/
template <typename T>
class PgmArray
{
    public:
    
    class ConstIterator
    {
        public:
        
        constexpr ConstIterator(const T * const ptr)
        :
        m_ptr(ptr)
        {}
        
        constexpr ~ConstIterator() = default;
        
        constexpr ConstIterator& operator++()
        {
            ++m_ptr;
            return  *this;
        }
        
        constexpr T operator*() const
        {
            return read(m_ptr);
        }
        
        constexpr bool operator==(const ConstIterator& other)
        {
            return m_ptr == other.m_ptr;
        }
        
        constexpr bool operator!=(const ConstIterator& other)
        {
            return m_ptr != other.m_ptr;
        }
        
        private:
        
        const T* m_ptr = nullptr;
    };

    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using const_iterator         = ConstIterator;

    /**
    @brief constructs new PgmArray
    Copy constructor is implicitly declared (until C++11) defaulted, and is constexpr
    */
    constexpr PgmArray(const PgmArray& other) = default;

    /**
    @brief constructs new PgmArray
    Move constructor is defaulted, and is constexpr
    */
    constexpr PgmArray(PgmArray&& other) = default;
    
    /**
    @brief destructs a PgmArray
    Data is stored in program memory and does not need to (or rather cannot) be deallocated
    */
    constexpr ~PgmArray() = default;
    
    /**
    @brief assigns the contents
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    */
    CXX20_CONSTEXPR PgmArray& operator=(const PgmArray& other)
    {
        m_data = other.m_data;
        m_size = other.m_size;
        return *this;
    }
    
    /**
    @brief Access specified element (read-only)
    Returns a copy of the element at specified location pos. No bounds checking is performed.
    @param pos Position of the element to return
    @result Copy of the requested element.
    */
    CXX14_CONSTEXPR value_type operator[](const size_type pos) const
    {
        return read(m_data + pos);
    }
    
    /**
    @brief Access specified element (read-only) with bounds checking
    Returns a copy of the element at specified location pos, with bounds checking.
    If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
    @param pos Position of the element to return
    @result Copy of the requested element.
    */
    CXX14_CONSTEXPR value_type at(const size_type pos) const
    {
        if (!(pos < size()))
        {
            throw_out_of_range();
        }
        
        return read(m_data + pos);
    }
    
    /**
    @brief Access the first element (read-only)
    Returns a const reference to the first element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR value_type front() const
    {
        return read(m_data);
    }
    
    /**
    @brief Access the last element (read-only)
    Returns a const reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR value_type back() const
    {
        return read(m_data + size()-1);
    }
    
    /**
    @brief Get const iterator pointing to first character of string
    @result begin const iterator
    */
    CXX17_CONSTEXPR const_iterator cbegin() const
    {
        return m_data; // implicit conversion to const_iterator
    }
    
    /**
    @brief Get const iterator pointing to first character of string
    @result begin const iterator
    */
    CXX17_CONSTEXPR const_iterator begin() const
    {
        return m_data; // implicit conversion to const_iterator
    }
    
    /**
    @brief Get const iterator pointing to last plus one character of string
    @result End const iterator
    */
    CXX17_CONSTEXPR const_iterator cend() const
    {
        return m_data + size(); // implicit conversion to const_iterator
    }
    
    /**
    @brief Get const iterator pointing to last plus one character of string
    @result End const iterator
    */
    CXX17_CONSTEXPR const_iterator end() const
    {
        return m_data + size(); // implicit conversion to const_iterator
    }
    
    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end().
    @result true if the container is empty, false otherwise
    */
    constexpr bool empty() const
    {
        return 0 == size();
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
    @brief Returns the maximum possible number of elements
    Returns the maximum number of elements the container is able to hold due to system or library implementation limitations
    @result Number of characters
    @note Because each Array<T, N> is a fixed-size container, the value returned by max_size equals N (which is also the value returned by size)
    */
    constexpr size_type max_size() const
    {
        return m_size;
    }

    /**
    @brief Swaps the contents
    Exchanges the contents of the container with those of other. Does not cause iterators and references to associate with the other container.
    @param other Container to exchange the contents with
    */
    CXX20_CONSTEXPR void swap(PgmArray& other)
    {
        ::swap(m_data, other.m_data);
        ::swap(m_size, other.m_size);
    }

    private:
    
    // PgmArray objects should only be generated by the function makePgmArray()
    // --> Declare makePgmArray() as a friend
    // --> Declare constructor private
    template <typename U, U ... t_data>
    friend constexpr PgmArray<U> makePgmArray();
    
    constexpr PgmArray(const T* data, const size_t size) : m_data(data), m_size(size)
    {}

    const T * m_data = nullptr;
    size_t m_size = 0;
    
    static T read(const T * ptr)
    {
        const uint8_t * uint8Src = reinterpret_cast<const uint8_t*>(ptr);
        T ret;
        uint8_t * uint8Ret = reinterpret_cast<uint8_t*>(&ret);

        for (uint8_t cnt = 0; cnt < sizeof(T); ++cnt)
        {
            *(uint8Ret++) = pgm_read_byte(uint8Src++);
        }

        return ret;
    }

    
    
    // Nested container to store an array in PROGMEM
    template <T ... t_data>
    class PgmArrayStorage
    {
        public:
        
        PgmArrayStorage() = delete;

        static constexpr const T * data()
        {
            return s_data;
        }

        static constexpr size_t size()
        {
            return sizeof...(t_data);
        }

        private:

        // Compile-time constant string stored in PROGMEM
        static constexpr const T s_data[] PROGMEM = {t_data...};
    };
};

// Static initialization
template <typename T>
template <T ... t_data>
constexpr const T PgmArray<T>::PgmArrayStorage<t_data...>::s_data[] PROGMEM;

/**
@brief
@tparam T
@tparam t_data
*/
template <typename T, T ... t_data>
constexpr PgmArray<T> makePgmArray()
{
    return PgmArray<T>(
    PgmArray<T>::template PgmArrayStorage<t_data ...>::data(),
    PgmArray<T>::template PgmArrayStorage<t_data ...>::size());
}

/**
@brief Exchanges the given values.
Specializes the swap algorithm for Array. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
*/
template <typename T>
CXX20_CONSTEXPR void swap(PgmArray<T>& lhs, PgmArray<T>& rhs)
{
    lhs.swap(rhs);
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the arrays are equal, false otherwise
*/
template <typename T>
CXX20_CONSTEXPR bool operator==(const PgmArray<T>& lhs, const PgmArray<T>& rhs)
{
    if (lhs.size() == rhs.size())
    {
        return equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    else
    {
        return false;
    }
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the arrays are not equal, false otherwise
*/
template <typename T>
CXX20_CONSTEXPR bool operator!=(const PgmArray<T>& lhs, const PgmArray<T>& rhs)
{
    if (lhs.size() == rhs.size())
    {
        return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a != b;});
    }
    else
    {
        return false;
    }
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically less than the contents of rhs, false otherwise
*/
template <typename T>
CXX20_CONSTEXPR bool operator<(const PgmArray<T>& lhs, const PgmArray<T>& rhs)
{
    if (lhs.size() == rhs.size())
    {
        return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a < b;});
    }
    else
    {
        return false;
    }
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically less than or equal to the contents of rhs, false otherwise
*/
template <typename T>
CXX20_CONSTEXPR bool operator<=(const PgmArray<T>& lhs, const PgmArray<T>& rhs)
{
    if (lhs.size() == rhs.size())
    {
        return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a <= b;});
    }
    else
    {
        return false;
    }
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically greater than the contents of rhs, false otherwise
*/
template <typename T>
CXX20_CONSTEXPR bool operator>(const PgmArray<T>& lhs, const PgmArray<T>& rhs)
{
    if (lhs.size() == rhs.size())
    {
        return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a > b;});
    }
    else
    {
        return false;
    }
}

/**
@brief Compares the contents of two arrays.
@result true if the contents of the lhs are lexicographically greater than or equal to the contents of rhs, false otherwise
*/
template <typename T>
CXX20_CONSTEXPR bool operator>=(const PgmArray<T>& lhs, const PgmArray<T>& rhs)
{
    if (lhs.size() == rhs.size())
    {
        return equal(lhs.begin(), lhs.end(), rhs.begin(), [](const T& a, const T& b){return a >= b;});
    }
    else
    {
        return false;
    }
}

#endif
#endif
