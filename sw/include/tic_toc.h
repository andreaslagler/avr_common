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

#ifndef TICTOC_H
#define TICTOC_H

/**
@brief Timer for hardware debugging using an oscilloscope.
Every time a TicToc object is instantiated, the debug pin is set to high
Every time a TicToc object is deleted, the debug pin is set to low
@tparam DebugPin Digital I/O pin driver class implementing static methods high(), low() and setAsOutput()
*/
template <typename DebugPin>
class TicToc
{
    public:
    
    /**
    @brief Constructor. When a TicToc object is instantiated, the debug pin is set to high
    */
    TicToc()
    {
        DebugPin::setAsOutput();
        DebugPin::high();
    }

    /**
    @brief Destructor. When a TicToc object goes out of scope, the debug pin is set to low
    */
    ~TicToc()
    {
        DebugPin::low();
    }
};

#endif