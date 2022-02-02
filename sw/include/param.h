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

#ifndef PARAM_H
#define PARAM_H

#include "type_traits.h"

/**
@brief Template class implementing a generic numerical UI parameter
@tparam value Encapsulated parameter type
*/
template <typename Value>
class Param
{
    public:

    /**
    @brief Default constructor
    @param value Initial parameter value
    */
    constexpr Param(Value value = numeric_limits<Value>::min()) : m_value{value}
    {}
    
    /**
    @brief Read access by cast to value type
    @result Encapsulated parameter value
    */
    constexpr const Value & getValue() const
    {
        return m_value;
    }

    /**
    @brief Read access by cast to value type
    @result Encapsulated parameter value
    */
    constexpr operator const Value & () const
    {
        return m_value;
    }

    /**
    @brief Write access
    @param value Parameter value to be written
    @result Reference to the parameter instance
    */
    constexpr Param<Value> & operator=(const Value & value)
    {
        m_value = value;
        
        return *this;
    }

    /**
    @brief Increment until max value is reached
    @param maxValue Maximum parameter value
    @result Reference to the parameter instance
    */
    constexpr Param<Value> & increment(const Value & maxValue = numeric_limits<Value>::max())
    {
        if (m_value < maxValue)
        {
            ++m_value;
        }
        
        return *this;
    }

    /**
    @brief Decrement until min value is reached
    @param minValue Minimum parameter value
    @result Reference to the parameter instance
    */
    constexpr Param<Value> & decrement(const Value & minValue = numeric_limits<Value>::min())
    {
        if (m_value > minValue)
        {
            --m_value;
        }
        
        return *this;
    }

    /**
    @brief Increment until max value is reached, then roll over to min value
    @param minValue Minimum parameter value
    @param maxValue Maximum parameter value
    @result Reference to the parameter instance
    */
    constexpr Param<Value> & incrementRollover(const Value & minValue = numeric_limits<Value>::min(), const Value & maxValue = numeric_limits<Value>::max())
    {
        if (m_value >= maxValue)
        {
            m_value = minValue;
        }
        else
        {
            ++m_value;
        }

        return *this;
    }
    
    private:

    // Parameter buffer
    Value m_value;
};

# endif