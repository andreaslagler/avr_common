/*
Copyright (C) 2023  Andreas Lagler

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

#ifndef TO_STRING_H
#define TO_STRING_H

#include <bits/format_spec.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pgm_string.h>
#include <div.h>

namespace to_string_helper
{
    // Count digits of a uint8_t value, considering the '+' sign (if needed)
    constexpr static uint8_t countDigits(const uint8_t number, const bool showPos)
    {
        // uint8_t has at least one digit
        uint8_t nofDigits = 1;
        
        // Second digit
        if (number >= 10)
        {
            ++nofDigits;
        }

        // Third digit
        if (number >= 100)
        {
            ++nofDigits;
        }
        
        // sign character
        if (showPos)
        {
            ++nofDigits;
        }

        return nofDigits;
    }
    
    // Count digits of an int8_t value, considering the '+' sign (if needed)
    constexpr static uint8_t countDigits(int8_t number, const bool showPos)
    {
        // int8_t has at least one digit
        uint8_t nofDigits = 1;
        
        // Check sign of number
        if (number < 0)
        {
            number = -number;
            ++nofDigits;
        }
        else if (showPos)
        {
            ++nofDigits;
        }

        // Second digit
        if (number >= 10)
        {
            ++nofDigits;
        }

        // Third digit
        if (number >= 100)
        {
            ++nofDigits;
        }
        
        return nofDigits;
    }
    
    // Count digits of a uint16_t value, considering the '+' sign (if needed)
    constexpr static uint8_t countDigits(const uint16_t number, const bool showPos)
    {
        // uint16_t has at least one digit
        uint8_t nofDigits = 1;
        
        // Second digit
        if (number >= 10)
        {
            ++nofDigits;
        }

        // Third digit
        if (number >= 100)
        {
            ++nofDigits;
        }

        // Fourth digit
        if (number >= 1000)
        {
            ++nofDigits;
        }

        // Fifth digit
        if (number >= 10000)
        {
            ++nofDigits;
        }
        
        // Sign character
        if (showPos)
        {
            ++nofDigits;
        }

        return nofDigits;
    }
    
    // Count fill characters
    constexpr static uint8_t countFillChars(const uint8_t width, const uint8_t nofDigits)
    {
        // Check desired width of next field
        if (width > nofDigits)
        {
            // Width not exceeded --> at least one fill character
            return width - nofDigits;
        }
        else
        {
            // Width exceeded --> no fill characters
            return static_cast<uint8_t>(0);
        }
    }
    
    // Make character upper case
    constexpr static char toUppercase(const char c)
    {
        // Check for lower case letters
        if(c >= 'a' && c <= 'z')
        {
            // If lower case, add difference to corresponding upper case letter
            return c + ('A' - 'a');
        }
        else
        {
            return c;
        }
    }
    
    // Put character to a string implementation declaring a method pushBack()
    template <typename StringImpl>
    constexpr void putChar(StringImpl& str, const char c)
    {
        // Append character to string
        str.pushBack(c);
    }
    
    // Format and put character to a string implementation declaring a method pushBack()
    template <typename StringImpl>
    constexpr void putChar(StringImpl& str, char c, const FormatSpec& formatSpec)
    {
        // Make character upper case if needed
        if (formatSpec.m_upperCase == upperCase)
        {
            c = toUppercase(c);
        }

        // Put character to string
        putChar(str, c);
    }
    
    // Put fill character to a string
    template <typename StringImpl>
    constexpr void putFillChars(StringImpl& str, uint8_t nofChars, const char c)
    {
        while (nofChars--)
        {
            putChar(str, c);
        }
    }
    
    // Put a digit to a string
    template <typename StringImpl>
    constexpr void putDigit(StringImpl& str, const uint8_t value)
    {
        // Add offset to digit to get the corresponding letter
        putChar(str, static_cast<char>(value + '0'));
    }
};

/**
@brief Convert boolean to string
@tparam StringImpl Used string implementation
@param str String implementation
@param value Boolean value to convert to string
@formatSpec Format specification to be used for conversion
*/
template <typename StringImpl>
constexpr inline void toString(StringImpl& str, const bool value, const FormatSpec& formatSpec)
{
    if (formatSpec.m_boolAlpha == boolAlpha)
    {
        // Use alphanumeric representation of bool
        if (value)
        {
            toString(str, "true"_pgm, formatSpec);
        }
        else
        {
            toString(str, "false"_pgm, formatSpec);
        }
    }
    else
    {
        // Use numeric representation of bool
        if (value)
        {
            toString(str, static_cast<uint8_t>(1), formatSpec);
        }
        else
        {
            toString(str, static_cast<uint8_t>(0), formatSpec);
        }
    }
}

