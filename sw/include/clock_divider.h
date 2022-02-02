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

#ifndef CLOCK_DIVIDER_H
#define CLOCK_DIVIDER_H

#include <stdint.h>
#include <stdbool.h>

/**
@brief Template class implementing an integer clock divider
@tparam Counter Integral clock counter type
@tparam t_divider Clock divider factor
*/
template <typename Counter, Counter t_divider>
class ClockDivider
{
    public:
    
    /**
    @brief Constructor
    @param clockInit Initial value for clock counter
    */
    constexpr  ClockDivider(Counter clockInit = t_divider)
    :
    m_clock(clockInit)
    {}
    
    /**
    @brief Increment divider input clock
    @result Flag indicating the divider output is incremented
    */
    constexpr bool clock()
    {
        if (--m_clock == 0)
        {
            reset();
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
    @brief Reset the clock divider
    @param polyphase Reset value of the clock counter
    */
    constexpr void reset(const Counter polyphase = t_divider)
    {
        m_clock = polyphase;
    }
    
    private:
    
    // Clock divider polyphase
    Counter m_clock {t_divider};    
};

#endif