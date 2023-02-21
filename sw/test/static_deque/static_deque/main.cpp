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
#include <algorithm.h>

#include "../../common/debug_print.h"

void throw_out_of_range()
{
    while(true);
}

void throw_length_error()
{
    while(true);
}

class Test
{
    public:
    
    constexpr Test()
    {
        //cout << "default ctor";
    }
    
    constexpr Test(const uint8_t value) : m_value(value)
    {
        //cout << "init ctor";
    }

    constexpr Test(const Test& other) : m_value(other.m_value)
    {
        //cout << "copy ctor";
    }

    constexpr Test(Test&& other) : m_value(forward<uint8_t>(other.m_value))
    {
        //cout << "move ctor";
    }
    
    constexpr ~Test()
    {
        //cout << "dtor";
    }
    
    constexpr uint8_t getValue() const
    {
        return m_value;
    }
    
    private:
    uint8_t m_value = 0;
};


int main(void)
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    // Possible output:
    //
    // numbers1: 42, 43, 44, 45, 46
    // numbers2: 42, 43, 44, 45, 46
    // numbers3: 42, 43, 44, 45, 46
    // numbers4: 42, 43, 44, 45, 46
    // numbers5: 42, 42, 42, 42, 42
    {
        cout << "----- deque";
                
        // C++11 initializer list syntax:
        StaticDeque<int, 5> numbers1 {42, 43, 44, 45, 46};
        cout << "numbers1: " << numbers1;
        
        // numbers2 == numbers1
        StaticDeque<int, 5> numbers2(numbers1.begin(), numbers1.end());
        cout << "numbers2: " << numbers2;
        
        // numbers3 == numbers1
        StaticDeque<int, 5> numbers3(numbers1);
        cout << "numbers3: " << numbers3;
        
        // numbers4 == numbers1
        StaticDeque<int, 5> numbers4(move(numbers1));
        cout << "numbers4: " << numbers4;
        cout << "numbers1 after move: " << numbers1;
        
        // numbers5 == 42
        const uint8_t value = 42;
        StaticDeque<int, 5> numbers5(5, value);
        cout << "numbers5: " << numbers5;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // Assignment operator
    //
    // Possible output:
    //
    // Initially:
    // x = 1, 2, 3
    // y =
    // z =
    // Copy assignment copies data from x to y:
    // x = 1, 2, 3
    // y = 1, 2, 3
    // Move assignment moves data from x to z, modifying both x and z:
    // x =
    // z = 1, 2, 3
    {
        cout << "----- operator=";
        
        StaticDeque<int, 5> x {1, 2, 3}, y, z;
        const auto w = { 4, 5, 6, 7 };
        
        cout << "Initially:";
        cout << "x = " << x;
        cout << "y = " << y;
        cout << "z = " << z;
        
        cout << "Copy assignment copies data from x to y:";
        y = x;
        cout << "x = " << x;
        cout << "y = " << y;
        
        cout << "Move assignment moves data from x to z, modifying both x and z:";
        z = move(x);
        cout << "x = " << x;
        cout << "z = " << z;
        
        cout << "Assignment of initializer_list w to z:\n";
        z = w;
        cout << "z = " << z;
    }    
    
    //////////////////////////////////////////////////////////////////////////
    // Assignment
    //
    // Possible output:
    //
    // 1,1,1,1,1
    // 2,2,2,2,2,2
    // 3,3,3
    {
        cout << "----- assign";
        
        StaticDeque<int, 5> d;
               
        d.assign(4, uint8_t(1));
        cout << d;
        
        uint8_t extra[5] = {2, 2, 2, 2, 2};
        d.assign(extra, extra + 5);
        cout << d;
        
        d.assign({3, 3, 3});
        cout << d;
    }

    //////////////////////////////////////////////////////////////////////////
    // at()
    //
    // Possible output:
    //
    // Element at index 2 has value 4
    // data: 1 88 4 5 6
    {
        cout << "----- at";
        
        StaticDeque<int,5> data = { 1, 2, 4, 5, 6 };
        
        // Set element 1
        data.at(1) = 88;
        
        // Read element 2
        cout << "Element at index 2 has value " << data.at(2);
                
        // Set element 6
        //data.at(6) = 666; // Exception
        
        // Print final values
        cout << "data:";
        cout << data;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // Access operator
    //
    // Possible output:
    //
    // Second element: 4
    // All numbers: 5 4 6 8
    {
        cout << "----- operator[]";
        
        StaticDeque<int, 5> numbers {2, 4, 6, 8};
        
        cout << "Second element: " << numbers[1];
        
        numbers[0] = 5;
        
        cout << "All numbers:";
        for (auto i : numbers) {
            cout << i;
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    // front()
    //
    // Possible output:
    //
    // The first character is o
    {
        cout << "----- front";
        
        StaticDeque<char, 6> letters {'o', 'm', 'g', 'w', 't', 'f'};
        
        if (!letters.empty()) {
            cout << "The first character is " << letters.front();
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    // back()
    //
    // Possible output:
    //
    // The first character is o
    {
        cout << "----- back";
        
        StaticDeque<char, 6> letters {'a', 'b', 'c', 'd', 'e', 'f'};
 
        if (!letters.empty()) {
            cout << "The last character is " << letters.back();
        }  
    }
    
    //////////////////////////////////////////////////////////////////////////
    // begin(), end(), cbegin(), cend()
    //
    // Possible output:
    //
    // 1 2 4 8 16
    // 1 2 4 8 16
    // deque 'empty' is indeed empty.
    {
        cout << "----- begin, end, cbegin, cend";
         
        StaticDeque<Test, 5> nums {1, 2, 4, 8, 16};
        StaticDeque<Test, 5> empty;
        
        // Print deque.
        for_each(nums.begin(), nums.end(), [](const Test& n) { cout << n; });
        for_each(nums.cbegin(), nums.cend(), [](const Test& n) { cout << n; });

        if (!(empty.begin() != empty.end()))
            cout << "deque 'empty' is indeed empty.";
    }
    

    //////////////////////////////////////////////////////////////////////////
    // rbegin(), rend(), crbegin(), crend()
    //
    // Possible output:
    //
    // 16 8 4 2 1
    // 16 8 4 2 1
    // deque 'empty' is indeed empty.
    {
        cout << "----- rbegin, rend, crbegin, crend";
        
        StaticDeque<Test, 5> nums {1, 2, 4, 8, 16};
        StaticDeque<Test, 5> empty;
        
        // Print deque.
        for_each(nums.rbegin(), nums.rend(), [](const Test& n) { cout << n; });
        for_each(nums.crbegin(), nums.crend(), [](const Test& n) { cout << n; });

        if (!(empty.rbegin() != empty.rend()))
        cout << "deque 'empty' is indeed empty.";
    }
    
    
    //////////////////////////////////////////////////////////////////////////
    // empty()
    //
    // Possible output:
    //
    // Initially, numbers.empty(): true
    // After adding elements, numbers.empty(): false
    {
        cout << "----- empty";
        
        StaticDeque<Test, 5> numbers;
        cout << "Initially, numbers.empty(): " << numbers.empty();
        
        numbers.pushBack(42);
        numbers.pushBack(66);
        cout << "After adding elements, numbers.empty(): " << numbers.empty();
    }

    //////////////////////////////////////////////////////////////////////////
    // size(), max_size()
    //
    // Possible output:
    //
    // nums contains 4 elements.
    // nums may contain 5 elements.
    {
        cout << "----- size, max_size";
        
        StaticDeque<Test, 5> nums {1, 3, 5, 7};
    
        cout << "nums contains " << nums.size() << " elements.";
        cout << "nums may contain " << nums.max_size() << " elements.";
    }

    //////////////////////////////////////////////////////////////////////////
    // clear()
    //
    // Possible output:
    //
    // Before clear:  1 2 3
    // After clear:
    {
        cout << "----- clear";
        
        StaticDeque<Test, 5> container{1, 2, 3};
        
        cout << "Before clear:";
        cout << container;
    
        container.clear();
    
        cout << "After clear:";
        cout << container;
    }

    //////////////////////////////////////////////////////////////////////////
    // pushBack()
    //
    // Possible output:
    //
    // container holds: 1 2 3 5 5
    {         
        cout << "----- pushBack";
        
        StaticDeque<Test, 5> container{{1}, {2}, {3}};
        
        Test x = 5;
        container.pushBack(x);
        container.pushBack(move(x));
        
        cout << "container holds:";
        cout << container;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // emplaceBack()
    //
    // Possible output:
    //
    // container holds: 1 2 3 5
    {         
        cout << "----- emplaceBack";
        
        StaticDeque<Test, 5> container{{1}, {2}, {3}};
        
        uint8_t x = 5;
        container.emplaceBack(x);
        
        cout << "container holds:";
        cout << container;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // popBack()
    //
    // Possible output:
    //
    // container holds: 1 2
    {         
        cout << "----- popBack";
        
        StaticDeque<Test, 5> container{{1}, {2}, {3}};
        
        container.popBack();
        
        cout << "container holds:";
        cout << container;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // pushFront()
    //
    // Possible output:
    //
    // container holds: 5 5 1 2 3
    {         
        cout << "----- pushFront";
        
        StaticDeque<Test, 5> container{{1}, {2}, {3}};
        
        Test x = 5;
        container.pushFront(x);
        container.pushFront(move(x));
        
        cout << "container holds:";
        cout << container;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // emplaceFront()
    //
    // Possible output:
    //
    // container holds: 5 1 2 3
    {         
        cout << "----- emplaceFront";
        
        StaticDeque<Test, 5> container{{1}, {2}, {3}};
        
        uint8_t x = 5;
        container.emplaceFront(x);
        
        cout << "container holds:";
        cout << container;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // popFront()
    //
    // Possible output:
    //
    // container holds: 2 3
    {         
        cout << "----- popFront";
        
        StaticDeque<Test, 5> container{{1}, {2}, {3}};
        
        container.popFront();
        
        cout << "container holds:";
        cout << container;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // resize()
    //
    // Possible output:
    //
    // nums contains 4 elements.
    // nums may contain 5 elements.
    {
        cout << "----- resize";
        
        StaticDeque<int, 5> c = {1, 2, 3};
        cout << "The deque holds: ";
        cout << c;
        
        c.resize(5);
        cout << "After resize up to 5: ";
        cout << c;
        
        c.resize(2);
        cout << "After resize down to 2: ";
        cout << c;
        
        c.resize(4, 4);
        cout << "After resize up to 4 (initializer = 4): ";
        cout << c;
        
        //c.resize(6); // Exception: length error
    }
    
    //////////////////////////////////////////////////////////////////////////
    // swap()
    //
    // Possible output:
    //
    // nums contains 4 elements.
    // nums may contain 5 elements.
    {
        cout << "----- swap";
        
        StaticDeque<int, 5> a1{1, 2, 3}, a2{4, 5};
        
        auto it1 = a1.begin();
        auto it2 = a2.begin();
        
        int& ref1 = a1.front();
        int& ref2 = a2.front();
        
        cout << a1 << a2 << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2 << '\n';
        a1.swap(a2);
        cout << a1 << a2 << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2 << '\n';
    }

    while (true)
    {
    }
}

template<typename Arg>
struct debugPrinter
{
static void print(const Arg& arg)
{
// Put a tracepoint here and display {arg} in output window
(void)arg;
}
};


template<typename T, size_t N>
struct debugPrinter<StaticDeque<T,N>>
{
    static void print(const StaticDeque<T,N>& arg)
    {
        for (const T& t : arg)
        {
            debugPrinter<T>::print(t);
        }
    }
};

template<>
struct debugPrinter<const char*>
{
    static void print(const char* arg)
    {
        // Put a tracepoint here and display {arg, s} in output window
        (void)arg;
    }
};

template<>
struct debugPrinter<char>
{
static void print(char arg)
{
// Put a tracepoint here and display {arg, c} in output window
(void)arg;
}
};

template<>
struct debugPrinter<bool>
{
    static void print(const bool arg)
    {
        // Put a tracepoint here and display {arg} in output window
        debugPrinter<const char*>::print(arg ? "true" : "false");
    }
};

template<>
struct debugPrinter<Test>
{
    static void print(const Test& arg)
    {
        // Put a tracepoint here and display {arg} in output window
        debugPrinter<uint8_t>::print(arg.getValue());
    }
};

template< size_t N>
struct debugPrinter<char[N]>
{
    static void print(const char* arg)
    {
        debugPrinter<const char*>::print(arg);
    }
};
