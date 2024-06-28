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

#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include <bits/format_spec.h>
#include <to_string.h>

/**
@brief String stream class
StringStream implements output operations on a string-based stream. 
At the low level, StringStream wraps a raw string implementation. 
@tparam StringImpl String implementation defining a method pushBack(const char) 
*/
template <typename StringImpl>
class StringStream
{
    public:

    /**
    @brief Non-standard constructor
    @param stringImpl String implementation to stream to
    */
    constexpr explicit StringStream(StringImpl& stringImpl) : m_stringImpl(stringImpl)
    {}

    /**
    @brief Access underlying string implementation
    @result Reference to underlying string implementation
    */
    constexpr StringImpl& str()
    {
        return m_stringImpl;
    }

    /**
    @brief Read-only access underlying string implementation
    @result Const reference to underlying string implementation
    */
    constexpr const StringImpl& str() const
    {
        return m_stringImpl;
    }

    /**
    @brief Clear the string stream
    Clears the string stream and its underlying string implementation
    */
    constexpr void clear()
    {
        m_stringImpl.clear();
        m_formatSpec = FormatSpec();
    }

    /**
    @brief Stream operator for format specification
    @param spec Format specification for booleans
    @result Reference to this object
    */
    constexpr StringStream& operator<<(const FormatSpec::BoolAlpha& spec)
    {
        m_formatSpec.m_boolAlpha = spec;
        return *this;
    }
    
    /**
    @brief Stream operator for format specification
    @param spec Format specification for upper case letter
    @result Reference to this object
    */
    constexpr StringStream& operator<<(const FormatSpec::UpperCase& spec)
    {
        m_formatSpec.m_upperCase = spec;
        return *this;
    }

    /**
    @brief Stream operator for format specification
    @param spec Format specification for alignment
    @result Reference to this object
    */
    constexpr StringStream& operator<<(const FormatSpec::Alignment& spec)
    {
        m_formatSpec.m_alignment = spec;
        return *this;
    }

    /**
    @brief Stream operator for format specification
    @param spec Format specification for filling character
    @result Reference to this object
    */
    constexpr StringStream& operator<<(const FormatSpec::FillChar& spec)
    {
        m_formatSpec.m_fillChar = spec.m_char;
        return *this;
    }

    /**
    @brief Stream operator for format specification
    @param spec Format specification for (numerical) field width
    @result Reference to this object
    */
    constexpr StringStream& operator<<(const FormatSpec::Width& spec)
    {
        m_formatSpec.m_width = spec.m_value;
        return *this;
    }

    /**
    @brief Stream operator
    @tparam T Type to stream to string
    @param value Value to stream to string
    @result Reference to this object
    */
    template <typename T>
    constexpr StringStream& operator <<(const T& value)
    {
        toString(m_stringImpl, value, m_formatSpec);
        m_formatSpec.m_width = 0;
        return *this;
    }

    private:

    // No copy constructor and copy assignment
    StringStream(const StringStream&) = delete;
    StringStream& operator=(const StringStream&) = delete;
    
    // Stream buffer
    StringImpl& m_stringImpl;
    
    // Format specifier
    FormatSpec m_formatSpec;
};

#endif