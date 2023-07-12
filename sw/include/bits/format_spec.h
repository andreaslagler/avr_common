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

#ifndef FORMAtT_SPEC_H
#define FORMAtT_SPEC_H

#include <stdint.h>
#include <stdbool.h>

struct FormatSpec
{
    struct BoolAlpha
    {
        bool m_value;
        bool operator==(const BoolAlpha& rhs) const {return m_value == rhs.m_value;}
    };
    
    static constexpr BoolAlpha noBoolAlpha = {false};
    static constexpr BoolAlpha boolAlpha = {true};
    BoolAlpha m_boolAlpha = noBoolAlpha;
    
    struct UpperCase
    {
        bool m_value;
        bool operator==(const UpperCase& rhs) const {return m_value == rhs.m_value;}
    };
    static constexpr UpperCase noUpperCase = {false};
    static constexpr UpperCase upperCase = {true};
    UpperCase m_upperCase = noUpperCase;
    
    struct ShowPos
    {
        bool m_value;
        bool operator==(const ShowPos& rhs) const {return m_value == rhs.m_value;}
    };
    static constexpr ShowPos noShowPos = {false};
    static constexpr ShowPos showPos = {true};
    ShowPos m_showPos = noShowPos;
    
    struct Alignment
    {
        bool m_value;
        bool operator==(const Alignment& rhs) const {return m_value == rhs.m_value;}
    };
    static constexpr Alignment leftAlign = {false};
    static constexpr Alignment rightAlign = {true};
    Alignment m_alignment = leftAlign;

    struct FillChar
    {
        char m_char = ' ';
    };
    
    static constexpr FillChar fillChar(const char c)
    {
        return {c};
    }
    
    char m_fillChar = ' ';
    
    struct Width
    {
        uint8_t m_value = 0;
    };
    
    static constexpr Width width(const uint8_t value)
    {
        return {value};
    }
    
    uint8_t m_width = 0;
};


static constexpr auto noBoolAlpha = FormatSpec::noBoolAlpha;
static constexpr auto boolAlpha = FormatSpec::boolAlpha;
static constexpr auto noUpperCase = FormatSpec::noUpperCase;
static constexpr auto upperCase = FormatSpec::upperCase;
static constexpr auto noShowPos = FormatSpec::noShowPos;
static constexpr auto showPos = FormatSpec::showPos;
static constexpr auto leftAlign = FormatSpec::leftAlign;
static constexpr auto rightAlign = FormatSpec::rightAlign;

static constexpr inline FormatSpec::FillChar setFill(const char c)
{
    return FormatSpec::fillChar(c);
}

static constexpr inline FormatSpec::Width setWidth(const uint8_t value)
{
    return FormatSpec::width(value);
}

#endif