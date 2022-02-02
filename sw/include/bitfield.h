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

#ifndef BITFILED_H
#define BITFILED_H

#include <stdint.h>

/**
@brief Template function setting a given bit in a result of given integral type
@tparam T Integral result type
@param bitIdx Index of bit to be set in result
@result A value of type T with only the bit indicated by bitIdx set
*/
template <typename T>
constexpr T setBit(const uint8_t bitIdx);

/**
@brief Template function setting a given bit in a uint8_t result
@param bitIdx Index of bit to be set in result
@result A uint8_t value with only the bit indicated by bitIdx set
*/
template<>
constexpr uint8_t setBit<uint8_t>(const uint8_t bitIdx)
{
    if (bitIdx < 8)
    {
        return 1 << bitIdx;
    }
    else
    {
        return 0;
    }
}

/**
@brief Template function setting a given bit in a uint16_t result
@param bitIdx Index of bit to be set in result
@result A uint16_t value with only the bit indicated by bitIdx set
*/
template<>
constexpr uint16_t setBit<uint16_t>(const uint8_t bitIdx)
{
    if (bitIdx < 8)
    {
        return 1 << bitIdx;
    }

    if (bitIdx < 16)
    {
        return uint16_t(setBit<uint8_t>(bitIdx>>3)) << 8;
    }

    return 0;
}

#endif