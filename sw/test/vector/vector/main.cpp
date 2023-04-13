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

#include <vector.h>
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

Vector<Test>::allocator_type::size_type checkAllocator()
{
    Vector<Test>::allocator_type allocator;
    for (Vector<Test>::allocator_type::size_type len = HEAP_SIZE; len > 0; --len)
    {
        void * test = allocator.allocate(len);
        if (nullptr != test)
        {
            allocator.deallocate(test);
            return len;
        }
    }
    return 0;
}

int main(void)
{
    bool allPassed = true;
    bool testPassed = true;
    
    const std::initializer_list<Test> testInit({42,43,44});
    const Vector<Test> testDeque(testInit);
    
    const auto len = checkAllocator();
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x;
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Default constructor", testPassed && Test::check(0,0,0,0,0));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2);
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == 0;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Init constructor", testPassed && Test::check(2,0,0,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2, *testInit.begin());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testInit.begin()).getValue();
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,2,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit.begin(), testInit.end());
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testDeque);
        auto it = testDeque.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Copy constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Copy constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> y(testInit);
        Vector<Test> x(move(y));
        testPassed &= y.empty();
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Move constructor", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2);
        x = testDeque;
        auto it = testDeque.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Copy assignment operator", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x;
        Vector<Test> y(testInit);
        x = move(y);
        testPassed &= y.empty();
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Move assignment operator", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2);
        x = testInit;
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("Copy assignment operator", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2);
        x.assign(testDeque.begin(), testDeque.end());
        auto it = testDeque.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2);
        x.assign(3,*testDeque.begin());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testDeque.begin()).getValue();
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,3,0,5));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(2);
        x.assign(testInit);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,3,0,5));
    
    
    {
        testPassed = true;
        testPassed &= HeapAllocator<>() == testDeque.getAllocator();
    }
    allPassed &= test_assert("get_allocator()", testPassed);
    
    {
        testPassed = true;
        auto it = testInit.begin();
        auto itBegin = testDeque.begin();
        auto itEnd = testDeque.end();
        while (itBegin != itEnd)
        {
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++it;
            ++itBegin;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("begin() / end()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
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
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("begin() / end()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
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
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("cbegin() / cend()", testPassed);
    
    {
        testPassed = true;
        auto it = testInit.end();
        auto itBegin = testDeque.rbegin();
        auto itEnd = testDeque.rend();
        while (itBegin != itEnd)
        {
            --it;
            testPassed &= (*itBegin).getValue() == (*it).getValue();
            ++itBegin;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("rbegin() / rend()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
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
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("rbegin() / rend()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
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
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("crbegin() / crend()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(2);
        testPassed &= !x.empty();
        Vector<Test> y;
        testPassed &= y.empty();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("empty()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
        testPassed &= testInit.size() == x.size();
        testPassed &= testInit.size() == x.capacity();
        Vector<Test> y;
        testPassed &= 0 == y.size();
        testPassed &= 0 == y.capacity();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("size() / capacity", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
        Test front = *testInit.begin();
        testPassed &= front.getValue() == x.front().getValue();
        testPassed &= front.getValue() == testDeque.front().getValue();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("front()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
        Test back = *(testInit.begin() + testInit.size() - 1);
        testPassed &= back.getValue() == x.back().getValue();
        testPassed &= back.getValue() == testDeque.back().getValue();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("back()", testPassed);
    
    {
        testPassed = true;
        Vector<Test> x(testInit);
        auto dataX = x.data();
        Vector<Test> y(testInit);
        auto dataY = y.data();
        for (const auto& t : testInit)
        {
            testPassed &= (*dataX).getValue() == t.getValue();
            testPassed &= (*dataY).getValue() == t.getValue();
            ++dataX;
            ++dataY;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("data()", testPassed);

    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit);
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
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("resize()", testPassed && Test::check(2,0,5,0,7));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit);
        x.reserve(2);
        testPassed &= (x.size() == 3);
        testPassed &= (x.capacity() == 3);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        x.reserve(4);
        testPassed &= (x.size() == 3);
        testPassed &= (x.capacity() == 4);
        it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("reserve()", testPassed && Test::check(0,0,6,0,6));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit);
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
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("resize()", testPassed && Test::check(0,0,7,0,7));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit);
        x.resize(testInit.size() * 2);
        testPassed &= (x.size() == testInit.size() * 2);
        auto it = x.begin();
        for (const Test& t : testInit)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        x.resize(testInit.size());
        testPassed &= (x.size() == testInit.size());
        it = x.begin();
        for (const Test& t : testInit)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        x.shrinkToFit();
        testPassed &= (x.size() == testInit.size());
        it = x.begin();
        for (const Test& t : testInit)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("shrinkToFit()", testPassed && Test::check(3,0,9,0,12));

    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testInit);
        auto it = testInit.begin();
        for (Vector<Test>::size_type idx = 0; idx < x.size(); ++idx)
        {
            testPassed &= x.at(idx).getValue() == (*it).getValue();
            testPassed &= x[idx].getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("at() / operator[]", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        const Vector<Test> x(testInit);
        auto it = testInit.begin();
        for (Vector<Test>::size_type idx = 0; idx < x.size(); ++idx)
        {
            testPassed &= x.at(idx).getValue() == (*it).getValue();
            testPassed &= x[idx].getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("at() / operator[]", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testDeque);
        x.emplaceBack(testDeque.front().getValue());
        testPassed &= (x.size() == (testDeque.size()+1));
        auto it = x.begin();
        for (const Test& t : testDeque)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testDeque.front().getValue();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("emplaceBack()", testPassed && Test::check(0,1,6,0,7));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testDeque);
        x.pushBack(testDeque.back());
        testPassed &= (x.size() == (testDeque.size()+1));
        auto it = x.begin();
        for (const Test& t : testDeque)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testDeque.back().getValue();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("pushBack()", testPassed && Test::check(0,0,7,0,7));
    
    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testDeque);
        Test t = testDeque.back();
        x.pushBack(move(t));
        testPassed &= (x.size() == (testDeque.size()+1));
        auto it = x.begin();
        for (const Test& t : testDeque)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        testPassed &= (*it).getValue() == testDeque.back().getValue();
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("pushBack()", testPassed && Test::check(0,0,7,1,8));

    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testDeque);
        x.popBack();
        testPassed &= (x.size() == (testDeque.size()-1));
        auto it = testDeque.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    testPassed &= (len == checkAllocator());
    allPassed &= test_assert("popBack()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        Vector<Test> x(testDeque);
        testPassed &= !x.empty();
        x.clear();
        testPassed &= x.empty();
    }
    testPassed &= (len == checkAllocator());
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

void throw_out_of_range()
{
    cout << (const char*)"RANGE ERROR !!!";
    while(true);
}
