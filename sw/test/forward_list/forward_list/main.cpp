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

#include <forward_list.h>
#include <allocator.h>

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
    
    constexpr size_t capacity = 6;
    //using Allocator = PoolAllocator<ForwardListNode<Test>, capacity>;
    using Allocator = FreeListAllocator;

    const std::initializer_list<Test> testInit({42,43,44});        
        
    char testMemory[3 * (sizeof(Test)+6)];
    const ForwardList<Test, Allocator> testList(testInit, Allocator(testMemory, 3 * (sizeof(Test)+6)));
    
    char memory[capacity * (sizeof(Test)+6)];
    Allocator allocator(memory, capacity * (sizeof(Test)+6));
    
    // construct/copy/destroy
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(allocator);
    }
    allPassed &= test_assert("Default constructor", testPassed && Test::check(0,0,0,0,0));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(allocator);
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,0,0,0));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(2,allocator);
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == 0;
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(2,0,0,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(2, *testInit.begin(), allocator);
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*testInit.begin()).getValue();
        }
    }
    allPassed &= test_assert("Init constructor", testPassed && Test::check(0,0,2,0,2));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testInit.begin(), testInit.end(), allocator);
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
        ForwardList<Test, Allocator> x(testList, allocator);
        auto it = testList.cbegin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Copy constructor", testPassed && Test::check(0,0,3,0,3));

    //forward_list(forward_list&& x);
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testInit, allocator);
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
        ForwardList<Test, Allocator> y(testInit, allocator);
        ForwardList<Test, Allocator> x(move(y));
        testPassed &= y.empty();                
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Move constructor", testPassed && Test::check(0,0,3,0,3));
       
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(2, allocator);
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
        ForwardList<Test, Allocator> x(allocator);
        ForwardList<Test, Allocator> y(testInit, allocator);
        x = move(y);
        testPassed &= y.empty();
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("Move assignment operator", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(2, allocator);
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
        ForwardList<Test, Allocator> x(2, allocator);
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
        ForwardList<Test, Allocator> x(2, allocator);
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
        ForwardList<Test, Allocator> x(2, allocator);
        x.assign(testInit);
        auto it = testInit.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("assign()", testPassed && Test::check(2,0,1,0,3));

    //allocator_type get_allocator() const noexcept;

    //// iterators
    //iterator before_begin() noexcept;
    //const_iterator before_begin() const noexcept;
    //iterator begin() noexcept;
    //const_iterator begin() const noexcept;
    //iterator end() noexcept;
    //const_iterator end() const noexcept;
    //
    //const_iterator cbegin() const noexcept;
    //const_iterator cbefore_begin() const noexcept;
    //const_iterator cend() const noexcept;
    //
    
    {
        testPassed = true;
        ForwardList<Test, Allocator> x(2, allocator);
        testPassed &= !x.empty();
        ForwardList<Test, Allocator> y(allocator);
        testPassed &= y.empty();
    }
    allPassed &= test_assert("empty()", testPassed);

    {
        testPassed = true;
        ForwardList<Test, Allocator> x(testList, allocator);
        testPassed &= (*testList.begin()).getValue() == x.front().getValue();
        testPassed &= (*testList.begin()).getValue() == testList.front().getValue();
    }
    allPassed &= test_assert("front()", testPassed);

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
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
        ForwardList<Test, Allocator> x(testList, allocator);
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
        ForwardList<Test, Allocator> x(testList, allocator);
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
        ForwardList<Test, Allocator> x(testList, allocator);
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
        ForwardList<Test, Allocator> x(testList, allocator);
        x.emplaceAfter(x.cbegin(), testList.front().getValue());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("emplaceAfter()", testPassed && Test::check(0,1,3,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        x.insertAfter(x.cbegin(), testList.front());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("insertAfter()", testPassed && Test::check(0,0,4,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        Test t = testList.front();
        x.insertAfter(x.cbegin(), move(t));
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("insertAfter()", testPassed && Test::check(0,0,4,1,5));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        x.insertAfter(x.cbegin(), 2,testList.front());
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
    allPassed &= test_assert("insertAfter()", testPassed && Test::check(0,0,5,0,5));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        x.insertAfter(x.cbegin(), testList.front());
        auto it = x.begin();
        testPassed &= (*it).getValue() == testList.front().getValue();
        for (const Test& t : testList)
        {
            ++it;
            testPassed &= t.getValue() == (*it).getValue();
        }
    }
    allPassed &= test_assert("insertAfter()", testPassed && Test::check(0,0,4,0,4));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(allocator);
        x.insertAfter(x.cbeforeBegin(), testList.begin(), testList.end());
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("insertAfter()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(allocator);
        x.insertAfter(x.cbeforeBegin(), testInit);
        auto it = x.begin();
        for (const Test& t : testInit)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("insertAfter()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        x.eraseAfter(x.cbegin());
        auto it = testList.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
            ++it;
        }
    }
    allPassed &= test_assert("eraseAfter()", testPassed && Test::check(0,0,3,0,3));

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        x.eraseAfter(x.cbegin(), x.cend());
        auto it = testList.begin();
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("eraseAfter()", testPassed && Test::check(0,0,3,0,3));

    //void swap(forward_list&)

    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(allocator);
        x.resize(3);
        Test test;
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == test.getValue();
        }
    }
    allPassed &= test_assert("resize()", testPassed && Test::check(4,0,0,0,4));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(allocator);
        x.resize(3,testList.front());
        for (const Test& t : x)
        {
            testPassed &= t.getValue() == testList.front().getValue();
        }
    }
    allPassed &= test_assert("resize()", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        testPassed &= !x.empty();
        x.clear();
        testPassed &= x.empty();
    }
    allPassed &= test_assert("clear()", testPassed && Test::check(0,0,3,0,3));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        ForwardList<Test, Allocator> y(testList, x.getAllocator());
        x.spliceAfter(x.cbeforeBegin(), y);
        testPassed &= y.empty();
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("spliceAfter()", testPassed && Test::check(0,0,6,0,6));
    
    {
        testPassed = true;
        Test::resetCounter();
        ForwardList<Test, Allocator> x(testList, allocator);
        ForwardList<Test, Allocator> y(testList, x.getAllocator());
        x.spliceAfter(x.cbeforeBegin(), move(y));
        testPassed &= y.empty();
        auto it = x.begin();
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
        for (const Test& t : testList)
        {
            testPassed &= t.getValue() == (*it).getValue();
            ++it;
        }
    }
    allPassed &= test_assert("spliceAfter()", testPassed && Test::check(0,0,6,0,6));




    //// forward_list operations
    //void splice_after(const_iterator position, forward_list& x);
    //void splice_after(const_iterator position, forward_list&& x);
    //void splice_after(const_iterator position, forward_list& x, const_iterator i);
    //void splice_after(const_iterator position, forward_list&& x, const_iterator i);
    //void splice_after(const_iterator position, forward_list& x,
    //const_iterator first, const_iterator last);
    //void splice_after(const_iterator position, forward_list&& x,
    //const_iterator first, const_iterator last);
    //
    //size_type remove(const T& value);
    //template<class Predicate> size_type remove_if(Predicate pred);
    //
    //size_type unique();
    //template<class BinaryPredicate> size_type unique(BinaryPredicate binary_pred);
    //
    //void merge(forward_list& x);
    //void merge(forward_list&& x);
    //template<class Compare> void merge(forward_list& x, Compare comp);
    //template<class Compare> void merge(forward_list&& x, Compare comp);
    //
    //void sort();
    //template<class Compare> void sort(Compare comp);
    //
    //void reverse() noexcept;

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

void throw_out_of_range()
{
    cout << (const char*)"OUT OF RANGE !!!";
    while(true);
}
