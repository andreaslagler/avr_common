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

#ifndef SPI_SLAVE_ASYNC_H
#define SPI_SLAVE_ASYNC_H

#include <stdint.h>

/**
@brief Implementation of driver for asynchronous SPI slave using a given SPI module driver
@tparam SPIModule SPI module driver
*/
template<typename _SPIModule>
class SPISlaveAsync
{
    public:
    
    typedef _SPIModule SPIModule;
    
    /// Slave Select Pin
    typedef typename SPIModule::SS_Pin SS_Pin;

    /**
    @brief Initialization of the SPI module in slave mode with interrupt enabled
    @param dataOrder SPI data order (MSB/LSB first), default is MSB first
    @param clockPolarity SPI Clock polarity according to SPI mode 00/01/10/11, default is mode 00
    @param clockPhase SPI Clock phase according to SPI mode 00/01/10/11, default is mode 00
    */
    static void init(
    const typename SPIModule::DataOrder dataOrder,
    const typename SPIModule::ClockPolarity clockPolarity,
    const typename SPIModule::ClockPhase clockPhase)
    {
        // Init SPI module in slave mode
        SPIModule::initSlaveMode();
        
        // Set SPI data order
        SPIModule::setDataOrder(dataOrder);
        
        // Set SPI clock phase
        SPIModule::setClockPhase(clockPhase);
        
        // Set SPI clock polarity
        SPIModule::setClockPolarity(clockPolarity);
        
        // Enable SPI interrupt for asynchronous operation
        SPIModule::enableInterrupt();
        
        // Enable SPI module
        SPIModule::enable();
    }

    /**
    @brief Transmit a single byte
    @param data Byte to be transmitted next
    */
    static void put(const uint8_t data) __attribute__((always_inline))
    {
        // Transmit data
        SPIModule::transmit(data);
    }

    /**
    @brief Receive single byte
    @result Received Byte
    */
    static uint8_t get() __attribute__((always_inline))
    {
        // Receive data
        return SPIModule::receive();
    }
};

#endif