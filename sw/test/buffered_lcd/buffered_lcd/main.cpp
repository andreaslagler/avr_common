/*
Copyright (C) 2020 Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

// #ifdef AVR_ATmega1284P

// Hardware-specific configuration

// ATMega1284P is running at 20 MHz
#define F_CPU 20000000UL

#include "m1284p_GPIO.h"
#include "m1284p_SPI.h"

using SPI = m1284p::SPI;

// Multiplexer for SPI Slave Select pin hooked to PD7:4
using SSMuxPort = m1284p::GPIOSubPort<m1284p::Port::D, PORTD4, PORTD7>;

// #endif

#include "HD44780.h"
#include "line_decoder.h"

#include "spi_master.h"
#include "mux_output_pin.h"
#include "buffered_lcd.h"

using SPIMaster = SPIMasterSync<SPI>;
using SSMux = LineDecoder<SSMuxPort>;
constexpr uint8_t SSLCDPinIdx = 10;
using SSLCDPin = MuxPin<SSMux, SPIMaster::SS_Pin, SSLCDPinIdx>;
using LCDPort = HD44780_Configuration_74HC595<SPIMaster, SSLCDPin>;
using LCD = LCDAlphanumericBuffered<HD44780<HD44780_NofCharacters::_2x16, LCDPort>>; // 2x16 display using LCD port defined above

// Stream interface for LCD
template <typename LCD>
struct LCDStream
{
    template <typename Arg>
    LCDStream<LCD> & operator<<(const Arg& arg)
    {
        LCD::put(arg);
        LCD::refresh();
        return *this;
    }

    //struct Clear {};
    static constexpr struct EndLine{} endl = EndLine();

    LCDStream<LCD> & operator<< (const EndLine& arg)
    {
        LCD::newLine();
        return *this;
    }
    
    static constexpr struct Flush{} flush = Flush();

    LCDStream<LCD> & operator<< (const Flush& arg)
    {
        LCD::clear();
        return *this;
    }
};

int main()
{
    SPIMaster::init(
    m1284p::SPI::ClockRate::FOSC_4,
    m1284p::SPI::DataOrder::MSB_FIRST,
    m1284p::SPI::ClockPolarity::LOW,
    m1284p::SPI::ClockPhase::LEADING);

    SSMux::init();

    LCD::init();
    
    LCDStream<LCD> lcd;
    lcd << "Hello"_pgm << LCDStream<LCD>::endl << "World!";

    while(true);
}