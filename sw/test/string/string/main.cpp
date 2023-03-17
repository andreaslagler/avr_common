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

#include "string.h"

#include "../../common/debug_print.h"

bool test_assert(const char * str, const bool flag)
{
    cout << str;
    if (flag)
    {
        cout << static_cast<const char *>("PASSED");
    }
    else
    {
        cout << static_cast<const char *>("FAILED");
    }
    return flag;
}


int main(void)
{
    bool allPassed = true;
    bool testPassed = true;
    
    const char * const str = "Test";
    
    {
        testPassed = true;
        String<> x;
        testPassed &= x.empty();
    }
    allPassed &= test_assert("Default constructor", testPassed);
    
    {
        testPassed = true;
        String<> x(str);
        testPassed &= !x.empty();
        const char * it = str;
        for (char c : x)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed);

    {
        testPassed = true;
        String<> x(str, static_cast<size_t>(4));
        testPassed &= !x.empty();
        const char * it = str;
        for (char c : x)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed);

    {
        testPassed = true;
        String<> y(str);
        String<> x(y);
        testPassed &= !x.empty();
        const char * it = str;
        for (char c : x)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("Copy constructor", testPassed);

    {
        testPassed = true;
        String<> y(str);
        String<> x(move(y));
        testPassed &= y.empty();
        testPassed &= !x.empty();
        const char * it = str;
        for (char c : x)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("Move constructor", testPassed);

    {
        testPassed = true;
        String<> y(str);
        String<> x;
        x = y;
        testPassed &= !x.empty();
        const char * it = str;
        for (char c : x)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("Copy assignment", testPassed);

    {
        testPassed = true;
        String<> y(str);
        String<> x;
        x = move(y);
        testPassed &= y.empty();
        testPassed &= !x.empty();
        const char * it = str;
        for (char c : x)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("Move assignment", testPassed);

    {
        testPassed = true;
        String<> x(str);
        x.append(str);
        String<> y(str);
        testPassed &= x.size() == 2*y.size();
        auto it = x.cbegin();
        for (char c : y)
        {
            testPassed &= (c == *it);
            ++it;
        }
        for (char c : y)
        {
            testPassed &= (c == *it);
            ++it;
        }
    }
    allPassed &= test_assert("append", testPassed);

    {
        testPassed = true;
        String<> x(str);
        testPassed &= !x.empty();
        x.clear();
        testPassed &= x.empty();
    }
    allPassed &= test_assert("clear", testPassed);

    {
        testPassed = true;
        String<> x(str);
        testPassed &= x.size() == 4;
        testPassed &= x.capacity() == 5;
    }
    allPassed &= test_assert("size/capacity", testPassed);

    test_assert("OVERALL:", allPassed);
    
    while (true)
    {
    }
}

template <>
struct debugPrinter<const char*>
{
    static void print(const char* arg)
    {
        // Put a tracepoint here and display {arg, s} in output window
        (void)arg;
    }
};
