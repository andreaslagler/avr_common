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

#ifndef BUFFERED_USART_H
#define BUFFERED_USART_H

#include <stdint.h>
#include <stdbool.h>
#include <static_deque.h>
#include <queue.h>

/**
@brief Driver class for tx-buffered USART using a static Decorator approach
@tparam USART Driver for underlying USART
@tparam t_txBufferSize Tx buffer size
*/
template <typename _USART, uint8_t t_txBufferSize>
class BufferedUSART : _USART
{
    public:
    
    // Expose underlying USART driver
    typedef _USART USART;

    /**
    @brief Callback for UDRE interrupt issuing the transmission of the next byte in the Tx buffer
    */
    static void transmitNextByte() __attribute__((always_inline))
    {
        if (s_txBuffer.empty())
        {
            // Stop USART transmission when Tx buffer runs empty
            USART::stopTransmission();
        }
        else
        {
            // Transmit next data byte
            USART::put(s_txBuffer.front());
            s_txBuffer.pop();
        }
    }

    /**
    @brief Transmit Byte (non-blocking, Tx errors must be handled in the caller's scope)
    @param data Data byte to be transmitted next
    @result Flag indicating the data byte has been enqueued for transmission
    */
    static bool put(const uint8_t data)
    {
        // Queue data in Tx ring buffer
        const bool txOK = s_txBuffer.size() < t_txBufferSize;
        if (txOK)
        {
            s_txBuffer.push(data);
        }

        // Start USART transmission
        USART::startTransmission();

        return txOK;
    }

    // Expose base class methods
    using USART::get;

    private:
    
    // Tx buffer
    static Queue<uint8_t, StaticDeque<uint8_t, t_txBufferSize>> s_txBuffer;
};

// static initialization
template <
typename USART,
uint8_t t_txBufferSize>
Queue<uint8_t, StaticDeque<uint8_t, t_txBufferSize>> BufferedUSART<USART, t_txBufferSize>::s_txBuffer;

#endif