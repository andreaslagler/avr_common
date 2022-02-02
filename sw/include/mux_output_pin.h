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

#ifndef MUX_PIN_H
#define MUX_PIN_H

#include <stdint.h>

/**
@brief Driver class for a multiplexed digital I/O pin
@tparam MuxDevice Multiplexer device driver class implementing a static selectLine() method
@tparam Pin Digital I/O pin driver class implementing static methods high(), low() and setAsOutput()
@tparam t_muxLine Multiplexer output line
@note This class is only implementing the logic for an output pin so far
*/
template <typename MuxDevice, typename Pin, uint8_t t_muxLine>
class MuxPin
{
    public:

    /**
    @brief Drive pin high
    */
    static void high()
    {
        MuxDevice::selectLine(t_muxLine);
        Pin::high();
    }

    /**
    @brief Drive pin low
    */
    static void low()
    {
        MuxDevice::selectLine(t_muxLine);
        Pin::low();
    }
};

#endif