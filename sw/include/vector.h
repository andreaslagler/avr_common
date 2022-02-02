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

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include "type_traits.h"

/**
@brief Vector container class with runtime length but compile-time capacity (i.e. maximum length)
@note Vector elements are created only by the constructor. When calling resize(), no vector elements are created or deleted.
@tparam Elem Type of vector elements
@tparam Len Type of vector length
@tparam t_capacity Vector capacity in elements
*/
template <typename Elem, typename Len, Len t_capacity>
class Vector
{
    public:
    
    template <typename T>
    class reverse_iterator
    {
        public:
        
        constexpr reverse_iterator(T* ptr) : m_ptr(ptr)
        {}
            
        constexpr T & operator*()
        {
            return *m_ptr;
        }
        
        constexpr reverse_iterator & operator++()
        {
            m_ptr--;
            return *this;
        }
        
        constexpr reverse_iterator & operator++(int)
        {
            m_ptr--;
            return *this;
        }
        
        constexpr bool operator==(const reverse_iterator& rhs) const
        {
            return m_ptr == rhs.m_ptr;
        }      
            
    //constexpr reverse_iterator& operator++();
    //constexpr reverse_iterator& operator--();
    //constexpr reverse_iterator operator--( int );
    //constexpr reverse_iterator operator+( difference_type n ) const;
    //constexpr reverse_iterator operator-( difference_type n ) const;
    //constexpr reverse_iterator& operator+=( difference_type n );
    //constexpr reverse_iterator& operator-=( difference_type n );
        private:        
        
        T* m_ptr {nullptr};
    };
    
    
    /**
    @brief Constructor resizing the vector and setting all elements to a default value
    @param len Length of the vector on construction
    @param elem Default value the vector elements are set to on construction
    */
    constexpr Vector(const Len & len, const Elem elem) : m_len(len)
    {
        set(elem);
    }
    
    /**
    @brief Constructor resizing the vector
    @param length Length of the vector on construction
    */
    constexpr Vector(const Len & len) : m_len(len)
    {}
    
    /**
    @brief Default constructor. Vector is cleared on construction
    */
    constexpr Vector() = default;
    
    template <typename ElemRhs, typename LenRhs, LenRhs capacity>
    Vector<Elem, Len, t_capacity> & operator=(const Vector<ElemRhs, LenRhs, capacity> & rhs)
    requires (t_capacity >= capacity)
    {
        resize(rhs.length());
        auto src = rhs.begin();
        for ( auto dst = begin(); src < rhs.end(); *dst++ = *src++);
        return *this;
    }
    
    /**
    @brief Read/Write access to vector element at given position
    @param idx Zero-based index of element to read
    @result Vector element at given position
    */
    constexpr Elem & operator[](const Len idx)
    {
        return m_data[idx];
    }

    /**
    @brief Read only access to vector element at given position
    @param idx Zero-based index of element to read
    @result Vector element at given position
    */
    constexpr const Elem & operator[](const Len tIdx) const
    {
        return m_data[tIdx];
    }
    
    /**
    @brief Read/write accessible iterator/pointer to first element of vector
    @result Pointer to first vector element
    */
    constexpr Elem * begin()
    {
        return m_data;
    }
    
    /**
    @brief Read only iterator/pointer to first element of vector
    @result Pointer to first vector element
    */
    constexpr const Elem * begin() const
    {
        return m_data;
    }
    
    /**
    @brief Read/write accessible iterator/pointer to last+1 element of vector
    @result Pointer to last+1 vector element
    */
    constexpr Elem * end()
    {
        return begin() + m_len;
    }
    
    /**
    @brief Read only iterator/pointer to last+1 element of vector
    @result Pointer to last+1 vector element
    */
    constexpr const Elem * end() const
    {
        return begin() + m_len;
    }
    
    /**
    @brief Read/write accessible iterator/pointer to first element of vector
    @result Pointer to first vector element
    */
    constexpr reverse_iterator<Elem> rbegin()
    {
        return reverse_iterator<Elem>{m_data+m_len-1};
    }
    
    /**
    @brief Read only iterator/pointer to first element of vector
    @result Pointer to first vector element
    */
    constexpr reverse_iterator<const Elem> rbegin() const
    {
        return reverse_iterator<const Elem>{m_data+m_len-1};
    }
    
    /**
    @brief Read/write accessible iterator/pointer to last+1 element of vector
    @result Pointer to last+1 vector element
    */
    constexpr reverse_iterator<Elem> rend()
    {
        return reverse_iterator<Elem>{m_data-1};
    }
    
    /**
    @brief Read only iterator/pointer to last+1 element of vector
    @result Pointer to last+1 vector element
    */
    constexpr reverse_iterator<const Elem> rend() const
    {
        return reverse_iterator<const Elem>{m_data-1};
    }
    
    /**
    @brief Read only access to vector length
    @result Vector length
    */
    constexpr const Len & length() const
    {
        return m_len;
    }
    
    /**
    @brief Set vector length
    @param length vector length to be set
    @result Flag indicating if resize was successful
    */
    constexpr bool resize(const Len & len)
    {
        if (len <= t_capacity)
        {
            m_len = len;
            return true;
        }
        else
        {
            return false;
        }
    }
    
    /**
    @brief Set all elements to given value
    @ elem Value to set all vector elements to
    */
    constexpr void set(const Elem & elem)
    {
        Elem * data = m_data;
        Len len = m_len;
        while (len--)
        {
            *data++ = elem;
        }
    }

    /**
    @brief Check if vector is empty
    @result Flag indicating if vector is empty
    */
    constexpr bool isEmpty() const
    {
        return (0 == m_len);
    }
    
    /**
    @brief Check if vector is full
    @result Flag indicating if vector is full
    */
    constexpr bool isFull() const
    {
        return (t_capacity == m_len);
    }
    
    /**
    @brief Clear vector, i.e set length to zero
    */
    constexpr void clear()
    {
        resize(0);
    }
    
    protected:
    
    // Encapsulated array with fixed number of elements
    Elem m_data[t_capacity];
    
    // Length of vector, i.e. used number of elements
    Len m_len {0};
};

#endif