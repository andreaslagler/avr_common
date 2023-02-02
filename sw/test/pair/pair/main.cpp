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

#include <utility.h>
#include "../../common/debug_print.h"

int main(void)
{
    // Default constructor
    Pair<uint8_t, uint8_t> p1;
    cout << p1;
    
    // Non-standard constructor
    uint8_t first = 42;
    uint8_t second = 123;
    Pair<uint8_t, uint8_t> p2(first,second);
    cout << p2;
    
    // Non-standard constructor
    Pair<uint8_t, uint8_t> p3(first+first,second+second);
    cout << p3;
    
    // Non-standard constructor
    Pair<uint16_t, uint16_t> p4(first,second);
    cout << p4;
    
    //  Copy constructor
    cout << Pair<uint8_t, uint8_t>(p2);
    cout << Pair<uint8_t, uint8_t>(Pair<uint8_t, uint8_t>(p2));
    
    // Converting constructor
    cout << Pair<uint16_t, uint16_t>(p2);
    cout << Pair<uint16_t, uint16_t>(Pair<uint8_t, uint8_t>(p2));
    
    // Copy assignment
    cout << (p3 = p2);
    cout << (p3 = Pair<uint8_t, uint8_t>(p2));
    
    // Conversion assignment
    cout << (p4 = p2);
    cout << (p4 = Pair<uint8_t, uint8_t>(p2));
    
    // make_pair
    cout << make_pair(p2.second,1111UL);
    
    // swap
    cout << p1;
    cout << p2;
    swap(p1,p2);
    cout << p1;
    cout << p2;
    
    // get
    cout << get<0>(p1);
    cout << get<1>(p1);
    //cout << get<2>(p1); // Error
    
    while (true)
    {
    }
}

// Specializations of debug printer
template<typename T1, typename T2>
struct debugPrinter<Pair<T1,T2>>
{
    static void print(const Pair<T1,T2>& arg)
    {
        // Put a tracepoint here and display {arg} in output window
        (void)arg;
    }
};

template<>
struct debugPrinter<uint8_t>
{
    static void print(const uint8_t arg)
    {
        // Put a tracepoint here and display {arg} in output window
        (void)arg;
    }
};
