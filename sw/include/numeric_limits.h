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

#ifndef NUMERIC_LIMITS_H
#define NUMERIC_LIMITS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
@brief Numeric limits of a given type
@tparam T Type for which numeric limits are provided
*/
template <typename T>
struct numeric_limits;

/**
@brief Numeric limits of uint8_t
*/
template <>
struct numeric_limits <uint8_t>
{
    /**
    @brief Maximum value
    @result Maximum value of uint8_t
    */
    static constexpr uint8_t max()
    {
        return 255U;
    }
    
    /**
    @brief Minimum value
    @result Minimum value of uint8_t
    */
    static constexpr uint8_t min()
    {
        return 0;
    }
};

/**
@brief Numeric limits of uint16_t
*/
template <>
struct numeric_limits <uint16_t>
{
    /**
    @brief Maximum value
    @result Maximum value of uint16_t
    */
    static constexpr uint16_t max()
    {
        return 65535U;
    }

    /**
    @brief Minimum value
    @result Minimum value of uint16_t
    */
    static constexpr uint16_t min()
    {
        return 0;
    }
};

#endif