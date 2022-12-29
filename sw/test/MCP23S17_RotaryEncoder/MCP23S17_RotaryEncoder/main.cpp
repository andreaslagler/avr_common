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
#include "rotary_encoder.h"
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
MCP23S17PinConfig<MCP23S17PinIdx::A0, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::A1, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::A2, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::A3, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::A7, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::A6, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::A5, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::A4, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::B0, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::B1, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::B2, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::B3, MCP23xxxPinType::ROTENC_PHASE_A>,
MCP23S17PinConfig<MCP23S17PinIdx::B7, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::B6, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::B5, MCP23xxxPinType::ROTENC_PHASE_B>,
MCP23S17PinConfig<MCP23S17PinIdx::B4, MCP23xxxPinType::ROTENC_PHASE_B>
> Encoder;

typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::A0>, Encoder::Pin<MCP23S17PinIdx::A7>, true> Encoder_00;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::A1>, Encoder::Pin<MCP23S17PinIdx::A6>, true> Encoder_01;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::A2>, Encoder::Pin<MCP23S17PinIdx::A5>, true> Encoder_02;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::A3>, Encoder::Pin<MCP23S17PinIdx::A4>, true> Encoder_03;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::B0>, Encoder::Pin<MCP23S17PinIdx::B7>, true> Encoder_04;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::B1>, Encoder::Pin<MCP23S17PinIdx::B6>, true> Encoder_05;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::B2>, Encoder::Pin<MCP23S17PinIdx::B5>, true> Encoder_06;
typedef RotaryEncoder<Encoder::Pin<MCP23S17PinIdx::B3>, Encoder::Pin<MCP23S17PinIdx::B4>, true> Encoder_07;

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

void print(const uint8_t idx, const bool direction, const uint8_t speed = 1)
{
    LCDStream<LCD> lcd;
    static uint8_t value = 0;
    
    if (direction)
    value += speed;
    else
    value -= speed;
    
    lcd << lcd.flush << "Encoder "_pgm << idx << " : "_pgm << lcd.endl << value;
    Encoder::reArmInterrupt();    
}

template <uint8_t t_Idx>
void print(const bool direction, const uint8_t speed)
{
    print(t_Idx, direction, speed);
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
    m1284p::Int0::registerCallback([]{Encoder::onInterrupt();});
    
    Encoder::init();
    Encoder_00::init();
    Encoder_01::init();
    Encoder_02::init();
    Encoder_03::init();
    Encoder_04::init();
    Encoder_05::init();
    Encoder_06::init();
    Encoder_07::init();

    Encoder_00::registerCallback([](const bool direction){print(0, direction);});
    Encoder_01::registerCallback([](const bool direction){print(1, direction);});
    Encoder_02::registerCallback([](const bool direction){print(2, direction);});
    Encoder_03::registerCallback([](const bool direction){print(3, direction);});
    Encoder_04::registerCallback([](const bool direction){print(4, direction);});
    Encoder_05::registerCallback([](const bool direction){print(5, direction);});
    Encoder_06::registerCallback([](const bool direction){print(6, direction);});
    Encoder_07::registerCallback([](const bool direction){print(7, direction);});
        
    sei();
    while(true);
}
