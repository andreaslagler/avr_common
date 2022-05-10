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

#ifndef LCD_BUFFERED_H
#define LCD_BUFFERED_H

#include <stdint.h>
#include "div.h"
#include "string.h"

/**
@brief Driver for buffered operation of an alphanumeric LCD.
@tparam LCDAlphanumeric Underlying alphanumeric LCD device driver.
This driver class needs to implement the following static methods:
uint8_t getNofColumns()
uint8_t getNofRows()
void setCursor(const uint8_t rowIdx, const uint8_t columnIdx)
void putc(const char character)
*/
template <typename LCDAlphanumeric>
class LCDAlphanumericBuffered
{
    public:

    /**
    @brief Get number of columns of alphanumeric LCD from underlying alphanumeric LCD device
    @result Number of columns of alphanumeric LCD
    */
    static constexpr uint8_t getNofColumns()
    {
        return LCDAlphanumeric::getNofColumns();
    }

    /**
    @brief Get number of rows of alphanumeric LCD from underlying alphanumeric LCD device
    @result Number of rows of alphanumeric LCD
    */
    static constexpr uint8_t getNofRows()
    {
        return LCDAlphanumeric::getNofRows();
    }

    /**
    @brief Initialization
    */
    static void init()
    {
        LCDAlphanumeric::init();
        clear();
        refresh();
    }

    /**
    @brief Clear the frame buffer
    */
    static constexpr void clear()
    {
        s_refresh = true;
        uint8_t cntChar = getNofChars();
        char * frameBuffer = s_frameBuffer[0];
        do
        {
            *frameBuffer++ = ' ';
        }
        while (--cntChar);

        home();
    }

    /**
    @brief Set cursor to home position
    */
    static constexpr void home()
    {
        setCursor(0, 0);
    }
    
    /**
    @brief Set cursor to rowIdx (0..nofColumns-1) / columnIdx (0..nofRows-1) position
    @param rowIdx Row index
    @param columnIdx Column index
    */
    static constexpr void setCursor(const uint8_t rowIdx, const uint8_t columnIdx)
    {
        s_cursor = rowIdx * getNofColumns() + columnIdx;
    }

    /**
    @brief Put character to LCD
    @param character Character to displayed on LCD
    */
    static constexpr void putc(const char character)
    {
        if (s_cursor < getNofChars())
        {
            s_refresh = true;
            *(s_frameBuffer[0] + (s_cursor++)) = character;
        }
    }

    /**
    @brief Put terminated string to LCD
    @param string Zero-terminated string to displayed on LCD (stored in RAM)
    */
    static constexpr void puts(const char * string)
    {
        while (*string != '\0')
        {
            putc(*string++);
        }
    }

    /**
    @brief Put terminated string to LCD
    @param string Zero-terminated string to displayed on LCD (stored in RAM)
    */
    template <uint8_t t_size>
    static constexpr void puts(const String<t_size>& string)
    {
        for (const auto& character : string)
        {
            putc(character);
        }
    }

    /**
    @brief Put terminated string stored in PROGMEM to LCD
    @param string Zero-terminated string to displayed on LCD (stored in PROG_MEM)
    */
    static void putsP(const char * string)
    {
        char character = pgm_read_byte(string++);
        while (character != 0)
        {
            putc(character);
            character = pgm_read_byte(string++);
        }
    }

    /**
    @brief Put digit to LCD
    @param digit single-digit unsigned number to be displayed on LCD
    */
    static constexpr void putDigit(const uint8_t digit)
    {
        putc(digit + 0x30);
    }

    /**
    @brief Put formatted number to LCD (right-aligned)
    @param digit unsigned number (0..255) to be displayed on LCD
    @param zeroChar Character to be used for prepending zeros (default is blank)
    */
    static constexpr void putNum(uint8_t number, const char zeroChar = ' ')
    {
        // First digit (100)
        uint8_t digit100 = 0;
        if (number >= 100)
        {
            digit100 = div<100>(number);
            putDigit(digit100);
        }
        else
        {
            putc(zeroChar);
        }

        // Second digit (10)
        uint8_t digit10 = 0;
        if (number >= 10)
        {
            number -= 100 * digit100;
            digit10 = div<10>(number);
            putDigit(digit10);
        }
        else
        {
            putc(zeroChar);
        }

        // Third digit (1)
        number -= 10 * digit10;
        putDigit(number);
    }

    /**
    @brief Put formatted number to LCD (right-aligned)
    @param number signed number (-128..127) to be displayed on LCD
    */
    static constexpr void putNum(const int8_t number)
    {
        const bool negative = number < 0;
        uint8_t absolute = negative ? -number : number;

        if (negative && (absolute >= 100))
        putc('-');
        else
        putc(' ');

        // First digit (100)
        uint8_t digit100 = 0;
        if (absolute >= 100)
        {
            digit100 = div<100>(absolute);
            putDigit(digit100);
        }
        else
        {
            if (negative && (absolute >= 10))
            putc('-');
            else
            putc(' ');
        }

        // Second digit (10)
        uint8_t digit10 = 0;
        if (absolute >= 10)
        {
            absolute -= 100 * digit100;
            digit10 = div<10>(absolute);
            putDigit(digit10);
        }
        else
        {
            if (negative)
            putc('-');
            else
            putc(' ');
        }

        // Third digit (1)
        absolute -= 10 * digit10;
        putDigit(absolute);
    }

    /**
    @brief Refresh the LCD, i.e. transfer the frame buffer to the LCD
    */
    static void refresh()
    {
        if (s_refresh)
        {
            // Iterate through all rows of the LCD
            uint8_t rowCnt = 0;
            for (auto & row : s_frameBuffer)
            {
                // Set Cursor to first column
                LCDAlphanumeric::setCursor(rowCnt++, 0);
                for (auto & Character : row)
                {
                    LCDAlphanumeric::putc(Character);
                }
            }
        }
    }

    private:

    static constexpr uint8_t getNofChars()
    {
        return getNofRows() * getNofColumns();
    }
    
    static char s_frameBuffer[getNofRows()][getNofColumns()];
    static uint8_t s_cursor;
    static bool s_refresh;
};

// Static initialization
template <typename LCDAlphanumeric>
char LCDAlphanumericBuffered<LCDAlphanumeric>::s_frameBuffer[getNofRows()][getNofColumns()];

template <typename LCDAlphanumeric>
uint8_t LCDAlphanumericBuffered<LCDAlphanumeric>::s_cursor;

template <typename LCDAlphanumeric>
bool LCDAlphanumericBuffered<LCDAlphanumeric>::s_refresh = true;

#endif


