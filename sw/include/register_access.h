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

#ifndef AVR_SFR_H
#define AVR_SFR_H

#include <avr/io.h>
#include <stdint.h>


/**
@brief Template class for accessing a memory-mapped I/O register specified by a data type and address
@tparam RegisterType data type represented by the I/O register
@tparam t_registerAddress Address of the I/O register
*/
template<typename RegisterType, uint16_t t_registerAddress>
struct MMIORegister
{
    /**
    @brief Data type represented by memory-mapped I/O register
    */
    typedef RegisterType Type;

    /**
    @brief Write memory-mapped I/O register
    @param value Value to be written to register
    */
    static void write(const RegisterType & value) __attribute__((always_inline))
    {
        *(volatile RegisterType *)(t_registerAddress) = value;
    }

    /**
    @brief Read memory-mapped I/O register
    @result Register content
    */
    static RegisterType read() __attribute__((always_inline))
    {
        return *(volatile RegisterType *)(t_registerAddress);
    }
};

// All 8-Bit SFRs are defined by the macro _MMIO_BYTE in file sfr_defs.h of selected avr-gcc toolchain to allow for simple C-style memory-mapped register access
// --> Redefine _MMIO_BYTE so it uses the C++ class defined above
#undef _MMIO_BYTE
#define _MMIO_BYTE(address) MMIORegister<uint8_t, address>

// All 16-Bit SFRs are defined by the macro _MMIO_WORD in file sfr_defs.h of selected avr-gcc toolchain to allow for simple C-style memory-mapped register access
// --> Redefine _MMIO_WORD so it uses the C++ class defined above
#undef _MMIO_WORD
#define _MMIO_WORD(address) MMIORegister<uint16_t, address>

// All 32-Bit SFRs are defined by the macro _MMIO_DWORD in file sfr_defs.h of selected avr-gcc toolchain to allow for simple C-style memory-mapped register access
// --> Redefine _MMIO_DWORD so it uses the C++ class defined above
#undef _MMIO_DWORD
#define _MMIO_DWORD(address) MMIORegister<uint32_t, address>


/**
@brief Template class implementing access to a group of bits of a (special function) register.
The bit group is considered as a value of type Type.
@tparam Register Memory mapped address of the 8 bit (special function) register
@tparam t_firstBit First bit of the bit group
@tparam t_lastBit Last bit of the bit group
@tparam Type Type used to interpret the bit group, e.g. an enum
*/
template <
typename Register,
uint8_t t_firstBit,
uint8_t t_lastBit,
typename Type = typename Register::Type>
class BitGroupInRegister
{
    public:
    
    /**
    @brief Read register bit group
    @result Register bit group content
    */
    static Type read()
    {
        check();
        return static_cast<Type>((Register::read() & s_bitMask) >> t_firstBit);
    }
    
    /**
    @brief Write register bit group
    @param value Value to be written to register bit group
    */
    static void write(const Type value)
    {
        check();
        Register::write((Register::read() & ~s_bitMask) | ((static_cast<uint8_t>(value) << t_firstBit) & s_bitMask));
    }
    
    private:
    
    // Plausibility check
    static constexpr void check()
    {
        static_assert(t_firstBit >= 0, "Invalid configuration: First bit index < 0!");
        static_assert(t_lastBit <= (sizeof(Type) * 8 - 1), "Invalid configuration: Last bit index out of range!");
        static_assert(t_firstBit <= t_lastBit, "Invalid configuration: First bit index > last bit index!");
    }
    
    static constexpr typename Register::Type s_bitMask = ((1<<(t_lastBit - t_firstBit + 1)) - 1) << t_firstBit;
};

/**
@brief Template class implementing access to a single bit of a (special function) register.
@tparam Register Encapsulated (special function) register
@tparam t_bit Selected bit
*/
template <
typename Register,
uint8_t t_bit>
class BitInRegister
{
    public:
    
    /**
    @brief Read selected register bit
    @result Content of selected register bit
    */
    static bool read() __attribute__((always_inline))
    {
        check();
        return Register::read() & _BV(t_bit);
    }
    
    /**
    @brief Set selected register bit
    */
    static void set() __attribute__((always_inline))
    {
        check();
        Register::write(Register::read() | _BV(t_bit));
    }
    
    /**
    @brief Clear selected register bit
    */
    static void clear() __attribute__((always_inline))
    {
        check();
        Register::write(Register::read() & ~_BV(t_bit));
    }
    
    /**
    @brief Write selected register bit
    @param Flag indicating if the selected register bit should be set
    */
    static void write(const bool value)
    {
        check();
        
        //auto registerValue = Register::read();
        //
        //asm volatile (
        //"bst %[value], 0                   ; Store LSB of boolean value in T flag \n"
        //"bld %[registerValue], %[bitIndex] ; Load T flag into register value \n"
        //: [registerValue] "+r" (registerValue)
        //: [value] "r" ((uint8_t)(value)), [bitIndex] "i" (t_bit)
        //:
        //);
        //
        //Register::write(registerValue);
        
        if (value)
        {
            set();
        }
        else
        {
            clear();
        }
    }
    
    private:
    
    // Plausibility check
    static constexpr void check()
    {
        static_assert(t_bit >= 0, "Invalid configuration: Bit index < 0!");
        static_assert(t_bit <= (sizeof(typename Register::Type) * 8 - 1), "Invalid configuration: Bit index out of range!");
    }
};

#endif