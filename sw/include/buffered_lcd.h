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
#include "pgm_string.h"

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
        for (char & c : s_frameBuffer.buffer)
        {
            c = ' ';
        }

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
            s_frameBuffer.buffer[s_cursor++] = character;
        }
    }

    /**
    @brief Put terminated string to LCD
    @param string Zero-terminated string to displayed on LCD (stored in RAM)
    */
    static constexpr void put(const String<>& str)
    {
        for (const auto& character : str)
        {
            if (character != 0)
            {
                putc(character);
            }            
        }
    }

    /**
    @brief Put terminated string to LCD
    @param string Zero-terminated string to displayed on LCD (stored in PROG_MEM)
    */
    static constexpr void put(const PgmString& string)
    {
        for (const auto& character : string)
        {
            if (character != 0)
            {
                putc(character);
            }            
        }
    }

    /**
    @brief Put digit to LCD
    @param digit single-digit unsigned number to be displayed on LCD
    */
    static constexpr void putDigit(const uint8_t digit)
    {
        putc(convertDigitToChar(digit));
    }
    
    static constexpr char convertDigitToChar(const uint8_t digit)
    {
        return static_cast<char>(digit + 0x30);
    }
    

    /**
    @brief Put formatted number to LCD (right-aligned)
    @param digit unsigned number (0..255) to be displayed on LCD
    @param zeroChar Character to be used for prepending zeros (default is blank)
    */
    static constexpr void putNum(uint8_t number, const char zeroChar = ' ')
    {
        char digitChar = zeroChar;
        uint8_t digit = 0;
        
        // First digit (100)
        if (number >= 100)
        {
            digit = div<100>(number);
            digitChar = convertDigitToChar(digit);
        }
        putc(digitChar);

        // Second digit (10)
        if (number >= 10)
        {
            number -= 100 * digit;
            digit = div<10>(number);
            digitChar = convertDigitToChar(digit);
        }
        putc(digitChar);

        // Third digit (1)
        number -= 10 * digit;
        putDigit(number);
    }

    /**
    @brief Put formatted 2-digit number to LCD (right-aligned)
    @param digit unsigned number (0..99) to be displayed on LCD
    @param zeroChar Character to be used for prepending zeros (default is blank)
    */
    static constexpr void putNum2(uint8_t number, const char zeroChar = ' ')
    {
        if (number < 100)
        {
            char digitChar = zeroChar;
            uint8_t digit = 0;

            // First digit (10)
            if (number >= 10)
            {
                digit = div<10>(number);
                digitChar = convertDigitToChar(digit);
            }
            putc(digitChar);

            // Second digit (1)
            number -= 10 * digit;
            putDigit(number);
        }
        else
        {
            putc('-');
            putc('-');
            return;
        }
    }

    /**
    @brief Put formatted 1-digit number to LCD (right-aligned)
    @param digit unsigned number (0..9) to be displayed on LCD
    @param zeroChar Character to be used for prepending zeros (default is blank)
    */
    static constexpr void putNum1(uint8_t number)
    {
        const char digitChar = (number < 10) ? convertDigitToChar(number) : '-';
        putc(digitChar);
    }

    /**
    @brief Put formatted number to LCD (right-aligned)
    @param number signed number (-128..127) to be displayed on LCD
    */
    static constexpr void putNum(const int8_t number)
    {
        const bool negative = number < 0;
        uint8_t absolute = negative ? -number : number;

        char digitChar = ' ';
        if (negative && (absolute >= 100))
        {
            digitChar = '-';
        }
        putc(digitChar);

        // First digit (100)
        uint8_t digit = 0;
        if (absolute >= 100)
        {
            digit = div<100>(absolute);
            digitChar = convertDigitToChar(digit);
        }
        else
        {
            if (negative && (absolute >= 10))
            {
                digitChar = '-';
            }
        }
        putc(digitChar);

        // Second digit (10)
        if (absolute >= 10)
        {
            absolute -= 100 * digit;
            digit = div<10>(absolute);
            digitChar = convertDigitToChar(digit);
        }
        else
        {
            if (negative)
            {
                digitChar = '-';
            }
        }
        putc(digitChar);

        // Third digit (1)
        absolute -= 10 * digit;
        putDigit(absolute);
    }

    /**
    @brief Put formatted number to LCD (right-aligned)
    @param digit unsigned number (0..65535) to be displayed on LCD
    @param zeroChar Character to be used for prepending zeros (default is blank)
    */
    static constexpr void putNum(uint16_t number, const char zeroChar = ' ')
    {
        char digitChar = zeroChar;
        uint8_t digit = 0;

        // First digit (10000)
        if (number >= 10000)
        {
            // digit = div<10000>(number); TODO
            digit = number / 10000;
            digitChar = convertDigitToChar(digit);
        }
        putc(digitChar);

        // Second digit (1000)
        if (number >= 1000)
        {
            number -= 10000 * digit;
            //digit = div<1000>(number); TODO
            digit = number / 1000;
            digitChar = convertDigitToChar(digit);
        }
        putc(digitChar);

        // Third digit (100)
        if (number >= 100)
        {
            number -= 1000 * digit;
            //digit = div<100>(number);
            digit = number / 100;
            digitChar = convertDigitToChar(digit);
        }
        putc(digitChar);

        // Fourth digit (10)
        if (number >= 10)
        {
            number -= 100 * digit;
            //digit = div<10>(number);
            digit = number / 10;
            digitChar = convertDigitToChar(digit);
        }
        putc(digitChar);

        // Fifth digit (1)
        number -= 10 * digit;
        putDigit(number);
    }

    /**
    @brief Put formatted number to LCD (right-aligned)
    @param digit unsigned number (0..255) to be displayed on LCD
    @param zeroChar Character to be used for prepending zeros (default is blank)
    */
    static constexpr void putNum3(uint16_t number, const char zeroChar = ' ')
    {
        char digitChar = zeroChar;
        uint8_t digit = 0;

        if (number < 1000)
        {
            // First digit (100)
            if (number >= 100)
            {
                //digit = div<100>(number); TODO
                digit = number / 100;
                digitChar = convertDigitToChar(digit);
            }
            putc(digitChar);

            // Second digit (10)
            if (number >= 10)
            {
                number -= 100 * digit;
                //digit = div<10>(number); TODO
                digit = number / 10;
                digitChar = convertDigitToChar(digit);
            }
            putc(digitChar);

            // Third digit (1)
            number -= 10 * digit;
            putDigit(number);
        }
        else
        {
            putc('-');
            putc('-');
            putc('-');
            return;
        }
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
            for (auto & row : s_frameBuffer.rows)
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
    
    static constexpr void newLine()
    {
        const uint8_t rowIdx = getRow() + 1;
        setCursor((rowIdx < getNofRows()) ? rowIdx : getNofRows(), 0);
    }        

    private:

    static constexpr uint8_t getRow()
    {
        return div<getNofColumns()>(s_cursor);
    }
    
    static constexpr uint8_t getNofChars()
    {
        return getNofRows() * getNofColumns();
    }
    
    //static char s_frameBuffer[getNofRows()][getNofColumns()];
    typedef union
    {
        char buffer[getNofChars()];
        char rows[getNofRows()][getNofColumns()];
    } FrameBuffer;
    
    static FrameBuffer s_frameBuffer;
    
    static uint8_t s_cursor;
    static bool s_refresh;
};

// Static initialization
template <typename LCDAlphanumeric>
typename LCDAlphanumericBuffered<LCDAlphanumeric>::FrameBuffer LCDAlphanumericBuffered<LCDAlphanumeric>::s_frameBuffer;

template <typename LCDAlphanumeric>
uint8_t LCDAlphanumericBuffered<LCDAlphanumeric>::s_cursor;

template <typename LCDAlphanumeric>
bool LCDAlphanumericBuffered<LCDAlphanumeric>::s_refresh = true;

#endif


