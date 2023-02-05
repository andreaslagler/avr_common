/*
Copyright (C) 2023 Andreas Lagler

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

#include "div.h"

void error()
{
    // Put a break point here to stop on error while debugging
    while (true);
}

template <uint8_t denominator = 255>
void divTest(const uint8_t numerator)
{
    if (div<denominator>(numerator) != numerator / denominator)
    {
        error();
    }
    
    divTest<denominator-1>(numerator);
}

template <>
void divTest<0>(const uint8_t)
{
    // Don't divide by zero ;-)
}


int main(void)
{
    // Try all combinations of numerator/denominator
    for (uint8_t numerator = 0; numerator < 255; ++numerator)
    {
        divTest(numerator);
    }

    while (true)
    {
    }
}