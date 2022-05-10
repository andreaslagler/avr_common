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

#ifndef UI_EDITABLE_STRING_H
#define UI_EDITABLE_STRING_H

#include "string.h"
#include "pgm_string.h"
#include "param.h"

#include <stdint.h>

template <uint8_t t_size>
class UIEditableString
{
    public:
    
    constexpr void clear()
    {
        for (auto & currentChar : m_currentChar)
        {
            currentChar = 0;
        }
    }
    
    constexpr void init(const String<t_size>& string)
    {
        resetCurrentCharIdx();
        for (uint8_t cnt = 0; cnt < t_size; ++cnt)
        {
            m_currentChar[cnt] = charToIdx(string[cnt]);
        }
    }
    
    constexpr void resetCurrentCharIdx()
    {
        m_currentCharIdx = 0;
    }
    
    constexpr void reset()
    {
        clear();
        resetCurrentCharIdx();
    }
    
    constexpr bool toggleCurrentCharIdx()
    {
        return m_currentCharIdx.incrementRollover(0, t_size-1).getValue() == 0;
    }
    
    constexpr uint8_t getCurrentCharacterIdx() const
    {
        return m_currentCharIdx;
    }
    
    constexpr void incCurrentChar()
    {
        m_currentChar[m_currentCharIdx].increment(getNofValidChars()-1);
    }
    
    constexpr void decCurrentChar()
    {
        m_currentChar[m_currentCharIdx].decrement(0);
    }
    
    constexpr String<t_size> toString() const
    {
        String<t_size> string;
        toString(string);
        return string;
    }
    
    constexpr void toString(String<t_size>& string) const
    {
        for (uint8_t cnt = 0; cnt < t_size; ++cnt)
        {
            string[cnt] = idxToChar(m_currentChar[cnt]);
        }
    }
    
    private:
    
    Param<uint8_t> m_currentCharIdx {0};
    Param<uint8_t> m_currentChar[t_size] {0};
    
    static char idxToChar(const uint8_t idx)
    {
        const char * valid_chars = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!?"_pgm;
        return pgm_read_byte(valid_chars+idx);
    }

    static uint8_t charToIdx(const char character)
    {
        for (uint8_t idx = 0; idx < getNofValidChars(); ++idx)
        {
            if (idxToChar(idx) == character)
            {
                return idx;
            };
        }
        
        return 0;
    }

    static constexpr uint8_t getNofValidChars()
    {
        return 65; // 1 + 26 + 26 + 10 + 2 valid characters
    }
};

#endif