/**
@brief Convert uint8_t number to string
@tparam StringImpl Used string implementation
@param str String implementation
@param value Number to convert to string
@formatSpec Format specification to be used for conversion
*/
template <typename StringImpl>
constexpr inline void toString(StringImpl& str, uint8_t value, const FormatSpec& formatSpec)
{
    // Calculate number of digits
    const uint8_t nofDigits = to_string_helper::countDigits(value, formatSpec.m_showPos == showPos);
    
    // Calculate number of fill characters
    const uint8_t nofFillChars = to_string_helper::countFillChars(formatSpec.m_width, nofDigits);
    
    // Insert fill characters for right alignment
    if (formatSpec.m_alignment == rightAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
    
    // Insert sign
    if (formatSpec.m_showPos == showPos)
    {
        to_string_helper::putChar(str, '+');
    }
    
    // Insert actual digits
    uint8_t digit = 0;
    
    if (value >= 100)
    {
        digit = div<100>(static_cast<uint8_t>(value));
        to_string_helper::putDigit(str, digit);
    }
    
    if (value >= 10)
    {
        value -= 100 * digit;
        digit = div<10>(static_cast<uint8_t>(value));
        to_string_helper::putDigit(str, digit);
        value -= 10 * digit;
    }

    to_string_helper::putDigit(str, value);

    // Insert fill characters for left alignment
    if (formatSpec.m_alignment == leftAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
}

/**
@brief Convert int8_t number to string
@tparam StringImpl Used string implementation
@param str String implementation
@param value Number to convert to string
@formatSpec Format specification to be used for conversion
*/
template <typename StringImpl>
constexpr inline void toString(StringImpl& str, int8_t value, const FormatSpec& formatSpec)
{
    // Calculate number of digits
    const uint8_t nofDigits = to_string_helper::countDigits(value, formatSpec.m_showPos == showPos);
    
    // Calculate number of fill characters
    const uint8_t nofFillChars = to_string_helper::countFillChars(formatSpec.m_width, nofDigits);
    
    // Insert fill characters for right alignment
    if (formatSpec.m_alignment == rightAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
    
    // Insert sign
    if (value < 0)
    {
        value = -value;
        to_string_helper::putChar(str, '-');
    }
    else if (formatSpec.m_showPos == showPos)
    {
        to_string_helper::putChar(str, '+');
    }
    
    // Insert digits
    uint8_t digit = 0;
    
    if (value >= 100)
    {
        digit = div<100>(static_cast<uint8_t>(value));
        to_string_helper::putDigit(str, digit);
    }
    
    if (value >= 10)
    {
        value -= 100 * digit;
        digit = div<10>(static_cast<uint8_t>(value));
        to_string_helper::putDigit(str, digit);
        value -= 10 * digit;
    }

    to_string_helper::putDigit(str, value);

    // Insert fill characters for left alignment
    if (formatSpec.m_alignment == leftAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
}

/**
@brief Convert uint16_t number to string
@tparam StringImpl Used string implementation
@param str String implementation
@param value Number to convert to string
@formatSpec Format specification to be used for conversion
*/
template <typename StringImpl>
constexpr inline void toString(StringImpl& str, uint16_t value, const FormatSpec& formatSpec)
{
    // Calculate number of digits
    const uint8_t nofDigits = to_string_helper::countDigits(value, formatSpec.m_showPos == showPos);
    
    // Calculate number of fill characters
    const uint8_t nofFillChars = to_string_helper::countFillChars(formatSpec.m_width, nofDigits);
    
    // Insert fill characters for right alignment
    if (formatSpec.m_alignment == rightAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
    
    // Insert sign
    if (formatSpec.m_showPos == showPos)
    {
        to_string_helper::putChar(str, '+');
    }
    
    // Insert actual characters
    uint8_t digit = 0;
    
    if (value >= 10000)
    {
        digit = static_cast<uint8_t>(div<10000>(value));
        to_string_helper::putDigit(str, digit);
    }
    
    if (value >= 1000)
    {
        value -= 10000 * digit;
        digit = static_cast<uint8_t>(div<1000>(value));
        to_string_helper::putDigit(str, digit);
    }
    
    if (value >= 100)
    {
        value -= 1000 * digit;
        digit = static_cast<uint8_t>(div<100>(value));
        to_string_helper::putDigit(str, digit);
    }
    
    if (value >= 10)
    {
        value -= 100 * digit;
        digit = div<10>(static_cast<uint8_t>(value));
        to_string_helper::putDigit(str, digit);
        value -= 10 * digit;
    }

    to_string_helper::putDigit(str, static_cast<uint8_t>(value));

    // Insert fill characters for left alignment
    if (formatSpec.m_alignment == leftAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
}

/**
@brief Convert a string to another string
@tparam StringImpl Used string implementation
@tparam Allocator Allocator used in source String object
@param str String implementation
@param arg String object to convert to string
@formatSpec Format specification to be used for conversion
*/
template <typename StringImpl, typename Allocator>
constexpr void toString(StringImpl& str, const String<Allocator>& arg, const FormatSpec& formatSpec)
{
    // Calculate number of digits
    const size_t nofChars = arg.size() ;
    
    // Calculate number of fill characters
    const uint8_t nofFillChars = to_string_helper::countFillChars(formatSpec.m_width, nofChars);
    
    // Insert fill characters for right alignment
    if (formatSpec.m_alignment == rightAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
    
    // Insert actual string
    for (const char c : arg)
    {
        to_string_helper::putChar(str, c, formatSpec);
    }

    // Insert fill characters for left alignment
    if (formatSpec.m_alignment == leftAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
}

/**
@brief Convert a progmem string to another string
@tparam StringImpl Used string implementation
@param str String implementation
@param arg Source PgmString object to convert to string
@formatSpec Format specification to be used for conversion
*/
template <typename StringImpl>
constexpr void toString(StringImpl& str, const PgmString& arg, const FormatSpec& formatSpec)
{
    // Calculate number of digits
    const size_t nofChars = arg.size() ;
    
    // Calculate number of fill characters
    const uint8_t nofFillChars = to_string_helper::countFillChars(formatSpec.m_width, nofChars);
    
    // Insert fill characters for right alignment
    if (formatSpec.m_alignment == rightAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
    
    // Insert actual string
    for (const char c : arg)
    {
        to_string_helper::putChar(str, c, formatSpec);
    }

    // Insert fill characters for left alignment
    if (formatSpec.m_alignment == leftAlign)
    {
        to_string_helper::putFillChars(str, nofFillChars, formatSpec.m_fillChar);
    }
}

#endif