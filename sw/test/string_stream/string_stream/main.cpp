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

#include <string_stream.h>
#include <string.h>

using StringStream = BasicStringStream<String<>>;

int main(void)
{
    String<> myString;
    StringStream stringStream(myString);
    
    stringStream << setWidth(5) << rightAlign << static_cast<uint8_t>(42);
    
    stringStream << setWidth(5) << rightAlign << static_cast<int8_t>(-42);
    
    stringStream << setWidth(5) << rightAlign << static_cast<uint16_t>(1337);

    stringStream << setWidth(5) << rightAlign << "Hello"_pgm;

    stringStream << setWidth(5) << rightAlign << String("World");
   
    while (true)
    {
    }
}