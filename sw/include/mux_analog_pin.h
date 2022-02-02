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

#ifndef MUX_ANALOG_PIN_H
#define MUX_ANALOG_PIN_H

#include <stdint.h>

/**
@brief Driver class for a multiplexed analog input pin
@tparam MuxDevice Multiplexer device driver class implementing a static selectChannel() method
@tparam Pin Analog input pin driver class implementing static methods startConversion(), wait() and a static template method read<Result>()
@tparam t_channel Multiplexer channel
*/
template <typename Pin, typename MuxDevice, uint8_t t_channel>
class MuxAnalogPin : public Pin
{
    public:

    /**
    @brief Start AD conversion on the selected multiplexer channel
    */
    static void startConversion() __attribute__((always_inline))
    {
        // t_channel is a compile-time constant, so a static assert is used instead of a runtime check inside MuxDevice::selectChannel(t_channel) 
        static_assert(t_channel < MuxDevice::getNofChannels(), "Invalid multiplexer channel: Selected channel index >= number of channels");
        
        MuxDevice::selectChannel(t_channel);
        Pin::startConversion();
    }
    
    using Pin::read;
    using Pin::wait;
};

#endif