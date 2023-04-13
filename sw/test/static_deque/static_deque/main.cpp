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

#include <static_deque.h>
#include "../../common/debug_print.h"

class Test
{
    public:
    
    Test()
    {
        ++s_NofDefaultCtorCalls;
    }
    
    Test(const uint8_t value) : m_value(value)
    {
        ++s_NofInitCtorCalls;
    }

    Test(const Test& other) : m_value(other.m_value)
    {
        ++s_NofCopyCtorCalls;
    }

    Test(Test&& other) : m_value(forward<uint8_t>(other.m_value))
    {
        ++s_NofMoveCtorCalls;
    }
    
    ~Test()
    {
        ++s_NofDtorCalls;
    }
    
    Test& operator=(const Test& other)
    {
        m_value = other.m_value;
        return *this;
    }
    
    bool operator==(const Test& other) const
    {
        return m_value == other.m_value;
    }
    
    uint8_t getValue() const
    {
        return m_value;
    }
    
    static void resetCounter()
    {
        s_NofDefaultCtorCalls = 0;
        s_NofInitCtorCalls = 0;
        s_NofCopyCtorCalls = 0;
        s_NofMoveCtorCalls = 0;
        s_NofDtorCalls = 0;
    }
    
    static bool check(
    const uint16_t nofDefaultCtorCalls,
    const uint16_t nofInitCtorCalls,
    const uint16_t nofCopyCtorCalls,
    const uint16_t nofMoveCtorCalls,
    const uint16_t nofDtorCalls)
    {
        auto _nofDefaultCtorCalls = s_NofDefaultCtorCalls;
        auto _nofInitCtorCalls = s_NofInitCtorCalls;
        auto _nofCopyCtorCalls = s_NofCopyCtorCalls;
        auto _nofMoveCtorCalls = s_NofMoveCtorCalls;
        auto _nofDtorCalls = s_NofDtorCalls;
        
        bool ok = true;
        ok &= nofDefaultCtorCalls == s_NofDefaultCtorCalls;
        ok &= nofInitCtorCalls == s_NofInitCtorCalls;
        ok &= nofCopyCtorCalls == s_NofCopyCtorCalls;
        ok &= nofMoveCtorCalls == s_NofMoveCtorCalls;
        ok &= nofDtorCalls == s_NofDtorCalls;
        return ok;
    }
    
    private:

    static uint16_t s_NofDefaultCtorCalls;
    static uint16_t s_NofInitCtorCalls;
    static uint16_t s_NofCopyCtorCalls;
    static uint16_t s_NofMoveCtorCalls;
    static uint16_t s_NofDtorCalls;
    
    uint8_t m_value = 0;
};

// static init
uint16_t Test::s_NofDefaultCtorCalls = 0;
uint16_t Test::s_NofInitCtorCalls = 0;
uint16_t Test::s_NofCopyCtorCalls = 0;
uint16_t Test::s_NofMoveCtorCalls = 0;
uint16_t Test::s_NofDtorCalls = 0;

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

constexpr size_t testCapacity = 10;

