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
    
    /**
    @brief Initialization
    */
    static constexpr void init()
    {
        // Attach observer callback to subject
        PhaseAPin::registerObserver([]() {s_subject.notifyObserver(t_polarity == PhaseBPin::read(), s_currentSpeed); s_currentSpeed = t_maxSpeed;});
    }

    /**
    @brief Clock. This method has to be called periodically in order to decrease the current speed after the last rotation event
    */
    static constexpr void clock()
    {
        // Decrease the current encoder rotational speed exponentially
        s_currentSpeed >>= 1;

        // Make sure the current encoder rotational speed is >= 1
        s_currentSpeed |= 1;
    }

    /**
    @brief Register an observer encoder rotation events
    @param observer Observer accepting a boolean parameter indicating the direction of rotation of the encoder and an uint8_t parameter indicating the current rotational speed
    */
    static constexpr void registerObserver(const Subject<bool, uint8_t>::Observer& observer)
    {
        s_subject.registerObserver(observer);
    }
    private:
    
    static uint8_t s_currentSpeed;
    static Subject<bool, uint8_t> s_subject;
};

// Static initialization
template <typename PhaseAPin, typename PhaseBPin, bool t_polarity, uint8_t t_maxSpeed>
uint8_t RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, t_maxSpeed>::s_currentSpeed = 1;

template <typename PhaseAPin, typename PhaseBPin, bool t_polarity, uint8_t t_maxSpeed>
Subject<bool, uint8_t> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, t_maxSpeed>::s_subject;

/**
@brief Driver class for a rotary encoder ignoring the actual rotational speed of the encoder
@tparam PhaseAPin Digital I/O pin driver class implementing a static registerObserver() method
@tparam PhaseBPin Digital I/O pin driver class implementing a static read() method
@tparam t_polarity Boolean flag indicating the PhaseBPin state for clockwise rotation of the encoder
*/
template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
class RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, 0>
{
    public:
    
    /**
    @brief Initialization
    */
    static constexpr void init()
    {
        // Attach observer callback to subject
        PhaseAPin::registerObserver([]() {s_subject.notifyObserver(t_polarity == PhaseBPin::read());});
    }

    /**
    @brief Register an observer encoder rotation events
    @param observer Observer accepting a boolean parameter indicating the direction of rotation of the encoder
    */
    static constexpr void registerObserver(const Subject<bool>::Observer& observer)
    {
        s_subject.registerObserver(observer);
    }

    private:
    
    static Subject<bool> s_subject;
};

// Static initialization
template <typename PhaseAPin, typename PhaseBPin, bool t_polarity>
Subject<bool> RotaryEncoder<PhaseAPin, PhaseBPin, t_polarity, 0>::s_subject;



#endif