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

#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <stdlib.h> // abs()
#include <stdint.h>
#include "Subject.h"

enum class PotentiometerDetent : uint8_t
{
    // No detent
    NONE = 0x00,
    
    // Center detent
    CENTER,
    
    NOFENTRIES
};

/**
@brief Driver class for a Potentiometer connected to an analog input pin
@tparam Pin Analog input pin driver class implementing static methods startConversion(), wait() and a static template method read<Result>()
*/
template <typename Pin, PotentiometerDetent t_detent = PotentiometerDetent::NONE>
class Potentiometer;


template <typename Pin>
class Potentiometer <Pin, PotentiometerDetent::NONE> : public Pin
{
    public:
    
    /**
    @brief Register an observer for potentiometer value changed event
    @param observer Any observer callback accepting an uint8_t parameter to be notified on potentiometer value change
    */
    static void registerObserver(const typename Subject<uint8_t>::Observer & observer)
    {
        s_subject.registerObserver(observer);
    }
    
    /**
    @brief Check current potentiometer value and notify observer if value has changed
    @param value Potentiometer value to check
    */
    static void check(const uint16_t value)
    {        
        // Values are considered identical if difference is less than half of the 8-Bit LSB
        if (abs(value - s_lastValue) > 128)
        {
            set(value);
        }
    }
    
    /**
    @brief Check current potentiometer value and notify observer if value has changed
    @param value Potentiometer value to check
    */
    static void set(const uint16_t value)
    {
        s_lastValue = value;
        s_subject.notifiyObserver(value >> 8);
    }
    
    using Pin::read;
    using Pin::wait;
    using Pin::startConversion;

    protected:
    
    // Subject for notification on potentiometer value change
    static Subject<uint8_t> s_subject;
    
    static uint16_t s_lastValue;
};

// Static initialization
template <typename Pin>
Subject<uint8_t> Potentiometer<Pin, PotentiometerDetent::NONE>::s_subject;

template <typename Pin>
uint16_t Potentiometer<Pin, PotentiometerDetent::NONE>::s_lastValue = 0;

static uint8_t calc_potvalue(const uint16_t adc_value)
{
    const uint8_t adc_value_MSB = static_cast<uint8_t>(adc_value >> 8);
    
    if (adc_value_MSB <= 120)
    {
        return (adc_value + (adc_value >> 4)) >> 8;
    }
    
    if (adc_value_MSB >= 136)
    {
        return (adc_value - 4096 + (adc_value >> 4)) >> 8;
    }

    // Default is center position
    return 128;
}


template <typename Pin>
class Potentiometer <Pin, PotentiometerDetent::CENTER> : public Potentiometer<Pin>
{
    public:
    
    /**
    @brief Check current potentiometer value and notify observer if value has changed
    @param value Potentiometer value to check
    */
    static void set(const uint16_t value) override
    {
        Potentiometer<Pin>::s_lastValue = value;
        Potentiometer<Pin>::s_subject.notifiyObserver(calc_potvalue(value));
    }
};

#endif