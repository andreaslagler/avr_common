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
#include <static_deque.h>
#include <static_string.h>

/**
@brief Driver for buffered operation of an alphanumeric LCD.
@tparam LCDAlphanumeric Underlying alphanumeric LCD device driver.
This driver class needs to implement the following static methods:
void init()
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
    }

    /**
    @brief Clear the LCD
    */
    static void clear()
    {
        s_frameBuffer.clear();
        refresh();       
    }

    /**
    @brief Refresh the LCD, i.e. transfer the frame buffer to the LCD
    */
    static void refresh()
    {
        if (s_refresh)
        {
            // Iterate through all rows of the frame buffer
            uint8_t rowCnt = 0;
            for (auto & row : s_frameBuffer.m_buffer)
            {
                // Set Cursor to first column
                LCDAlphanumeric::setCursor(rowCnt++, 0);
                for (auto & c : row)
                {
                    LCDAlphanumeric::putc(c);
                }
            }
            
            // Add blank lines
            while (rowCnt < getNofRows())
            {
                LCDAlphanumeric::setCursor(rowCnt++, 0);
                for (uint8_t colCnt = 0; colCnt < getNofColumns(); ++colCnt)
                {
                    LCDAlphanumeric::putc(' ');
                }
            }
        }
    }
    
    /**
    @brief LCD frame buffer
    This class meets the requirements of a string implementation to be used with StringStream
    */
    class FrameBuffer
    {
        public:
        
        /**
        @brief Constructor
        */
        constexpr FrameBuffer()
        {
            clear();
        }
        
        /**
        @brief Clear the frame buffer
        */
        constexpr void clear()
        {
            // Clear underlying memory
            m_buffer.clear();
            
            // Set the cursor to end of the line to make sure a new line is inserted before the next character
            m_cursor = getNofColumns();
        }

        /**
        @brief Put character to LCD
        @param character Character to displayed on LCD
        */
        constexpr void pushBack(const char c)
        {
            if (c == '\n')
            {
                newLine();
            }
            else
            {
                if (m_cursor >= getNofColumns())
                {
                    newLine();
                }
                m_buffer.back()[m_cursor++] = c;
            }
            s_refresh = true;
        }

        private:
        
        friend class LCDAlphanumericBuffered<LCDAlphanumeric>;
        
        uint8_t m_cursor = 0;
        
        constexpr void newLine()
        {
            if (m_buffer.size() >= getNofRows())
            {
                m_buffer.popFront();
            }
            m_buffer.emplaceBack(getNofColumns(), ' ');
            m_cursor = 0;
        }
        
        StaticDeque<StaticString<getNofColumns()>, getNofRows()> m_buffer;
    };
    
    static constexpr FrameBuffer& getBuffer()
    {
        return s_frameBuffer;
    }
    
    private:
    
    static FrameBuffer s_frameBuffer;
    
    static bool s_refresh;
};

// Static initialization
template <typename LCDAlphanumeric>
typename LCDAlphanumericBuffered<LCDAlphanumeric>::FrameBuffer LCDAlphanumericBuffered<LCDAlphanumeric>::s_frameBuffer;

template <typename LCDAlphanumeric>
bool LCDAlphanumericBuffered<LCDAlphanumeric>::s_refresh = true;

#endif
