/*
Copyright (C) 2022 Andreas Lagler

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

// ATMega1284P is running at 20 MHz
#define F_CPU 20000000UL

#include "m1284p_GPIO.h"
#include "m1284p_SPI.h"
#include "m1284p_Int0.h"

using SPI = m1284p::SPI;

// Multiplexer for SPI Slave Select pin hooked to PD7:4
using SSMuxPort = m1284p::GPIOSubPort<m1284p::Port::D, PORTD4, PORTD7>;

#include "HD44780.h"
#include "MCP23S17.h"
#include "line_decoder.h"
#include "spi_master.h"
#include "mux_output_pin.h"
#include "buffered_lcd.h"

using SPIMaster = SPIMasterSync<SPI>;
using SSMux = LineDecoder<SSMuxPort>;
constexpr uint8_t SSLCDPinIdx = 1;
using SSLCDPin = MuxPin<SSMux, SPIMaster::SS_Pin, SSLCDPinIdx>;
using LCDPort = HD44780_Configuration_74HC595<SPIMaster, SSLCDPin>;
using LCD = LCDAlphanumericBuffered<HD44780<HD44780_NofCharacters::_2x16, LCDPort>>; // 2x16 display using LCD port defined above

constexpr uint8_t SSButtonsPinIdx = 0;
using SSButtonsPin = MuxPin<SSMux, SPIMaster::SS_Pin, SSButtonsPinIdx>;

typedef MCP23S17<
SPIMaster,
SSButtonsPin,
MCP23S17PinConfig<MCP23S17PinIdx::A0, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A1, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A2, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A3, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A4, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A5, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A6, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::A7, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B0, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B1, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B2, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B3, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B4, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B5, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B6, MCP23xxxPinType::SWITCH>,
MCP23S17PinConfig<MCP23S17PinIdx::B7, MCP23xxxPinType::SWITCH>
> Buttons;

typedef Buttons::Pin<MCP23S17PinIdx::A0> Button_00;
typedef Buttons::Pin<MCP23S17PinIdx::A1> Button_01;
typedef Buttons::Pin<MCP23S17PinIdx::A2> Button_02;
typedef Buttons::Pin<MCP23S17PinIdx::A3> Button_03;
typedef Buttons::Pin<MCP23S17PinIdx::A4> Button_04;
typedef Buttons::Pin<MCP23S17PinIdx::A5> Button_05;
typedef Buttons::Pin<MCP23S17PinIdx::A6> Button_06;
typedef Buttons::Pin<MCP23S17PinIdx::A7> Button_07;
typedef Buttons::Pin<MCP23S17PinIdx::B0> Button_08;
typedef Buttons::Pin<MCP23S17PinIdx::B1> Button_09;
typedef Buttons::Pin<MCP23S17PinIdx::B2> Button_10;
typedef Buttons::Pin<MCP23S17PinIdx::B3> Button_11;
typedef Buttons::Pin<MCP23S17PinIdx::B4> Button_12;
typedef Buttons::Pin<MCP23S17PinIdx::B5> Button_13;
typedef Buttons::Pin<MCP23S17PinIdx::B6> Button_14;
typedef Buttons::Pin<MCP23S17PinIdx::B7> Button_15;

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

    LCDStream<LCD> & operator<< (const EndLine&)
    {
        LCD::newLine();
        return *this;
    }
    
    static constexpr struct Flush{} flush = Flush();

    LCDStream<LCD> & operator<< (const Flush&)
    {
        LCD::clear();
        return *this;
    }
};


void print(const uint8_t buttonIdx)
{
    LCDStream<LCD> lcd;
    lcd << lcd.flush << "Button "_pgm << buttonIdx;
    Buttons::reArmInterrupt();
}    

int main()
{
    SPIMaster::init(
    m1284p::SPI::ClockRate::FOSC_4,
    m1284p::SPI::DataOrder::MSB_FIRST,
    m1284p::SPI::ClockPolarity::LOW,
    m1284p::SPI::ClockPhase::LEADING);

    SSMux::init();
    
    LCD::init();

    m1284p::Int0::init();
    m1284p::Int0::registerCallback([]{Buttons::onInterrupt();});
        
    Buttons::init();
    Button_00::registerCallback([]{print(0);});    
    Button_01::registerCallback([]{print(1);});
    Button_02::registerCallback([]{print(2);});
    Button_03::registerCallback([]{print(3);});
    Button_04::registerCallback([]{print(4);});
    Button_05::registerCallback([]{print(5);});
    Button_06::registerCallback([]{print(6);});
    Button_07::registerCallback([]{print(7);});
    Button_08::registerCallback([]{print(8);});
    Button_09::registerCallback([]{print(9);});
    Button_10::registerCallback([]{print(10);});
    Button_11::registerCallback([]{print(11);});
    Button_12::registerCallback([]{print(12);});
    Button_13::registerCallback([]{print(13);});
    Button_14::registerCallback([]{print(14);});
    Button_15::registerCallback([]{print(15);});
    sei();    
    while(true);
}
