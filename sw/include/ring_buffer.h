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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "type_traits.h"

/**
@brief Template class implementing a size 2^N ring buffer
@tparam Elem Type of queue elements
@tparam t_lengthPower2 Length of the ring buffer as a power of 2, i.e. the buffer size will be 2^t_lengthPower2
@tparam t_volatile Flag indicating if read and write position should be volatile
@note Because of the missing support of std::new in avr-gcc, this queue implementation is not self-contained and does not store copies of the node elements. SingleLinkedNode<Elem> has to be a public base class of Elem.
*/
template <
typename Elem,
uint8_t t_lengthPower2,
bool t_volatile = false>
class RingBuffer
{
    public:
    
    /// @brief Constructor
    constexpr RingBuffer() = default;
    
    /**
    @brief Write element to the current write position
    @param elem Element to be written
    @result Flag indicating if the element has been written successfully
    */
    constexpr bool write(const Elem & elem)
    {
        // Check if buffer is full
        const uint8_t writePos = m_writePos;
        const uint8_t nextWritePos = incIndex(writePos);
        if (nextWritePos == m_readPos)
        {
            return false;
        }

        // Write data
        m_buffer[writePos] = elem;

        // Write back write index
        m_writePos = nextWritePos;

        return true;
    }

    /**
    @brief Read element from the current read position
    @param elem Buffer for the read element
    @result Flag indicating if an element has been read successfully
    @note If the ring buffer is empty, the data in elem will not be changed
    */
    constexpr bool read(Elem & elem)
    {
        // Cache read position
        const uint8_t readPos = m_readPos;

        // Check if buffer is empty
        if (readPos == m_writePos)
        {
            return false;
        }

        // Read data
        elem = m_buffer[readPos];

        // Read, increment and write back read index
        m_readPos = incIndex(readPos);

        return true;
    }

    private:
    
    // Ring buffer size 1/2/4/8/16/32/64/128 elements
    static constexpr uint8_t getBufferSize()
    {
        return (1 << t_lengthPower2);
    }

    // Bit mask to utilize 2^N overflow when incrementing read and write positions
    static constexpr uint8_t getIndexBitMask()
    {
        return getBufferSize() - 1;
    }

    // Read position
    typename MakeVolatile<uint8_t, t_volatile>::type m_readPos {0};
    
    // Write position
    typename MakeVolatile<uint8_t, t_volatile>::type m_writePos {0};
    
    // Buffer (size 2^N)
    Elem m_buffer[getBufferSize()];
    
    // Increment and roll over index
    static constexpr uint8_t incIndex(const uint8_t idx)
    {
        return (idx + 1) & getIndexBitMask();
    }
};

#endif