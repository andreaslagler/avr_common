/*
Copyright (C) 2022 Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

// Test for GPIO class

// Prerequisites: GPIO Test passed

#include "potentiometer.h"
#include "m328p_ADC.h"

typedef m328p::ADConverter::Pin<0> PotPin;
typedef Potentiometer<PotPin, PotentiometerDetent::NONE> Pot;

int main(void)
{
         
    
    while (1) 
    {
        Pot::read<uint16_t>();
    }
}
    

