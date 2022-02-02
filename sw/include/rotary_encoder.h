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
    
    static void init()
    {
        PhaseAPin::registerObserver([]() {if (t_polarity == PhaseBPin::read()) s_subjectCW.notifiyObserver(); else s_subjectCCW.notifiyObserver();});
    }

    /**
    @brief
    */
    static void registerObserverCW(const Subject<void>::Observer& observer)
    {
        s_subjectCW.registerObserver(observer);
    }

    /**
    @brief
    */
    static void registerObserverCCW(const Subject<void>::Observer& observer)
    {
        s_subjectCCW.registerObserver(observer);
    }

    private:
    
    static Subject<void> s_subjectCW;
    static Subject<void> s_subjectCCW;
};

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
Subject<void> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity>::s_subjectCW;

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
Subject<void> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity>::s_subjectCCW;


#endif