int main(void)
{
    bool allPassed = true;
    bool testPassed = true;
    
    const std::initializer_list<Test> testInit({42,43,44});
    const StaticDeque<Test, testCapacity> testStaticDeque(testInit);
    
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x;
    }
    allPassed &= test_assert("Default constructor", testPassed && Test::check(0,0,0,0,0));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2);
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == 0;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(2,0,0,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2, *testInit.begin());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testInit.begin()).getValue();
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,2,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testInit.begin(), testInit.end());
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        auto it = testStaticDeque.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> y(testInit);
        StaticDeque<Test, testCapacity> x(move(y));
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Move constructor", testPassed && Test::check(0,0,3,3,6));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2);
        x = testStaticDeque;
        auto it = testStaticDeque.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy assignment operator", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x;
        StaticDeque<Test, testCapacity> y(testInit);
        x = move(y);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Move assignment operator", testPassed && Test::check(0,0,3,3,6));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2);
        x = testInit;
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy assignment operator", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2);
        x.assign(testStaticDeque.begin(), testStaticDeque.end());
        auto it = testStaticDeque.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2);
        x.assign(3,*testStaticDeque.begin());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testStaticDeque.begin()).getValue();
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(2);
        x.assign(testInit);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,3,0,5));

    {
        testPassed = true;
        auto it = testInit.begin();
        auto itBegin = testStaticDeque.begin();
        auto itEnd = testStaticDeque.end();
        while (itBegin != itEnd)
        {
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++it;
            ++itBegin;
        }
    }
    allPassed &= test_assert("begin() / end()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.begin();
        auto itBegin = x.begin();
        auto itEnd = x.end();
        while (itBegin != itEnd)
        {
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++it;
            ++itBegin;
        }
    }
    allPassed &= test_assert("begin() / end()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.begin();
        auto itBegin = x.cbegin();
        auto itEnd = x.cend();
        while (itBegin != itEnd)
        {
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++it;
            ++itBegin;
        }
    }
    allPassed &= test_assert("cbegin() / cend()", testPassed);
    
    {
        testPassed = true;
        auto it = testInit.end();
        auto itBegin = testStaticDeque.rbegin();
        auto itEnd = testStaticDeque.rend();
        while (itBegin != itEnd)
        {
            --it;
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++itBegin;
        }
    }
    allPassed &= test_assert("rbegin() / rend()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.end();
        auto itBegin = x.rbegin();
        auto itEnd = x.rend();
        while (itBegin != itEnd)
        {
            --it;
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++itBegin;
        }
    }
    allPassed &= test_assert("rbegin() / rend()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.end();
        auto itBegin = x.crbegin();
        auto itEnd = x.crend();
        while (itBegin != itEnd)
        {
            --it;
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++itBegin;
        }
    }
    allPassed &= test_assert("crbegin() / crend()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(2);
        testPassed &= !x.empty();
        StaticDeque<Test, testCapacity> y;
        testPassed &= y.empty();
    }
    allPassed &= test_assert("empty()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        testPassed &= testInit.size() == x.size();
        StaticDeque<Test, testCapacity> y;
        testPassed &= 0 == y.size();
    }
    allPassed &= test_assert("size()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        Test front = *testInit.begin();
        testPassed &= front.getValue() == x.front().getValue();
        testPassed &= front.getValue() == testStaticDeque.front().getValue();
    }
    allPassed &= test_assert("front()", testPassed);
    
    {
        testPassed = true;
        StaticDeque<Test, testCapacity> x(testInit);
        Test back = *(testInit.begin() + testInit.size() - 1);
        testPassed &= back.getValue() == x.back().getValue();
        testPassed &= back.getValue() == testStaticDeque.back().getValue();
    }
    allPassed &= test_assert("back()", testPassed);
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testInit);
        x.resize(2);
        testPassed &= (x.size() == 2);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        x.resize(4);
        testPassed &= (x.size() == 4);
        it = testInit.begin();
        testPassed &= x[0].getValue() == (*it).getValue();
        ++it;
        testPassed &= x[1].getValue() == (*it).getValue();
        testPassed &= x[2].getValue() == 0;
        testPassed &= x[3].getValue() == 0;
    }
    allPassed &= test_assert("resize()", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testInit);
        x.resize(2, *testInit.begin());
        testPassed &= (x.size() == 2);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        x.resize(4, *testInit.begin());
        testPassed &= (x.size() == 4);
        it = testInit.begin();
        testPassed &= x[0].getValue() == (*it).getValue();
        ++it;
        testPassed &= x[1].getValue() == (*it).getValue();
        testPassed &= x[2].getValue() == (*testInit.begin()).getValue();
        testPassed &= x[3].getValue() == (*testInit.begin()).getValue();
    }
    allPassed &= test_assert("resize()", testPassed && Test::check(0,0,5,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.begin();
        for (StaticDeque<Test, testCapacity>::size_type idx = 0; idx < x.size(); ++idx)
        {
            testPassed &= x.at(idx).getValue() == (*it).getValue();
            testPassed &= x[idx].getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("at() / operator[]", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        const StaticDeque<Test, testCapacity> x(testInit);
        auto it = testInit.begin();
        for (StaticDeque<Test, testCapacity>::size_type idx = 0; idx < x.size(); ++idx)
        {
            testPassed &= x.at(idx).getValue() == (*it).getValue();
            testPassed &= x[idx].getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("at() / operator[]", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        x.emplaceFront(testStaticDeque.front().getValue());
        testPassed &= (x.size() == (testStaticDeque.size()+1));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testStaticDeque.front().getValue();
        for (const Test& t : testStaticDeque)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("emplaceFront()", testPassed && Test::check(0,1,3,0,4));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        x.emplaceBack(testStaticDeque.front().getValue());
        testPassed &= (x.size() == (testStaticDeque.size()+1));
        auto it = x.begin();
        for (const Test& t : testStaticDeque)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testStaticDeque.front().getValue();
    }
    allPassed &= test_assert("emplaceBack()", testPassed && Test::check(0,1,3,0,4));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        x.pushFront(testStaticDeque.front());
        testPassed &= (x.size() == (testStaticDeque.size()+1));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testStaticDeque.front().getValue();
        for (const Test& t : testStaticDeque)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("pushFront()", testPassed && Test::check(0,0,4,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        Test t = testStaticDeque.front();
        x.pushFront(move(t));
        testPassed &= (x.size() == (testStaticDeque.size()+1));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testStaticDeque.front().getValue();
        for (const Test& t : testStaticDeque)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("pushFront()", testPassed && Test::check(0,0,4,1,5));

    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        x.pushBack(testStaticDeque.back());
        testPassed &= (x.size() == (testStaticDeque.size()+1));
        auto it = x.begin();
        for (const Test& t : testStaticDeque)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testStaticDeque.back().getValue();
    }
    allPassed &= test_assert("pushBack()", testPassed && Test::check(0,0,4,0,4));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        Test t = testStaticDeque.back();
        x.pushBack(move(t));
        testPassed &= (x.size() == (testStaticDeque.size()+1));
        auto it = x.begin();
        for (const Test& t : testStaticDeque)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testStaticDeque.back().getValue();
    }
    allPassed &= test_assert("pushBack()", testPassed && Test::check(0,0,4,1,5));

    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        x.popFront();
        testPassed &= (x.size() == (testStaticDeque.size()-1));
        auto it = testStaticDeque.begin();
        for (const Test& t : x)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("popFront()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        x.popBack();
        testPassed &= (x.size() == (testStaticDeque.size()-1));
        auto it = testStaticDeque.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("popBack()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticDeque<Test, testCapacity> x(testStaticDeque);
        testPassed &= !x.empty();
        x.clear();
        testPassed &= x.empty();
    }
    allPassed &= test_assert("clear()", testPassed && Test::check(0,0,3,0,3));

    allPassed &= test_assert("OVERALL:", allPassed);

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

void throw_bad_alloc()
{
    cout << (const char*)"BAD ALLOC !!!";
    while(true);
}

void throw_length_error()
{
    cout << (const char*)"LENGTH ERROR !!!";
    while(true);
}

void throw_out_of_range()
{
    cout << (const char*)"RANGE ERROR !!!";
    while(true);
}
