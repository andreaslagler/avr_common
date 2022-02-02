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

#ifndef PAIR_H
#define PAIR_H

/**
@brief Template class implementing an associative pair container
@tparam First Type of first element of the pair
@tparam Second Type of second element of the pair
*/
template <typename First, typename Second>
class Pair
{
    public:

    /// @brief Constructor
    constexpr Pair() = default;

    /**
    @brief Constructor
    @param first Initial value of the first element of the pair
    @param second Initial value if the second element of the pair
    */
    constexpr Pair(const First first, const Second second)
    :
    m_first(first),
    m_second(second)
    {}

    /**
    @brief Read/Write access to first element of the pair
    @result Reference to first element of the pair
    */
    constexpr First & first()
    {
        return m_first;
    }

    /**
    @brief Read access to first element of the pair
    @result Reference to first element of the pair
    */
    constexpr const First & first() const
    {
        return m_first;
    }

    /**
    @brief Read/Write access to second element of the pair
    @result Reference to second element of the pair
    */
    constexpr Second & second()
    {
        return m_second;
    }

    /**
    @brief Read access to second element of the pair
    @result Reference to second element of the pair
    */
    constexpr const Second & second() const
    {
        return m_second;
    }

    private:

    First m_first;
    Second m_second;
};

#endif