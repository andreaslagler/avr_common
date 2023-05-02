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
#include <functional.h>

/**
@brief Driver class for a rotary encoder
@tparam PhaseAPin Digital I/O pin driver class implementing a static registerObserver() method
@tparam PhaseBPin Digital I/O pin driver class implementing a static read() method
@tparam t_polarity Boolean flag indicating the PhaseBPin state for clockwise rotation of the encoder
@tparam t_maxSpeed Maximum speed value returned by two subsequent increment/decrement events
@note Current encoder speed decays with calls of clock() method
*/
template <typename PhaseAPin, typename PhaseBPin, bool t_polarity = true, uint8_t t_maxSpeed = 0>
class RotaryEncoder
{
    public:
    
    static constexpr void init()
    {
        PhaseAPin::registerCallback([]{s_callback(t_polarity == PhaseBPin::read(), s_currentSpeed); resetSpeed();});
    }

    static constexpr void clock()
    {
        (s_currentSpeed >>= 1) |= 1;
    }

    /**
    @brief
    */
    template <typename Callback>
    static constexpr void registerCallback(Callback&& callback)
    {
        s_callback = callback;
    }

    private:
    
    static constexpr void resetSpeed()
    {
        s_currentSpeed = t_maxSpeed;
    }
    
    static uint8_t s_currentSpeed;
    static function<void(bool, uint8_t)> s_callback;
};

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity, uint8_t t_maxSpeed>
uint8_t RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, t_maxSpeed>::s_currentSpeed = 1;

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity, uint8_t t_maxSpeed>
function<void(bool, uint8_t)> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, t_maxSpeed>::s_callback;

/**
@brief Driver class for a rotary encoder
Specialization ignoring the encoder speed
@tparam PhaseAPin Digital I/O pin driver class implementing a static registerObserver() method
@tparam PhaseBPin Digital I/O pin driver class implementing a static read() method
@tparam t_polarity Boolean flag indicating the PhaseBPin state for clockwise rotation of the encoder
*/
template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
class RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, 0>
{
    public:
    
    /**
    @brief
    */
    static constexpr void init()
    {
        PhaseAPin::registerCallback([]{s_callback(t_polarity == PhaseBPin::read());});
    }

    /**
    @brief
    */
    template <typename Callback>
    static constexpr void registerCallback(Callback&& callback)
    {
        s_callback = callback;
    }

    private:
    
    static function<void(bool)> s_callback;
};

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
function<void(bool)> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, 0>::s_callback;



#endif