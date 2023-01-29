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

// Test for Array class

#include "array.h"
#include "../../common/debug_print.h"

int main(void)
{
    //////////////////////////////////////////////////////////////////////////
    // see https://en.cppreference.com/w/cpp/container/array
    {
        // construction uses aggregate initialization
        Array<uint8_t, 3> a1{ {1, 2, 3} }; // double-braces required in C++11 prior to the CWG 1270 revision (not needed in C++11 after the revision and in C++14 and beyond)
        cout << a1;
        
        Array<uint8_t, 3> a2 = {1, 2, 3};  // double braces never required after =
        cout << a2;
    }
    
    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/at
    
    Possible output:
    
    Element at index 2 has value 4
    data size = 6
    data: 1 88 4 5 5 6
    */
    {
        Array<int,6> data = { 1, 2, 4, 5, 5, 6 };
        
        // Set element 1
        data.at(1) = 88;
        
        // Read element 2
        cout << "Element at index 2 has value " << data.at(2);
        
        cout << "data size = " << data.size();
        
        // Set element 6
        //data.at(6) = 666; // <-- Exception
        
        // Print final values
        cout << "data:";
        for (int elem : data)
        cout << elem;
    }
    
    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/operator_at
    
    Possible output:
    
    Second element: 4
    All numbers: 5 4 6 8
    */
    {
        Array<int,4> numbers {2, 4, 6, 8};
        
        cout << "Second element: " << numbers[1];
        
        numbers[0] = 5;
        
        cout << "All numbers:";
        for (auto i : numbers) {
            cout << i;
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/front
    
    Possible output:
    
    The first character is 'o'.
    */
    {
        Array<char, 6> letters {'o', 'm', 'g', 'w', 't', 'f'};
        
        if (!letters.empty()) {
            cout << "The first character is" << letters.front();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/back
    
    Possible output:
    
    The first character is 'f'.
    */
    {
        Array<char, 6> letters {'a', 'b', 'c', 'd', 'e', 'f'};
        
        if (!letters.empty()) {
            cout << "The last character is " << letters.back();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/data
    
    Possible output:
    
    data = 1 2 3 4
    */
    {
        Array<int, 4> container { 1, 2, 3, 4 };
        
        auto pointer_func = [](const int* p, size_t size)
        {
            cout << "data = ";
            for (size_t i = 0; i < size; ++i)
            cout << p[i];
        };

        // Prefer container.data() over &container[0]
        pointer_func(container.data(), container.size());
        
        // span (C++20) is a safer alternative to separated pointer/size.
        //span_func({container.data(), container.size()});
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/begin
    see https://en.cppreference.com/w/cpp/container/array/end
    
    Possible output:
    
    1) true true
    2) false false
    3) 5 5
    4) 1
    5) 1 2 3 4
    */
    {
        Array<int, 0> empty;
        cout << "1) "
        << (empty.begin() == empty.end())   // true
        << (empty.cbegin() == empty.cend()); // true
        // *(empty.begin()) = 42; // => undefined behaviour at run-time
        
        Array<int, 4> numbers{5, 2, 3, 4};
        cout << "2) "
        << (numbers.begin() == numbers.end())   // false
        << (numbers.cbegin() == numbers.cend()) // false
        << "3) "
        << *(numbers.begin())   // 5
        << *(numbers.cbegin()); // 5
        
        *numbers.begin() = 1;
        cout << "4) " << *(numbers.begin()); // 1
        // *(numbers.cbegin()) = 42; // compile-time error:
        // read-only variable is not assignable
        
        // print out all elements
        cout << "5) ";
        for_each(numbers.cbegin(), numbers.cend(), [](int x) {cout << x;});
        
        
        constexpr Array<char, 3> constants {{'A', 'B', 'C'}};
        static_assert(constants.begin() != constants.end());   // OK
        static_assert(constants.cbegin() != constants.cend()); // OK
        static_assert(*constants.begin() == 'A');              // OK
        static_assert(*constants.cbegin() == 'A');             // OK
        //*constants.begin() = 'Z'; // compile-time error:
        // read-only variable is not assignable
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/empty
    
    Possible output:
    
    numbers.empty(): false
    no_numbers.empty(): true
    */
    {
        Array<int, 4> numbers {3, 1, 4, 1};
        Array<int, 0> no_numbers;
        
        cout << "numbers.empty(): " << numbers.empty();
        cout << "no_numbers.empty(): " << no_numbers.empty();
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/size
    
    Possible output:
    
    nums contains 4 elements.
    */
    {
        Array<int, 4> nums {1, 3, 5, 7};
        cout << "nums contains " << nums.size() << " elements.";
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/size
    
    Possible output:
    
    Maximum size of the array is 10
    */
    {
        Array<char, 10> q;
        cout << "Maximum size of the array is " << q.max_size();
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/fill
    
    Possible output:
    
    ????????
    ????????
    ????????
    ????????
    */
    {
        constexpr uint8_t xy = 4;
        using Cell = Array<char, 8>;
        Array<Cell, xy * xy> board;
        board.fill({ {0xE2, 0x96, 0x84, 0xE2, 0x96, 0x80, 0, 0} }); // "??";
        
        for (Cell c : board)
        {
            cout << c.data();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/swap
    
    Possible output:
    
    { 1 2 3 } { 4 5 6 } 1 4 2 5
    { 4 5 6 } { 1 2 3 } 4 1 5 2
    */
    {
        Array<int, 3> a1{1, 2, 3}, a2{4, 5, 6};
        
        auto it1 = a1.begin();
        auto it2 = a2.begin();
        int& ref1 = a1[1];
        int& ref2 = a2[1];
        
        cout << '{' << a1 << "} {" << a2 << '}' << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2;
        swap(a1,a2);
        cout << '{' << a1 << "} {" << a2 << '}' << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2;
        
        // Note that after swap iterators and references stay associated with their original
        // array, e.g. `it1` still points to element a1[0], `ref1` still refers to a1[1].
    }
    
    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/get
    
    Possible output:
    
    (1, 2, 3)
    */
    {
        Array<int, 3> arr;
        
        // set values:
        get<0>(arr) = 1;
        get<1>(arr) = 2;
        get<2>(arr) = 3;
        //get<3>(arr) = 4; // Error: Index out of range
        
        // get values:
        cout << "(" << get<0>(arr) << ", " << get<1>(arr) << ", " << get<2>(arr) << ")";
    }
    
    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/operator_cmp
    
    Possible output:
    
    alice == bob returns false
    alice != bob returns true
    alice <  bob returns true
    alice <= bob returns true
    alice >  bob returns false
    alice >= bob returns false

    alice == eve returns true
    alice != eve returns false
    alice <  eve returns false
    alice <= eve returns true
    alice >  eve returns false
    alice >= eve returns true
    */
    {
        Array<int, 3> alice{1, 2, 3};
        Array<int, 3> bob{7, 8, 9};
        Array<int, 3> eve{1, 2, 3};
        
        // Compare non equal containers
        cout << "alice == bob returns " << (alice == bob);
        cout << "alice != bob returns " << (alice != bob);
        cout << "alice <  bob returns " << (alice < bob);
        cout << "alice <= bob returns " << (alice <= bob);
        cout << "alice >  bob returns " << (alice > bob);
        cout << "alice >= bob returns " << (alice >= bob);
        
        // Compare equal containers
        cout << "alice == eve returns " << (alice == eve);
        cout << "alice != eve returns " << (alice != eve);
        cout << "alice <  eve returns " << (alice < eve);
        cout << "alice <= eve returns " << (alice <= eve);
        cout << "alice >  eve returns " << (alice > eve);
        cout << "alice >= eve returns " << (alice >= eve);
    }
    
    while (true);
}

// Definition of exception handler
constexpr void throw_out_of_range()
{
    while(true);
}

// debugPrinter specializations
template<typename T, size_t N>
struct debugPrinter<Array<T,N>>
{
    static void print(const Array<T,N>& arg)
    {
        for (const T & elem : arg)
        {
            debugPrinter<T>::print(elem);
        }
    }
};

template<>
struct debugPrinter<uint8_t>
{
    static void print(const uint8_t arg)
    {
        // Put a tracepoint here and display {arg} in output window
        (void)arg;
    }
};

template<>
struct debugPrinter<int>
{
    static void print(const int arg)
    {
        // Put a tracepoint here and display {arg} in output window
        (void)arg;
    }
};

template<>
struct debugPrinter<bool>
{
    static void print(const bool arg)
    {
        debugPrint(arg ? "true" : "false");
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
struct debugPrinter<char*>
{
    static void print(char* arg)
    {
        // Put a tracepoint here and display {arg, s} in output window
        (void)arg;
    }
};

template<>
struct debugPrinter<char>
{
    static void print(const char arg)
    {
        // Put a tracepoint here and display {arg,c} in output window
        (void)arg;
    }
};


