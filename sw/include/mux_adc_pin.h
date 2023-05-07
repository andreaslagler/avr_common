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

#ifndef MUX_ADC_PIN_H
#define MUX_ADC_PIN_H

#include <stdint.h>

/**
@brief Driver class for a multiplexed ADC pin
@tparam MuxDevice Multiplexer device driver class implementing static methods getNofChannels() and selectChannel() 
@tparam ADCPin ADC pin driver class implementing static methods startConversion(), wait() and a static template method read<Result>()
@tparam t_channel Zero-based channel index on multiplexer device
*/
template <typename ADCPin, typename MuxDevice, uint8_t t_channel>
class MuxADCPin : public ADCPin
{
    typedef ADCPin __super;
    
    public:

    /**
    @brief Start AD conversion on the selected multiplexer channel
    */
    static void startConversion()
    {
        // Validity check
        static_assert(t_channel < MuxDevice::getNofChannels(), "Invalid multiplexer channel: Selected channel index >= number of channels");
        
        // Select channel on multiplexer device
        MuxDevice::selectChannel(t_channel);
        
        // Start A/D conversion on multiplexed ADC pin
        __super::startConversion();
    }
    
    using __super::read;
    using __super::wait;
};

#endif