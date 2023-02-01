/*
Copyright (C) 2020 Andreas Lagler

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

#include "buffered_lcd.h"


void print(const char * str)
{
    // Put a tracepoint here
    // {str,s}
}

// Dummy for LCD driver emulating a 2x16 LCD
class DummyLCD
{
    public:
    
    static constexpr uint8_t getNofColumns()
    {
        return s_nofCols;
    }

    static constexpr uint8_t getNofRows()
    {
        return s_nofRows;
    }
    
    static constexpr void init()
    {
        for (char & c : s_buffer)
        {
            c = ' ';
        }
        
        s_buffer[s_nofRows * s_nofCols] = '\0';
        
        s_cursor = 0;
    }
    
    static void putc(const char c)
    {
        s_buffer[s_cursor++] = c;
        
        // Only dump LCD content on a complete refresh 
        if (s_cursor >= s_nofRows * s_nofCols)
        {
            print(s_buffer);
        }            
    }
    
    static constexpr void setCursor(const uint8_t row, const uint8_t col)
    {
        s_cursor = row * s_nofCols + col;
    }
    
    private:
    
    static constexpr uint8_t s_nofRows = 2;
    static constexpr uint8_t s_nofCols = 16;
    
    static uint8_t s_cursor;
    static char s_buffer[s_nofRows * s_nofCols+1];
};

uint8_t DummyLCD::s_cursor = 0;
char DummyLCD::s_buffer[s_nofRows * s_nofCols+1];

using LCD = LCDAlphanumericBuffered<DummyLCD>;

int main()
{
    LCD::init();
    
    LCD::put("Hello World!");
    LCD::refresh();
    
    LCD::clear();
    LCD::put("PROGRAM MEMORY"_pgm); 
    LCD::refresh();

    LCD::clear();    
    LCD::put(String<12>("DATA MEMORY"));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint8_t>(1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint8_t>(12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint8_t>(123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum2(static_cast<uint8_t>(1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum2(static_cast<uint8_t>(12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum2(static_cast<uint8_t>(123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum1(static_cast<uint8_t>(1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum1(static_cast<uint8_t>(12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum1(static_cast<uint8_t>(123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<int8_t>(1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<int8_t>(12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<int8_t>(123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<int8_t>(-1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<int8_t>(-12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<int8_t>(-123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint16_t>(1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint16_t>(12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint16_t>(123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint16_t>(1234));
    LCD::refresh();

    LCD::clear();
    LCD::putNum(static_cast<uint16_t>(12345));    
    LCD::refresh();
    
    LCD::clear();    
    LCD::putNum3(static_cast<uint16_t>(1));
    LCD::refresh();

    LCD::clear();
    LCD::putNum3(static_cast<uint16_t>(12));
    LCD::refresh();

    LCD::clear();
    LCD::putNum3(static_cast<uint16_t>(123));
    LCD::refresh();

    LCD::clear();
    LCD::putNum3(static_cast<uint16_t>(1234));
    LCD::refresh();

    LCD::clear();
    LCD::putNum3(static_cast<uint16_t>(12345));
    LCD::refresh();
    
    while(true);
}
