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

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>
#include <stdbool.h>
#include "Subject.h"

/**
@brief Driver class for a rotary encoder
@tparam MuxDevice Multiplexer device driver class implementing a static selectLine() method
@tparam Pin Digital I/O pin driver class implementing static methods high(), low() and setAsOutput()
@tparam t_muxLine Multiplexer output line
@note This class is only implementing the logic for an output pin so far
*/
template <typename PhaseAPin, typename PhaseBPin, bool t_polarity = true>
class RotaryEncoder
{
    public:
    
    static constexpr void init()
    {
        PhaseAPin::registerObserver([]() {s_subject.notifyObserver(t_polarity == PhaseBPin::read());});
    }

    /**
    @brief
    */
    static constexpr void registerObserver(const Subject<bool>::Observer& observer)
    {
        s_subject.registerObserver(observer);
    }

    private:
    
    static Subject<bool> s_subject;
};

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
Subject<bool> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity>::s_subject;


#endif