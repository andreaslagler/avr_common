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

#include <stdint.h>

/**
@brief String class with fixed length
@tparam t_size Number of characters in string
*/
template <uint8_t t_size>
class String
{
    public:
    
    /**
    @brief Get number of characters
    @result Number of characters
    */ 
    static constexpr uint8_t size()
    {
        return t_size;
    }
    
    /**
    @brief Buffer access
    @param idx Index (zero-based)
    @result Reference to character at position idx 
    */ 
    constexpr char & operator[](const uint8_t idx)
    {
        return m_data[idx];
    }
    
    /**
    @brief Buffer access (read-only)
    @param idx Index (zero-based)
    @result Character at position (idx)
    */ 
    constexpr char operator[](const uint8_t idx) const
    {
        return m_data[idx];
    }
    
    /**
    @brief Copy assignment
    @param rhs String object to copy
    @result Reference to this object
    */ 
    constexpr String<t_size>& operator=(const String<t_size>& rhs)
    {
        memcopy(m_data, rhs.m_data, t_size);
    }
    
    /**
    @brief Default constructor
    */ 
    constexpr String() = default;
    
    /**
    @brief Copy constructor
    @param rhs String object to copy-construct
    */ 
    constexpr String(const String<t_size>& rhs)
    {
        memcopy(m_data, rhs.m_data, t_size);
    }
    
    /**
    @brief Get const iterator pointing to first character of string
    @result begin const iterator
    */ 
    constexpr const char* begin() const
    {
        return m_data;
    }
    
    /**
    @brief Get iterator pointing to first character of string
    @result Begin iterator
    */ 
    constexpr char* begin()
    {
        return m_data;
    }
    
    /**
    @brief Get const iterator pointing to last plus one character of string
    @result End const iterator
    */ 
    constexpr const char* end() const
    {
        return m_data + t_size;
    }
    
    /**
    @brief Get iterator pointing to last plus one character of string
    @result End iterator
    */ 
    constexpr char* end()
    {
        return m_data + t_size;
    }
    
    private:
    
    char m_data[t_size];
};


#endif