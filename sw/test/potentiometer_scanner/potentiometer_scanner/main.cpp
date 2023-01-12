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

#include <stdint.h>
#include "potentiometer.h"
#include "potentiometer_scanner.h"

// The test logic emulates the behaviour of the ADC
// The synchronous potentiometer scanner implentation uses ADC in synchronous operation mode
// which consists of three steps:
// 1. Start a new A/D conversion
// 2. Busy wait while A/D conversion is in progress
// 3. Read the A/D conversion result
//
// The asynchronous potentiometer scanner implentation uses ADC in asynchronous operation mode
// which consists of two steps:
// 1. Start a new A/D conversion
// 2. Read the A/D conversion result in the ADC interrupt handler

// ADC interrupt handler
void handleADCInterrupt();

// Dummy ADC pin
template <uint8_t t_index>
struct ADCPin
{
    static void startConversion()
    {
        // Indicate if A/D conversion is in progress
        s_adConversionInProgress = true;
    }
    
    template <typename Result>
    static Result readResult()
    {
        return static_cast<Result>(s_adcValue);
    }
    
    static void wait()
    {}
    
    static void interrupt()
    {
        // Interrupt if A/D conversion is in progress
        if (s_adConversionInProgress)
        {
            s_adConversionInProgress = false;
            handleADCInterrupt();
        }
    }
    
    static uint16_t s_adcValue;
    static bool s_adConversionInProgress;
};

template <uint8_t t_Index>
uint16_t ADCPin<t_Index>::s_adcValue = 0;

template <uint8_t t_Index>
bool ADCPin<t_Index>::s_adConversionInProgress = false;

// Potentiometers
using Potentiometer0 = Potentiometer<ADCPin<0>>;
using Potentiometer1 = Potentiometer<ADCPin<1>, PotentiometerDetent::CENTER>;

// Potentiometer scanner
using PotentiometerScanner = PotentiometerScannerAsync<
Potentiometer0,
Potentiometer1>;

// Potentiometer callback
void print(const uint8_t index, const uint8_t value)
{
    // Put a tracepoint here:
    // Reading of potentiometer {index} is {value}
}

template <uint8_t t_index>
void print(const uint8_t value)
{
    print(t_index, value);
}


// Fire ADC interrupt(s)
void adcInterrupt()
{
    // Fire interrupts for all used ADC pins
    Potentiometer0::Pin::interrupt();
    Potentiometer1::Pin::interrupt();
}
       
int main()
{
    // Register callbacks
    Potentiometer0::registerCallback(print<0>);
    Potentiometer1::registerCallback(print<1>);
    
    // Init
    PotentiometerScanner::init();

    // Update potentiometer scanner once    
    PotentiometerScanner::startOnce();
    Potentiometer0::Pin::s_adcValue = 10000;
    Potentiometer1::Pin::s_adcValue = 20000;
    adcInterrupt();

    // Run continuous
    Potentiometer0::Pin::s_adcValue = 0;
    Potentiometer1::Pin::s_adcValue = 0;
    PotentiometerScanner::init();
    PotentiometerScanner::startContinuous();
    
    // Emulate a "rotation" of potentiometer 0 at constant rotational speed (A/D increments of 1024 resulting in potentiometer increments of 4)
    // Potentiometer 1 is not touched and does not update, because A/D conversion returns the same value
    for (uint16_t value = 0; value < 64512; value += 1024)
    {
        Potentiometer0::Pin::s_adcValue = value;
        adcInterrupt();
    }
    
    // Emulate a "rotation" of potentiometer 1 at the same rotational speed (A/D increments of 1024 resulting in potentiometer increments of ~4 due to center detent)
    // Stop potentiometer scanner at some point in time
    for (uint16_t value = 0; value < 64512; value += 1024)
    {
        Potentiometer1::Pin::s_adcValue = value;
        adcInterrupt();
        if (value > 40000)
        {
            // Last update to potentiometer value 158. However, the emulated "rotation" of potentiometer 1 runs to completion
            PotentiometerScanner::stop();
        }
    }
   
    while(true)
    {
    }
}

void handleADCInterrupt()
{
    PotentiometerScanner::onADCInterrupt();
}
