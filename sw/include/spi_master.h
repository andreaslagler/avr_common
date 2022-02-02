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

#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include <stdint.h>

/**
@brief Implementation of driver for SPI master using a given SPI module driver
@tparam SPIModule SPI module driver
@note Data is transferred synchronously, i.e. driver is actively waiting for SPI transfer to complete
*/
template<typename SPIModule>
class SPIMasterSync
{
    public:
    
    /// Slave Select Pin
    typedef typename SPIModule::SS_Pin SS_Pin;

    /**
    @brief Initialization of the SPI module in master mode
    @param clockRate SPI clock rate
    @param dataOrder SPI data order (MSB/LSB first), default is MSB first
    @param clockPolarity SPI Clock polarity according to SPI mode 00/01/10/11, default is mode 00
    @param clockPhase SPI Clock phase according to SPI mode 00/01/10/11, default is mode 00
    */
    static void init(
    const typename SPIModule::ClockRate clockRate,
    const typename SPIModule::DataOrder dataOrder,
    const typename SPIModule::ClockPolarity clockPolarity,
    const typename SPIModule::ClockPhase clockPhase)
    {
        // Init SPI module in master mode
        SPIModule::initMasterMode();
        
        // Set SPI data order
        SPIModule::setDataOrder(dataOrder);
        
        // Set SPI clock phase
        SPIModule::setClockPhase(clockPhase);
        
        // Set SPI clock polarity
        SPIModule::setClockPolarity(clockPolarity);
        
        // Set SPI clock rate
        SPIModule::setClockRate(clockRate);
        
        // Disable SPI interrupt for synchronous operation
        SPIModule::disableInterrupt();
        
        // Enable SPI module
        SPIModule::enable();
    }

    /**
    @brief Transmit a single byte
    @param data Byte to be transmitted
    @note This method compiles into ~5 assembly instructions, so it should always be inlined
    */
    static void put(const uint8_t data) __attribute__((always_inline))
    {
        // Transmit data
        SPIModule::transmit(data);

        // Wait while SPI is busy
        SPIModule::wait();
    }

    /**
    @brief Transmit multiple bytes
    @param data Pointer to Bytes to be transmitted
    @param nofBytes Number of Bytes to be transmitted
    */
    static void put(const uint8_t * data, uint8_t nofBytes)
    {
        if (0 == nofBytes)
        {
            return;
        }

        // Transmission pipeline
        do
        {
            // Cache data into register so it can be sent immediately after the bus is ready
            const uint8_t cachedData = *data++;
            
            // Wait while SPI is busy
            SPIModule::wait();
            
            // Transmit data
            SPIModule::transmit(cachedData);
        }
        while (--nofBytes);

        // Wait while SPI is busy
        SPIModule::wait();
    }

    /**
    @brief Receive single byte
    @param dummy Dummy Byte to be transmitted, default is 0x00
    @result Received Byte
    @note This method compiles into ~5 assembly instructions, so it should always be inlined
    */
    static uint8_t get(const uint8_t dummy = 0) __attribute__((always_inline))
    {
        // Transmit dummy data
        SPIModule::transmit(dummy);

        // Wait while SPI is busy
        SPIModule::wait();

        // Receive data
        return SPIModule::receive();
    }

    /**
    @brief Receive multiple bytes
    */
    static void get(uint8_t * data, uint8_t nofBytes, const uint8_t dummy = 0)
    {
        if (0 == nofBytes)
        {
            return;
        }

        // Dummy write to initiate first data transfer
        SPIModule::transmit(dummy);

        // Reception pipeline
        while (--nofBytes)
        {
            // Wait while SPI is busy
            SPIModule::wait();

            // Receive and cache data
            const uint8_t cachedData = SPIModule::receive();

            // Dummy write to initiate next data transfer
            SPIModule::transmit(dummy);

            // Copy cached data to receive buffer
            *data++ = cachedData;
        }

        // Wait while SPI is busy
        SPIModule::wait();

        // Receive and store last byte
        *data = SPIModule::receive();
    }
};

#endif