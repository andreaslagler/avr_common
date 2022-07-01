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

#ifndef DIV_H
#define DIV_H

/**
@brief Unsigned 8 bit integer division by a compile-time constant using a "multiplication and bit-shift" approach.
@tparam t_denominator Denominator
@param numerator Numerator
@result Numerator / Denominator
*/
template <uint8_t t_denominator>
constexpr uint8_t div(const uint8_t numerator)
{
    return numerator / t_denominator;
}

/**
@brief Unsigned 8 bit integer division by 1, trivial.
@param numerator Numerator
@result Numerator / 1
*/
template <>
constexpr uint8_t div<1>(const uint8_t numerator)
{
    return numerator;
}

/**
@brief Unsigned 8 bit integer division by 2, trivial.
@param numerator Numerator
@result Numerator / 2
*/
template <>
constexpr uint8_t div<2>(const uint8_t numerator)
{
    return numerator >> 1;
}

/**
@brief Unsigned 8 bit integer division by 4, trivial.
@param numerator Numerator
@result Numerator / 4
*/
template <>
constexpr uint8_t div<4>(const uint8_t numerator)
{
    return numerator >> 2;
}

/**
@brief Unsigned 8 bit integer division by 6.
@param numerator Numerator
@result (Numerator * 171) / 1024
*/
template <>
constexpr uint8_t div<6>(const uint8_t numerator)
{
    // Multiplication by 171/1024 doesn't cause any round-off errors for uint8_t
    return (uint8_t((171 * numerator) >> 8)) >> 2;
}

/**
@brief Unsigned 8 bit integer division by 8, trivial.
@param numerator Numerator
@result Numerator / 8
*/
template <>
constexpr uint8_t div<8>(const uint8_t numerator)
{
    return numerator >> 3;
}

/**
@brief Unsigned 8 bit integer division by 10.
@param numerator Numerator
@result (Numerator * 205) / 2048
*/
template <>
constexpr uint8_t div<10>(const uint8_t numerator)
{
    // Multiplication by 205/2048 doesn't cause any round-off errors for uint8_t
    return (uint8_t((205 * numerator) >> 8)) >> 3;
}

/**
@brief Unsigned 8 bit integer division by 12.
@param numerator Numerator
@result (Numerator * 171) / 2048
*/
template <>
constexpr uint8_t div<12>(const uint8_t numerator)
{
    // Multiplication by 171/2048 doesn't cause any round-off errors for uint8_t
    return (uint8_t((171 * numerator) >> 8)) >> 3;
}


/**
@brief Unsigned 8 bit integer division by 100.
@param numerator Numerator
@result (Numerator * 41) / 4096
*/
template <>
constexpr uint8_t div<100>(const uint8_t numerator)
{
    // Multiplication by 41/4096 doesn't cause any round-off errors for uint8_t
    return (uint8_t((41 * numerator) >> 8)) >> 4;
}

/**
@brief Unsigned 8 bit integer modulo division by a compile-time constant.
@tparam t_denominator Denominator
@param numerator Numerator, will be overwritten by the division result Numerator / Denominator
@result Numerator % Denominator
*/
template <uint8_t t_denominator>
constexpr uint8_t mod(uint8_t & numerator)
{
    const uint8_t remainder = numerator;
    numerator = div<t_denominator>(numerator); // Write-back result
    return remainder - numerator * t_denominator;
}

#endif