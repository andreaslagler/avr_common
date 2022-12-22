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

#ifndef CXX_CONFIG_H
#define CXX_CONFIG_H

namespace std
{
    typedef __SIZE_TYPE__ size_t;
}


#if __cplusplus >= 201103L
#define SINCE_CXX11 1
#else
#define SINCE_CXX11 0
#endif

#if __cplusplus < 201103L
#define UNTIL_CXX11 1
#else
#define UNTIL_CXX11 0
#endif

// Macro for declaring member functions constexpr
#if SINCE_CXX11
#define CXX_CONSTEXPR constexpr
#else
#define CXX_CONSTEXPR
#endif


#if __cplusplus >= 201402L
#define SINCE_CXX14 1
#else
#define SINCE_CXX14 0
#endif

#if __cplusplus < 201402L
#define UNTIL_CXX14 1
#else
#define UNTIL_CXX14 0
#endif

// Macro for declaring member functions constexpr for C++14
#if SINCE_CXX14
#define CXX14_CONSTEXPR constexpr
#else
#define CXX14_CONSTEXPR
#endif


#if __cplusplus >= 201703L
#define SINCE_CXX17 1
#else
#define SINCE_CXX17 0
#endif

#if __cplusplus < 201703L
#define UNTIL_CXX17 1
#else
#define UNTIL_CXX17 0
#endif

// Macro for declaring member functions constexpr for C++17
#if SINCE_CXX17
#define CXX17_CONSTEXPR constexpr
#else
#define CXX17_CONSTEXPR
#endif


#if __cplusplus >= 202002L 
#define SINCE_CXX20 1
#else
#define SINCE_CXX20 0
#endif

#if __cplusplus < 202002L 
#define UNTIL_CXX20 1
#else
#define UNTIL_CXX20 0
#endif



// Macro for declaring member functions constexpr for C++20
#if SINCE_CXX20
#define CXX20_CONSTEXPR constexpr
#else
#define CXX20_CONSTEXPR
#endif


#if SINCE_CXX11
typedef decltype(nullptr) nullptr_t;
#endif


#endif