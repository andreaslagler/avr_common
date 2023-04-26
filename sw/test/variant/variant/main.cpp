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

#include <variant.h>

#include <stdint.h>

#include "../../common/debug_print.h"


template <uint8_t t_id>
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
template <uint8_t t_id>
uint16_t Test<t_id>::s_NofDefaultCtorCalls = 0;
template <uint8_t t_id>
uint16_t Test<t_id>::s_NofInitCtorCalls = 0;
template <uint8_t t_id>
uint16_t Test<t_id>::s_NofCopyCtorCalls = 0;
template <uint8_t t_id>
uint16_t Test<t_id>::s_NofMoveCtorCalls = 0;
template <uint8_t t_id>
uint16_t Test<t_id>::s_NofDtorCalls = 0;

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
    using T0 = Test<1>;
    using T1 = Test<2>;
    using T2 = Test<3>;
    using T3 = Test<4>;
    using T4 = Test<5>;
    
    using TestVariant = Variant<T0, T1, T2, T3, T4>;

    bool allPassed = true;
    bool testPassed = true;

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        TestVariant x;
        testPassed &= x.index() == 0;
        testPassed &= get<0>(x).getValue() == 0;
    }
    testPassed &= T0::check(1,0,0,0,1);
    testPassed &= T1::check(0,0,0,0,0);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Default constructor", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        constexpr size_t index = 1;
        const uint8_t value = 42;
        TestVariant x(in_place_index_t<index>(), value);
        testPassed &= x.index() == index;
        testPassed &= get<index>(x).getValue() == value;
    }
    testPassed &= T0::check(0,0,0,0,0);
    testPassed &= T1::check(0,1,0,0,1);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Init constructor", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        using Type = T1;
        const uint8_t value = 42;
        TestVariant x(in_place_type_t<Type>(), value);
        testPassed &= x.index() == 1;
        testPassed &= get<Type>(x).getValue() == value;
    }
    testPassed &= T0::check(0,0,0,0,0);
    testPassed &= T1::check(0,1,0,0,1);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Init constructor", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        using Type = T1;
        const uint8_t value = 42;
        TestVariant x(in_place_type_t<Type>(), value);
        TestVariant y = x;
        
        testPassed &= y.index() == 1;
        testPassed &= get<Type>(y).getValue() == value;
    }
    testPassed &= T0::check(0,0,0,0,0);
    testPassed &= T1::check(0,1,1,0,2);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Copy constructor", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        using Type = T1;
        const uint8_t value = 42;
        TestVariant x(in_place_type_t<Type>(), value);
        TestVariant y = move(x);
        
        testPassed &= y.index() == 1;
        testPassed &= get<Type>(y).getValue() == value;
    }
    testPassed &= T0::check(0,0,0,0,0);
    testPassed &= T1::check(0,1,0,1,2);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Move constructor", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        TestVariant x;
        using Type = T1;
        const uint8_t value = 42;
        Type t(value);
        x = t;
        testPassed &= x.index() == 1;
        testPassed &= get<Type>(x).getValue() == value;
    }
    testPassed &= T0::check(1,0,0,0,1);
    testPassed &= T1::check(0,1,1,0,2);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Copy assignment", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        TestVariant x;
        using Type = T1;
        const uint8_t value = 42;
        Type t(value);
        x = move(t);
        testPassed &= x.index() == 1;
        testPassed &= get<Type>(x).getValue() == value;
    }
    testPassed &= T0::check(1,0,0,0,1);
    testPassed &= T1::check(0,1,0,1,2);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("Move assignment", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        TestVariant x;
        const uint8_t value = 42;
        x.emplace<0>(value);
        testPassed &= x.index() == 0;
        testPassed &= get<0>(x).getValue() == value;
        x.emplace<1>(value);
        testPassed &= x.index() == 1;
        testPassed &= get<1>(x).getValue() == value;
    }
    testPassed &= T0::check(1,1,0,0,2);
    testPassed &= T1::check(0,1,0,0,1);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("emplace by index", testPassed);

    {
        testPassed = true;
        T0::resetCounter();
        T1::resetCounter();
        T2::resetCounter();
        T3::resetCounter();
        T4::resetCounter();
        TestVariant x;
        using Type = T1;
        const uint8_t value = 42;
        x.emplace<T0>(value);
        testPassed &= x.index() == 0;
        testPassed &= get<T0>(x).getValue() == value;
        x.emplace<Type>(value);
        testPassed &= x.index() == 1;
        testPassed &= get<Type>(x).getValue() == value;
    }
    testPassed &= T0::check(1,1,0,0,2);
    testPassed &= T1::check(0,1,0,0,1);
    testPassed &= T2::check(0,0,0,0,0);
    testPassed &= T3::check(0,0,0,0,0);
    testPassed &= T4::check(0,0,0,0,0);
    allPassed &= test_assert("emplace by type", testPassed);
    
    {
        testPassed = true;
        TestVariant x1;
        const TestVariant x2;
        testPassed &= (RefType::LRef == CheckRefType(get<0>(x1)));
        testPassed &= (RefType::ConstLRef == CheckRefType(get<0>(x2)));
        testPassed &= (RefType::RRef == CheckRefType(get<0>(move(x1))));
    }
    allPassed &= test_assert("access by index", testPassed);
    
    {
        testPassed = true;
        TestVariant x1;
        const TestVariant x2;
        testPassed &= (RefType::LRef == CheckRefType(get<T0>(x1)));
        testPassed &= (RefType::ConstLRef == CheckRefType(get<T0>(x2)));
        testPassed &= (RefType::RRef == CheckRefType(get<T0>(move(x1))));
    }
    allPassed &= test_assert("access by type", testPassed);
    
    {
        class NotDefaultConstructible
        {
            public:
            
            NotDefaultConstructible(const uint8_t value) : m_value(value)
            {}
            
            uint8_t getValue() const
            {
                return m_value;
            }
            
            private:
            
            uint8_t m_value = 0;
        };
        
        testPassed = true;
        // Variant<NotDefaultConstructible> x; // ERROR
        Variant<Monostate, NotDefaultConstructible> x;
        testPassed &= x.index() == 0;
        const uint8_t value = 42;
        x.emplace<NotDefaultConstructible>(value);
        testPassed &= x.index() == 1;
        testPassed &= get<1>(x).getValue() == value;
    }
    allPassed &= test_assert("Monostate", testPassed);
    
    

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


void throw_bad_variant_access()
{
    cout << (const char*)"BAD ALLOC !!!";
    while(true);
}
