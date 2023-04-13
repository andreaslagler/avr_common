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

#include <static_list.h>

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
        //auto _nofDefaultCtorCalls = s_NofDefaultCtorCalls;
        //auto _nofInitCtorCalls = s_NofInitCtorCalls;
        //auto _nofCopyCtorCalls = s_NofCopyCtorCalls;
        //auto _nofMoveCtorCalls = s_NofMoveCtorCalls;
        //auto _nofDtorCalls = s_NofDtorCalls;
        
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


int main(void)
{
    bool allPassed = true;
    bool testPassed = true;
    
    const std::initializer_list<Test> testInit({42,43,44});
    const StaticList<Test,10> testList(testInit);
    
    // construct/copy/destroy
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x;
    }
    allPassed &= test_assert("Default constructor", testPassed && Test::check(0,0,0,0,0));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(2);
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == 0;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(2,0,0,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(2, *testInit.begin());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testInit.begin()).getValue();
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,2,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testInit.begin(), testInit.end());
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
        StaticList<Test,10> x(testList);
        auto it = testList.cbegin();
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
        StaticList<Test,10> x(testInit);
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
        StaticList<Test,10> y(testInit);
        StaticList<Test,10> x(move(y));
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
        StaticList<Test,10> x(2);
        x = testList;
        auto it = testList.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy assignment operator", testPassed && Test::check(2,0,1,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x;
        StaticList<Test,10> y(testInit);
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
        StaticList<Test,10> x(2);
        x = testInit;
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy assignment operator", testPassed && Test::check(2,0,1,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(2);
        x.assign(testList.begin(), testList.end());
        auto it = testList.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,1,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(2);
        x.assign(3,*testList.begin());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testList.begin()).getValue();
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,1,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(2);
        x.assign(testInit);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,1,0,3));
    
    {
        testPassed = true;
        auto it = testInit.begin();
        auto itBegin = testList.begin();
        auto itEnd = testList.end();
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
        StaticList<Test,10> x(testInit);
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
        StaticList<Test,10> x(testInit);
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
        auto itBegin = testList.rbegin();
        auto itEnd = testList.rend();
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
        StaticList<Test,10> x(testInit);
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
        StaticList<Test,10> x(testInit);
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
        StaticList<Test,10> x(2);
        testPassed &= !x.empty();
        StaticList<Test,10> y;
        testPassed &= y.empty();
    }
    allPassed &= test_assert("empty()", testPassed);
    
    {
        testPassed = true;
        StaticList<Test,10> x(testInit);
        testPassed &= testInit.size() == x.size();
        StaticList<Test,10> y;
        testPassed &= 0 == y.size();
    }
    allPassed &= test_assert("size()", testPassed);

    {
        testPassed = true;
        StaticList<Test,10> x(testInit);
        Test front = *testInit.begin();
        testPassed &= front.getValue() == x.front().getValue();
        testPassed &= front.getValue() == testList.front().getValue();
    }
    allPassed &= test_assert("front()", testPassed);

    {
        testPassed = true;
        StaticList<Test,10> x(testInit);
        Test back = *(testInit.begin() + testInit.size() - 1);
        testPassed &= back.getValue() == x.back().getValue();
        testPassed &= back.getValue() == testList.back().getValue();
    }
    allPassed &= test_assert("back()", testPassed);
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.emplaceFront(testList.front().getValue());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("emplaceFront()", testPassed && Test::check(0,1,3,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.emplaceBack(testList.front().getValue());
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testList.front().getValue();
    }
    allPassed &= test_assert("emplaceBack()", testPassed && Test::check(0,1,3,0,4));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.pushFront(testList.front());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("pushFront()", testPassed && Test::check(0,0,4,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        Test t = testList.front();
        x.pushFront(move(t));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("pushFront()", testPassed && Test::check(0,0,4,1,5));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.popFront();
        auto it = testList.begin();
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
        StaticList<Test,10> x(testList);
        x.pushBack(testList.back());
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testList.back().getValue();
    }
    allPassed &= test_assert("pushBack()", testPassed && Test::check(0,0,4,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        Test t = testList.back();
        x.pushBack(move(t));
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testList.back().getValue();
    }
    allPassed &= test_assert("pushBack()", testPassed && Test::check(0,0,4,1,5));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.popBack();
        auto it = testList.begin();
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
        StaticList<Test,10> x(testList);
        x.emplace(x.cbegin(), testList.front().getValue());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("emplace()", testPassed && Test::check(0,1,3,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.insert(x.cbegin(), testList.front());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("insert()", testPassed && Test::check(0,0,4,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        Test t = testList.front();
        x.insert(x.cbegin(), move(t));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("insert()", testPassed && Test::check(0,0,4,1,5));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.insert(x.cbegin(), 2, testList.front());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        ++it;
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("insert()", testPassed && Test::check(0,0,5,0,5));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x;
        x.insert(x.cbegin(), testList.begin(), testList.end());
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("insert()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x;
        x.insert(x.cbegin(), testInit);
        auto it = x.begin();
        for (const Test& t : testInit)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("insert()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.erase(x.cbegin());
        auto it = testList.begin();
        for (const Test& t : x)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("erase()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.erase(x.cbegin(), x.cend());
        auto it = testList.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("erase()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        testPassed &= !x.empty();
        x.clear();
        testPassed &= x.empty();
    }
    allPassed &= test_assert("clear()", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        testPassed &= 1 == x.remove(testList.front());
        testPassed &= 0 == x.remove(testList.front());
        auto it = testList.begin();
        for (const Test& t : x)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("remove()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        testPassed &= 1 == x.remove_if([](const Test& test){return test.getValue() == 42;});
        testPassed &= 0 == x.remove_if([](const Test& test){return test.getValue() == 42;});
        auto it = testList.begin();
        for (const Test& t : x)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("remove_if()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        StaticList<Test,10> x(testList);
        x.reverse();
        auto it = testInit.end();
        for (const Test& t : x)
        {
            --it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("reverse()", testPassed);

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

void throw_nullptr_error()
{
    cout << (const char*)"NULL POINTER !!!";
    while(true);
}

void throw_bad_alloc()
{
    cout << (const char*)"BAD ALLOC !!!";
    while(true);
}

