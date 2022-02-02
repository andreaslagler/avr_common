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

#ifndef PGM_STRING_H
#define PGM_STRING_H

#include <avr/pgmspace.h>

/**
@brief Variadic template class implementing a string literal stored in PROGMEM using an avr-gcc extension as described here: https://www.mikrocontroller.net/topic/410966
@tparam Char This parameter is needed for the parameter pack to work
@tparam t_string The actual string literal passed as a parameter pack
*/
template <typename Char, Char ... t_string>
class PgmString
{
    public:
    
    /**
    @brief Get pointer to string
    @result Pointer to encapsulated string
    */
    static constexpr const char* str()
    {
        return &data[0];
    }

    private:

    // Compile-time constant string stored in PROGMEM
    static constexpr const char data[] PROGMEM = {t_string ..., '\0'};
};

// Static initialization
template <typename Char, Char ... t_string>
constexpr const char PgmString<Char, t_string...>::data[] PROGMEM;

/**
@brief Postfix operator converting a string literal stored in RAM into a string literal stored in PROGMEM
@tparam Char This parameter is needed for the parameter pack to work
@tparam t_string The actual string literal passed as a parameter pack
*/
template <typename Char, Char ... t_string>
constexpr const char * operator "" _pgm()
{
    return PgmString<Char, t_string...>::str();
}

#endif
