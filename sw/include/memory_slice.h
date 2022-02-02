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

#ifndef MEMORY_SLICE_H
#define MEMORY_SLICE_H

#include <stdint.h>

/**
@brief Driver for SPI EEPROM 25AA512/25LC512
@param SPIMaster Driver class for SPI master implementing a static put() method
@param SSPin Driver class for SS (output) pin
*/
template <typename Memory, uint32_t t_offsetBytes, uint32_t t_lengthBytes>
class MemorySlice
{
    public:
    
    /// @brief Data type for memory address/offset
    typedef typename Memory::Address Address;

    /**
    @brief Get 25LC512 EEPROM capacity in bytes
    @result EEPROM capacity in bytes
    */
    static constexpr uint32_t capacity()
    {
        check();
        return t_lengthBytes;
    }

    /**
    @brief Write one byte to EEPROM at given position
    @param pos Position in EEPROM (0..65535)
    @param data Byte to be written to EEPROM
    */
    static void write(const Address address, const uint8_t data)
    {
        check();
        Memory::write(address + t_offsetBytes, data);
    }

    /**
    @brief Write multiple Bytes to EEPROM starting at given position
    @param pos Position of first byte in EEPROM (0..65535)
    @param data Bytes to be written to EEPROM
    @param nofBytes Number of Bytes to be written to EEPROM (1..65535)
    */
    template <typename Length>
    static void write(const Address address, const uint8_t * data, const Length nofBytes)
    {
        check();
        Memory::write(address + t_offsetBytes, data, nofBytes);
    }

    /**
    @brief Write multiple Bytes to EEPROM starting at given position
    @param pos Position of first byte in EEPROM (0..65535)
    @param data Bytes to be written to EEPROM
    @param nofBytes Number of Bytes to be written to EEPROM (1..65535)
    */
    template <typename Iter>
    static void write(const Address address, const Iter & first, const Iter & last)
    {
        check();
        Memory::write(address + t_offsetBytes, first, last);
    }

    /**
    @brief Write one Byte to EEPROM starting at given position
    @param pos Position of first byte in EEPROM (0..65535)
    @param data Bytes to be written to EEPROM
    @param nofBytes Number of Bytes to be filled on EEPROM (1..65535)
    */
    template <typename Length>
    static void fill(const Address address, const uint8_t data, const Length nofBytes)
    {
        check();
        Memory::fill(address + t_offsetBytes, data, nofBytes);
    }

    /**
    @brief Read one byte from EEPROM from given position
    @param pos Position in EEPROM (0..65535)
    @result data Byte read from EEPROM
    */
    static uint8_t read(const Address address)
    {
        check();
        return Memory::read(address + t_offsetBytes);
    }

    /**
    @brief Read multiple Bytes from EEPROM starting at given position
    @param pos Position of first byte in EEPROM (0..65535)
    @param data Bytes to be read from EEPROM
    @param nofBytes Number of Bytes to be read from EEPROM (1..65535)
    */
    template <typename Length>
    static void read(const Address address, uint8_t * data, const Length nofBytes)
    {
        check();
        Memory::read(address + t_offsetBytes, data, nofBytes);
    }
    
    private:
    
    static constexpr void check()
    {
        static_assert(t_offsetBytes + t_lengthBytes <= Memory::capacity(), "Capacity exceeded");
    }
};

#endif