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

// Potentiometer callback
template <uint8_t t_index>
void print(const uint8_t value)
{
    const uint8_t index = t_index;
    // Put a tracepoint here:
    // Reading of potentiometer {index} is {value}
}

// Dummy ADC pin
template <uint8_t t_index>
struct ADCPin
{
    static void startConversion()
    {}
    
    template <typename Result>
    static Result readResult()
    {
        return static_cast<Result>(s_adcValue);
    }
    
    static void wait()
    {}
    
    static uint16_t s_adcValue;
};

template <uint8_t t_Index>
uint16_t ADCPin<t_Index>::s_adcValue = 0;

// Potentiometers
using Potentiometer0 = Potentiometer<ADCPin<0>>;
using Potentiometer1 = Potentiometer<ADCPin<1>, PotentiometerDetent::CENTER>;

int main()
{
    // Register callbacks
    Potentiometer0::registerCallback(print<0>);
    Potentiometer1::registerCallback(print<1>);
    
    // Init
    Potentiometer0::forceUpdateSync();
    
    // Read ADC values
    for (uint16_t value = 0; value < 65535; ++value)
    {
        Potentiometer0::Pin::s_adcValue = value;
        Potentiometer0::updateSync();
    }
    
    for (uint16_t value = 0; value < 65408; ++value += 128)
    {
        Potentiometer1::Pin::s_adcValue = value;
        Potentiometer1::forceUpdateSync();
    }
    
    //sei();
    while(true)
    {
    }
}
