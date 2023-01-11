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
#include <functional>

enum class PotentiometerDetent : uint8_t
{
    // No detent
    NONE = 0x00,
    
    // Center detent
    CENTER,
    
    NOFENTRIES
};

/**
@brief Driver class for a Potentiometer
@tparam t_detent Detent type of potentiometer
*/
template <PotentiometerDetent t_detent>
class PotentiometerBase;

/**
@brief Driver class for a Potentiometer connected to an ADC input pin
@tparam ADCPin Analog input pin driver class implementing static methods startConversion(), wait() and a static template method readResult<Result>()
@tparam t_detent Detent type of potentiometer. Default is no detent
*/
template <typename ADCPin, PotentiometerDetent t_detent = PotentiometerDetent::NONE>
class Potentiometer : public PotentiometerBase<t_detent>
{
    typedef PotentiometerBase<t_detent> __super;
    
    public:
    
    /**
    @brief Alias for the connected ADC pin
    */
    typedef ADCPin Pin;
    
    /**
    @brief Register a callback to be invoked when potentiometer value has changed
    @param callback Any callback accepting an uint8_t parameter to be notified on potentiometer value change
    */
    template <typename Callback>
    static void registerCallback(Callback&& callback)
    {
        s_callback = callback;
    }
    
    /**
    @brief Asynchronous update of potentiometer value
    This method will read the  A/D conversion result assuming the A/D conversion on the associated ADC pin is completed (hence "asynchronous") and invoke the registered callback only if the potentiometer value is changed
    */
    static constexpr void updateAsync()
    {
        // Read ADC value
        const uint16_t adcValue = ADCPin:: template readResult<uint16_t>();
        
        // Do not update to the same pot value
        const uint8_t potValue = __super::convertAdcToPotValue(adcValue);
        if (potValue != __super::convertAdcToPotValue(s_lastAdcValue))
        {
            // Add some hysteresis (half of the 8-Bit LSB)
            if (abs(adcValue - s_lastAdcValue) > 128)
            {
                s_lastAdcValue = adcValue;
                s_callback(potValue);
            }
        }
    }
    
    /**
    @brief Asynchronous update of potentiometer value
    This method will read the  A/D conversion result assuming the A/D conversion on the associated ADC pin is completed (hence "asynchronous") and invoke the registered callback
    */
    static constexpr void forceUpdateAsync()
    {
        // Read ADC value
        const uint16_t adcValue = ADCPin:: template readResult<uint16_t>();

        // Force update of the ADC value
        s_lastAdcValue = adcValue;
        
        // Force invocation of callback
        const uint8_t potValue = __super::convertAdcToPotValue(adcValue);
        s_callback(potValue);
    }
    
    /**
    @brief Synchronous update of potentiometer value
    This method will start an A/D conversion on the associated ADC pin, wait actively until the A/D conversion result is available, read the A/D conversion result (hence "synchronous") and invoke the registered callback only if the potentiometer value is changed
    */
    static void updateSync()
    {
        ADCPin::startConversion();
        ADCPin::wait();
        updateAsync();
    }

    /**
    @brief Synchronous update of potentiometer value
    This method will start an A/D conversion on the associated ADC pin, wait actively until the A/D conversion result is available, read the A/D conversion result (hence "synchronous") and invoke the registered callback
    */
    static void forceUpdateSync()
    {
        ADCPin::startConversion();
        ADCPin::wait();
        forceUpdateAsync();
    }

    private:

    // Callback for potentiometer value change
    static std::function<void(uint8_t)> s_callback;

    static uint16_t s_lastAdcValue;
};

// Static initialization
template <typename ADCPin, PotentiometerDetent t_detent>
std::function<void(uint8_t)> Potentiometer<ADCPin, t_detent>::s_callback;

template <typename ADCPin, PotentiometerDetent t_detent>
uint16_t Potentiometer<ADCPin, t_detent>::s_lastAdcValue = 0;

/**
@brief Driver class for a potentiometer without detent (base class)
*/
template <>
class PotentiometerBase<PotentiometerDetent::NONE>
{
    protected:
    
    // Conversion of 16bit ADC readout to 8bit potentiometer value
    // This method is implemented in the base class to avoid template code bloat
    static constexpr uint8_t convertAdcToPotValue(const uint16_t adcValue)
    {
        // No detent --> return MSB of ADC value
        return static_cast<uint8_t>(adcValue >> 8);
    }
};

/**
@brief Driver class for a potentiometer with center detent (base class)
*/
template <>
class PotentiometerBase<PotentiometerDetent::CENTER>
{
    protected:
    
    // Conversion of 16bit ADC readout to 8bit potentiometer value
    // This method is implemented in the base class to avoid template code bloat
    static constexpr uint8_t convertAdcToPotValue(const uint16_t adcValue)
    {
        // Center detent --> check raw pot value given by MSB of ADC value
        const uint8_t potValue = static_cast<uint8_t>(adcValue >> 8);
        
        // Check if raw pot value is inside the guard band 128 +/- 8
        if (potValue <= 120)
        {
            // Pot value is below guard band --> return compressed value
            return (adcValue + (adcValue >> 4)) >> 8;
        }
        
        if (potValue >= 136)
        {
            // Pot value is above guard band --> return compressed value
            return (adcValue - 4096 + (adcValue >> 4)) >> 8;
        }

        // Value is inside guard band --> return center position
        return 128;
    }
};


#endif