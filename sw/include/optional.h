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

#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <stdint.h>
#include <stdbool.h>

template <typename T>
class Optional
{
    public:
    
    constexpr Optional()
    :
    m_dummy(0),
    m_hasValue(false)
    {}
    
    Optional(const T & value)
    :
    m_value(value),
    m_hasValue(true)
    {}
    
    ~Optional()
    {
        if (m_hasValue)
        {
            m_value.~T();
        }
    }
    
    constexpr T & operator*()
    {
        return m_value;
    }
    
    constexpr const T & operator*() const
    {
        return m_value;
    }
    
    constexpr explicit operator bool() const
    {
        return m_hasValue;
    }
    
    private:
    
     // char is the data type with the smallest possible memory consumption
     // All data types are at least the same size as char, so this unnamed union does not cause any memory overhead.
    union
    {
        char m_dummy;
        T m_value;
    };
    
    bool m_hasValue;
};

#endif