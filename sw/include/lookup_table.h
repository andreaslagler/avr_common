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

#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H

#include "pair.h"
#include <avr/pgmspace.h>
#include "memcopy.h"

/**
@brief Template class implementing a compile-time or run-time constant array of data.
This array is populated by the constructor with given elements at given indices, while all other table entries are set to a default value.
@note Use the operator() to access elements of constexpr instances or instances allocated in RAM. Use the getP() method to access elements of instances allocated in PROG_MEM.
@tparam Elem Type of elements stored in the table
@tparam Idx Index type used when accessing the table
@tparam t_length number of elements stored in the table
*/
template <typename Elem, typename Idx, Idx t_length>
class SparseLUT
{
    public:
    
    /// @brief Data type for table entries
    typedef Pair<Idx, Elem> Entry;

    /**
    @brief Non-standard constructor. This constructor has to be called with a variadic list of CPair<Idx, Elem> objects and populates the actual table at given positions.
    @note This constructor is constexpr so it can be used to create objects at compile-time
    @param A parameter pack of Pair<Idx, Elem> objects or their respective initializer lists
    */
    template <typename ... Arg>
    constexpr SparseLUT(const Elem& defaultElem, Arg ... arg)
    {
        // Set all elements to default
        for (auto & data : m_data)
        {
            data = defaultElem;
        }
        
        // Dump parameter pack into a temporary array of Pair objects
        // This works with a parameter pack consisting of actual Pair objects but also with initializer lists
        const Entry entries[] = {arg ...};

        // Overwrite elements given by arguments
        for (const auto & entry : entries)
        {
            if (t_length > entry.first())
            {
                m_data[entry.first()] = entry.second();
            }                
        }
    }

    /**
    @brief Read only access to table entries for SparseLUT objects stored in RAM.
    @param idx Index of element to read
    */
    constexpr const Elem & operator() (const Idx idx) const
    {
        return m_data[idx];
    }

    /**
    @brief Read only access to table entries for SparseLUT objects stored in PROGMEM.
    @note This method cannot be constexpr because it has to read from PROGMEM
    @param idx Index of element to read
    */
    const Elem getP(const Idx idx) const
    {
        return memread_P(&m_data[idx]);
    }

    protected:

    Elem m_data[t_length];
};

#endif
