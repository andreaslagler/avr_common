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

#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <bits/c++config.h>
#include <bits/move.h>
#include <allocator.h>


// TODO memcpy strLen should move to cstring
CXX14_CONSTEXPR void* memcpy(void* dest, const void* src, size_t count)
{
    // Convert the source and destination pointers to uint8_t*.
    const auto src_bytes = static_cast<const uint8_t*>(src);
    auto dest_bytes = static_cast<uint8_t*>(dest);

    // Copy the bytes from the source to the destination.
    for (size_t i = 0; i < count; ++i)
    {
        dest_bytes[i] = src_bytes[i];
    }

    // Return the destination pointer.
    return dest;
}


CXX14_CONSTEXPR size_t strLen(const char* str)
{
    // Initialize the length to zero.
    size_t length = 0;

    // Loop over the string until we find a null character.
    while (str[length] != '\0')
    {
        ++length;
    }

    // Return the length of the string.
    return length;
}



/**
@brief A light-weight string class with customizable allocator.
@tparam Allocator The allocator used to allocate and deallocate memory.
*/
template<typename Allocator = HeapAllocator<>>
class String
{
    public:
    
    using size_type = typename Allocator::size_type;
    using iterator = char*;
    using const_iterator = const char*;
    
    /**
    @brief Default constructor. Constructs an empty string.
    */
    constexpr String(const Allocator& = Allocator())
    {}

    /**
    @brief Constructor. Constructs a string with the given content.
    @param str The content of the string.
    */
    CXX14_CONSTEXPR String(const char* str, const Allocator& = Allocator())
    {
        assign(str);
    }

    /**
    @brief Constructor. Constructs a string with the given content.
    @param str The content of the string.
    @param len The length of the content.
    */
    CXX14_CONSTEXPR String(const char* str, size_t len, const Allocator& = Allocator())
    {
        assign(str, len);
    }

    /**
    @brief Copy constructor. Constructs a string with the same content as the given string.
    @param other The other string.
    */
    CXX14_CONSTEXPR String(const String& other, const Allocator& = Allocator())
    {
        assign(other.data(), other.size());
    }

    /**
    @brief Move constructor. Constructs a string by moving the content of the given string.
    @param other The other string.
    */
    CXX14_CONSTEXPR String(String&& other)
    {
        swap(m_data, other.m_data);
        swap(m_size, other.m_size);
        swap(m_capacity, other.m_capacity);
        m_allocator = move(other.m_allocator);
    }

    /**
    @brief Destructor. Destroys the string and deallocates its memory.
    */
    CXX20_CONSTEXPR ~String()
    {
        clear();
    }

    /**
    @brief Assignment operator. Assigns the content of the given string to this string.
    @param other The other string.
    @return String& A reference to this string.
    */
    CXX14_CONSTEXPR String& operator=(const String& other)
    {
        if (this != &other)
        {
            clear();
            assign(other.data(), other.size());
        }
        return *this;
    }

    /**
    @brief Move-assignment operator. Assigns the content of the given string by moving it to this string.
    @param other The other string.
    @return String& A reference to this string.
    */
    CXX14_CONSTEXPR String& operator=(String&& other)
    {
        if (this != &other)
        {
        swap(m_data, other.m_data);
        swap(m_size, other.m_size);
        swap(m_capacity, other.m_capacity);
        m_allocator = move(other.m_allocator);
        }
        return *this;
    }

    /**
    @brief Assigns the given content to this string.
    @param str The content to assign.
    */
    CXX14_CONSTEXPR void assign(const char* str)
    {
        assign(str, strLen(str));
    }

    /**
    @brief Assigns the given content to this string.
    @param str The content to assign.
    @param len The length of the content.
    */
    CXX14_CONSTEXPR void assign(const char* str, size_t len)
    {
        reserve(len);
        memcpy(m_data, str, len);
        m_size = len;
    }

    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return m_data;
    }

    CXX14_CONSTEXPR const_iterator begin() const
    {
        return cbegin();
    }

    CXX14_CONSTEXPR iterator begin()
    {
        return m_data;
    }

    CXX14_CONSTEXPR const_iterator cend() const
    {
        return m_data + m_size;
    }

    CXX14_CONSTEXPR const_iterator end() const
    {
        return cend();
    }

    CXX14_CONSTEXPR iterator end()
    {
        return m_data + m_size;
    }

    /**
    @brief Appends the given content to this string.
    @param str The content to append.
    */
    CXX14_CONSTEXPR void append(const char* str)
    {
        append(str, strLen(str));
    }

    /**
    @brief Appends the given content to this string.
    @param str The content to append.
    @param len The length of the content.
    */
    CXX14_CONSTEXPR void append(const char* str, size_t len)
    {
        reserve(m_size + len);
        memcpy(m_data + m_size, str, len);
        m_size += len;
    }

    /**
    @brief Appends the given character to this string.
    @param c The character to append.
    */
    CXX14_CONSTEXPR void pushBack(const char c)
    {
        if (m_size == m_capacity)
        {
            reserve(2 * m_capacity + 1);
        }        
        m_data[m_size++] = c;
    }
    
    /**
    @brief Clears the content of this string.
    */
    CXX14_CONSTEXPR void clear()
    {
        m_allocator.deallocate(m_data);
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    /**
    @brief Returns the length of this string.
    @return size_t The length of this string.
    */
    constexpr bool empty() const
    {
        return m_size == 0;
    }

    /**
    @brief Returns the length of this string.
    @return size_t The length of this string.
    */
    constexpr size_t size() const
    {
        return m_size;
    }

    /**
    @brief Returns the capacity of this string.
    @return size_t The capacity of this string.
    */
    constexpr size_t capacity() const
    {
        return m_capacity;
    }

    /**
    @brief Returns the content of this string as a C-style string.
    @return The content of this string as a C-style string.
    */
    constexpr const char* c_str() const
    {
        // Append null terminator
        reserve(m_size + 1);
        m_data[m_size] = '\0';
        return m_data;
    }

    private:
    
    /**
    @brief Reserves the given capacity for this string.
    @param cap The capacity to reserve.
    */
    void reserve(size_t cap)
    {
        if (cap > m_capacity)
        {
            const size_t new_cap = cap;
            char* const new_data = reinterpret_cast<char*>(m_allocator.allocate(new_cap));
            if (new_data != nullptr)
            {
                memcpy(new_data, m_data, m_size);
                m_allocator.deallocate(m_data);
                m_data = new_data;
                m_capacity = new_cap;
            }
        }
    }

    char* m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;
    Allocator m_allocator = Allocator();
};


#endif