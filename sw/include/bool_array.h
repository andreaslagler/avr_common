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

#ifndef BOOL_VECTOR_H
#define BOOL_VECTOR_H

#include <stdint.h>
#include <array.h>


template <uint8_t t_length>
class BoolArray
{
    public:
    
    constexpr BoolArray(const bool value = false)
    {
        if (value)
        {
            setAll();
        }
        else
        {
            clearAll();
        }
    }
    
    constexpr void setAll()
    {
        for (uint8_t & data : m_data)
        {
            data = 0xff;
        }
    }
    
    constexpr void clearAll()
    {
        for (uint8_t & data : m_data)
        {
            data = 0;
        }
    }
    
    constexpr void set(const uint8_t idx, const bool value = true)
    {
        const uint8_t intIdx = idx >> 3;
        const uint8_t fracIdx = idx & 0b111;
        const uint8_t bitmask = 1 << fracIdx;
        if (value)
        m_data[intIdx] |= bitmask;
        else
        m_data[intIdx] &= ~bitmask;        
    }
    
    constexpr void clear(const uint8_t idx)
    {
        const uint8_t intIdx = idx >> 3;
        const uint8_t fracIdx = idx & 0b111;
        m_data[intIdx] &= ~(1 << fracIdx);
    }
    
    constexpr bool toggle(const uint8_t idx)
    {
        const uint8_t intIdx = idx >> 3;
        const uint8_t fracIdx = idx & 0b111;
        return m_data[intIdx] ^= (1 << fracIdx);
    }
    
    constexpr bool operator[] (const uint8_t idx) const
    {
        const uint8_t intIdx = idx >> 3;
        const uint8_t fracIdx = idx & 0b111;
        return m_data[intIdx] & (1 << fracIdx);
    }
    
    constexpr auto& data()
    {
        return m_data;
    }
    
    constexpr const auto& data() const
    {
        return m_data;
    }    
    
    private:
    
    static consteval uint8_t getNofBytes()
    {
        return ((t_length+7) >> 3);
    }
    
    Array<uint8_t, getNofBytes()> m_data;
};

#endif