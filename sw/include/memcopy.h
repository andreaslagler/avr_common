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

#ifndef MEMCOPY_H
#define MEMCOPY_H

#include <stdint.h>
#include <avr/pgmspace.h>

/**
@brief Copy memory for a given number of elements of given type from RAM to RAM.
Replacement for the avr-libc implementation. Depending on the data type and length, the compiler will unroll the for-loop and generate smaller code compared to the call into avr-libc
@tparam Elem Type of the elements to be copied
@tparam Len Integral length type
@param dst Destination pointer
@param src Source pointer
@param len length in Elem
*/
template <typename Elem, typename Len = uint8_t>
inline constexpr void memcopy(Elem * dst, const Elem * src, Len len)
{
    const uint8_t * uint8Src = reinterpret_cast<const uint8_t*>(src);
    uint8_t * uint8Dst = reinterpret_cast<uint8_t*>(dst);

    while (len--)
    {
        for (uint8_t cnt = 0; cnt < sizeof(Elem); ++cnt)
        {
            *(uint8Dst++) = *(uint8Src++);
        }
    }
}

/**
@brief Copy memory for a given number of elements of given type from PROGMEM to RAM.
@tparam Elem Type of the elements to be copied
@tparam Len Integral length type
@param dst Destination pointer
@param src Source pointer
@param len length in Elem
*/
template <typename Elem, typename Len = uint8_t>
inline void memcopy_P(Elem * dst, const Elem * src, Len len)
{
    const uint8_t * uint8Src = reinterpret_cast<const uint8_t*>(src);
    uint8_t * uint8Dst = reinterpret_cast<uint8_t*>(dst);

    while (len--)
    {
        for (uint8_t cnt = 0; cnt < sizeof(Elem); ++cnt)
        {
            *(uint8Dst++) = pgm_read_byte(uint8Src++);
        }
    }
}

/**
@brief Copy memory for one element of given type from PROGMEM to RAM.
@tparam Elem Type of the elements to be copied
@param dst Destination pointer
@param src Source pointer
*/
template <typename Elem>
inline void memcopy_P(Elem * dst, const Elem * src)
{
    const uint8_t * uint8Src = reinterpret_cast<const uint8_t*>(src);
    uint8_t * uint8Dst = reinterpret_cast<uint8_t*>(dst);

    for (uint8_t cnt = 0; cnt < sizeof(Elem); ++cnt)
    {
        *(uint8Dst++) = pgm_read_byte(uint8Src++);
    }
}

/**
@brief Read memory for one element of given type from PROGMEM.
@tparam Elem Type of the elements to be copied
@param src Source pointer
@result Element read from PROGMEM
*/
template <typename Elem>
inline Elem memread_P(const Elem * src)
{
    const uint8_t * uint8Src = reinterpret_cast<const uint8_t*>(src);
    Elem dst;
    uint8_t * uint8Dst = reinterpret_cast<uint8_t*>(&dst);

    for (uint8_t cnt = 0; cnt < sizeof(Elem); ++cnt)
    {
        *(uint8Dst++) = pgm_read_byte(uint8Src++);
    }

    return dst;
}

/**
@brief Read one uint8_t from PROGMEM.
@param src Source pointer
@result Element read from PROGMEM
*/
template <>
inline uint8_t memread_P<uint8_t>(const uint8_t * src)
{
    return pgm_read_byte(src);
}

/**
@brief Read one int8_t from PROGMEM.
@param src Source pointer
@result Element read from PROGMEM
*/
template <>
inline int8_t memread_P<int8_t>(const int8_t * src)
{
    return pgm_read_byte(src);
}

#endif