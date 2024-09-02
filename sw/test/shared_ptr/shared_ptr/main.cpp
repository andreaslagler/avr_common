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

#include <memory.h>

#include <stdint.h>

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

enum class RefType : uint8_t
{
    ConstLRef = 0,
    ConstRRef,
    LRef,
    RRef
};

template <typename Type>
RefType CheckRefType(const Type&)
{
    return RefType::ConstLRef;
}

template <typename Type>
RefType CheckRefType(Type&)
{
    return RefType::LRef;
}

template <typename Type>
RefType CheckRefType(const Type&&)
{
    return RefType::ConstRRef;
}

template <typename Type>
RefType CheckRefType(Type&&)
{
    return RefType::RRef;
}


int main(void)
{

    bool allPassed = true;
    bool testPassed = true;

    {
        testPassed = true;
        SharedPtr<Test> ptr;
    }
    testPassed &= Test::check(0,0,0,0,0);
    Test::resetCounter();
    allPassed &= test_assert("Default constructor", testPassed);
    
    {
        testPassed = true;
        SharedPtr<Test> ptr(nullptr);
    }
    testPassed &= Test::check(0,0,0,0,0);
    Test::resetCounter();
    allPassed &= test_assert("Initializing constructor", testPassed);

    {
        testPassed = true;
        SharedPtr<Test> ptr = makeShared<Test>(42);
    }
    testPassed &= Test::check(0,1,0,0,1);
    Test::resetCounter();
    allPassed &= test_assert("Initializing constructor / makeShared", testPassed);

    {
        testPassed = true;
        SharedPtr<Test> ptr1 = makeShared<Test>(42);
        SharedPtr<Test> ptr2 = ptr1;
        testPassed &= 42 == ptr1->getValue();
        testPassed &= 42 == ptr2->getValue();
    }
    testPassed &= Test::check(0,1,0,0,1);
    Test::resetCounter();
    allPassed &= test_assert("Copy constructor", testPassed);

    {
        testPassed = true;
        SharedPtr<Test> ptr1 = makeShared<Test>(42);
        SharedPtr<Test> ptr2 = move(ptr1);
        //testPassed &= 42 == ptr1->getValue(); // <- nullptr exception !!!
        testPassed &= 42 == ptr2->getValue();
    }
    testPassed &= Test::check(0,1,0,0,1);
    Test::resetCounter();
    allPassed &= test_assert("Move constructor", testPassed);

    {
        testPassed = true;
        SharedPtr<Test> ptr1 = makeShared<Test>(42);
        SharedPtr<Test> ptr2;
        ptr2 = ptr1;
        testPassed &= 42 == ptr1->getValue();
        testPassed &= 42 == ptr2->getValue();
    }
    testPassed &= Test::check(0,1,0,0,1);
    Test::resetCounter();
    allPassed &= test_assert("Copy assignment", testPassed);

    {
        testPassed = true;
        SharedPtr<Test> ptr1 = makeShared<Test>(42);
        SharedPtr<Test> ptr2;
        ptr2 = move(ptr1);
        //testPassed &= 42 == ptr1->getValue(); // <- nullptr exception !!!
        testPassed &= 42 == ptr2->getValue();
    }
    testPassed &= Test::check(0,1,0,0,1);
    Test::resetCounter();
    allPassed &= test_assert("Move assignment", testPassed);


    allPassed &= test_assert("OVERALL:", allPassed);
    
    while (true)
    {
    }
}

volatile const char* dummy;

template <>
struct debugPrinter<const char*>
{
    static void print(const char* arg)
    {
        dummy = arg;
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
    cout << (const char*)"NULLPTR !!!";
    while(true);
}
