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

template <uint8_t t_size>
class String
{
    public:
    
    static constexpr uint8_t size()
    {
        return t_size;
    }
    
    constexpr char & operator[](const uint8_t idx)
    {
        return m_data[idx];
    }
    
    constexpr char operator[](const uint8_t idx) const
    {
        return m_data[idx];
    }
    
    constexpr String<t_size>& operator=(const String<t_size>& rhs)
    {
        memcopy(m_data, rhs.m_data, t_size);
    }
    
    constexpr String()
    {}
    
    constexpr String(const String<t_size>& rhs)
    {
        memcopy(m_data, rhs.m_data, t_size);
    }
    
    constexpr const char* begin() const
    {
        return m_data;
    }
    
    constexpr char* begin()
    {
        return m_data;
    }
    
    constexpr const char* end() const
    {
        return m_data + t_size;
    }
    
    constexpr char* end()
    {
        return m_data + t_size;
    }
    
    private:
    
    char m_data[t_size];
};


#endif