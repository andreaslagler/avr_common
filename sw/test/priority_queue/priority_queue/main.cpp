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

#include <queue.h>

#include <list.h>
#include <static_list.h>

#include "../../common/debug_print.h"

class Test
{
    public:
    
    Test()
    {}
    
    Test(const uint8_t value) : m_value(value)
    {}

    Test(const Test& other) : m_value(other.m_value)
    {}

    Test(Test&& other) : m_value(forward<uint8_t>(other.m_value))
    {}
    
    ~Test()
    {}
    
    Test& operator=(const Test& other)
    {
        m_value = other.m_value;
        return *this;
    }
    
    bool operator==(const Test& other) const
    {
        return m_value == other.m_value;
    }
    
    bool operator<(const Test& other) const
    {
        return m_value < other.m_value;
    }
    
    uint8_t getValue() const
    {
        return m_value;
    }
    
    private:
    
    uint8_t m_value = 0;
};

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

template <typename T>
struct Less
{
    constexpr bool operator()(const T& t1, const T& t2)
    {
        return t1 < t2;
    }
};

template <template<typename> class Container>
bool testQueue()
{
    bool allPassed = true;
    bool testPassed = true;
    
    const std::initializer_list<Test> testInit({42,43,44});
    const PriorityQueue<Test, Container<Test>, Less<Test>> testQueue(testInit);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x;
    }
    allPassed &= test_assert("Default constructor", testPassed);
    
    {
        testPassed = true;
        const Container<Test> c(testInit);
        PriorityQueue<Test, Container<Test>, Less<Test>> x(c);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed);
    
    {
        testPassed = true;
        Container<Test> c(testInit);
        PriorityQueue<Test, Container<Test>, Less<Test>> x(move(c));
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit.begin(), testInit.end());
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testQueue);
        auto it = testQueue.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy constructor", testPassed);

    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> y(testInit);
        PriorityQueue<Test, Container<Test>, Less<Test>> x(move(y));
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Move constructor", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x;
        x = testQueue;
        auto it = testQueue.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy assignment operator", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x;
        PriorityQueue<Test, Container<Test>, Less<Test>> y(testInit);
        x = move(y);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Move assignment operator", testPassed);

    {
        testPassed = true;
        auto it = testInit.begin();
        auto itBegin = testQueue.begin();
        auto itEnd = testQueue.end();
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
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
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
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
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
        auto itBegin = testQueue.rbegin();
        auto itEnd = testQueue.rend();
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
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
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
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
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
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
        testPassed &= !x.empty();
        PriorityQueue<Test, Container<Test>, Less<Test>> y;
        testPassed &= y.empty();
    }
    allPassed &= test_assert("empty()", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
        testPassed &= testInit.size() == x.size();
        PriorityQueue<Test, Container<Test>, Less<Test>> y;
        testPassed &= 0 == y.size();
    }
    allPassed &= test_assert("size()", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testInit);
        Test front = *testInit.begin();
        testPassed &= front.getValue() == x.top().getValue();
        testPassed &= front.getValue() == testQueue.top().getValue();
    }
    allPassed &= test_assert("top()", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testQueue);
        x.emplace(testQueue.top().getValue());
        testPassed &= (x.size() == (testQueue.size()+1));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testQueue.top().getValue();
        for (const Test& t : testQueue)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("emplace()", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testQueue);
        x.push(testQueue.top());
        testPassed &= (x.size() == (testQueue.size()+1));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testQueue.top().getValue();
        for (const Test& t : testQueue)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("push()", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testQueue);
        Test t = testQueue.top();
        x.push(move(t));
        testPassed &= (x.size() == (testQueue.size()+1));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testQueue.top().getValue();
        for (const Test& t : testQueue)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("push()", testPassed);
    
    {
        testPassed = true;
        PriorityQueue<Test, Container<Test>, Less<Test>> x(testQueue);
        x.pop();
        testPassed &= (x.size() == (testQueue.size()-1));
        auto it = testQueue.begin();
        for (const Test& t : x)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("pop()", testPassed);

    return allPassed;
}

template <typename T>
using StaticList_ = StaticList<T,10>;

int main(void)
{
    bool allPassed = true;
    
    allPassed &= test_assert("PriorityQueue using List", testQueue<List>());
    allPassed &= test_assert("PriorityQueue using StaticList", testQueue<StaticList_>());

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

void throw_nullptr_error()
{
    cout << (const char*)"NULL POINTER !!!";
    while(true);
}
