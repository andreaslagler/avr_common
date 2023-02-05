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

#include "pgm_array.h"
#include "../../common/debug_print.h"

int main(void)
{
    //////////////////////////////////////////////////////////////////////////
    /*
    Constructor
    */
    {
    auto a = makePgmArray<int, 1, 2, 3>();
    auto b = a;
    cout << a;
    cout << b;
    
    auto c = move(a);
    cout << c;
    }
        
    //////////////////////////////////////////////////////////////////////////
    /*
    Assignment operator
    */
    {
    auto a = makePgmArray<int, 1, 2, 3>();
    auto b = makePgmArray<int, 4, 5, 6, 7>();
    cout << a;
    cout << b;
    
    a = b;
    cout << a;    
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
        auto data = makePgmArray<int, 1, 2, 4, 5, 5, 6 >();
        
        // Read element 2
        cout << "Element at index 2 has value " << data.at(2);
        
        // Read element 6
        //cout << "Element at index 6 has value " << data.at(6); // <-- Exception
        
        // Print final values
        cout << "data:";
        for (int elem : data)
        cout << elem;
    }
    
    ////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/operator_at
    
    Possible output:
    
    Second element: 4
    All numbers: 5 4 6 8
    */
    {
        auto numbers = makePgmArray<int, 2, 4, 6, 8>();

        cout << "Second element: " << numbers[1];
        
        cout << "All numbers:";
        for (auto i : numbers) {
            cout << i;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/front
    
    Possible output:
    
    The first character is 'o'.
    */
    {
       auto letters = makePgmArray<char, 'o', 'm', 'g', 'w', 't', 'f'>();
        
        if (!letters.empty()) {
            cout << "The first character is" << letters.front();
        }
    }

    ////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/back
    
    Possible output:
    
    The first character is 'f'.
    */
    {
        auto letters = makePgmArray<char, 'a', 'b', 'c', 'd', 'e', 'f'>();
        
        if (!letters.empty()) {
            cout << "The last character is " << letters.back();
        }
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
        auto empty = makePgmArray<int>();
        cout << "1) "
        << (empty.begin() == empty.end())   // true
        << (empty.cbegin() == empty.cend()); // true
        // *(empty.begin()) = 42; // => undefined behaviour at run-time
        
        auto numbers = makePgmArray<int, 5, 2, 3, 4>();
        cout << "2) "
        << (numbers.begin() == numbers.end())   // false
        << (numbers.cbegin() == numbers.cend()) // false
        << "3) "
        << *(numbers.begin())   // 5
        << *(numbers.cbegin()); // 5
        
        cout << "4) " << *(numbers.begin()); // 1
        // *(numbers.cbegin()) = 42; // compile-time error:
        // read-only variable is not assignable
        
        // print out all elements
        cout << "5) ";
        for_each(numbers.cbegin(), numbers.cend(), [](int x) {cout << x;});
        
        
        constexpr auto constants = makePgmArray<char, 'A', 'B', 'C'>();
        static_assert(constants.begin() != constants.end());   // OK
        static_assert(constants.cbegin() != constants.cend()); // OK
        cout << (*constants.begin() == 'A');              // true
        cout << (*constants.cbegin() == 'A');             // true
        //*constants.begin() = 'Z'; // compile-time error:
        // read-only variable is not assignable
    }

    ////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/empty
    
    Possible output:
    
    numbers.empty(): false
    no_numbers.empty(): true
    */
    {
        auto numbers = makePgmArray<int, 3, 1, 4, 1>();
        auto no_numbers = makePgmArray<int>();
        
        cout << "numbers.empty(): " << numbers.empty();
        cout << "no_numbers.empty(): " << no_numbers.empty();
    }

    ////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/size
    
    Possible output:
    
    nums contains 4 elements.
    */
    {
        auto nums = makePgmArray<int, 1, 3, 5, 7>();
        cout << "nums contains " << nums.size() << " elements.";
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/size
    
    Possible output:
    
    Maximum size of the array is 10
    */
    {
        auto q = makePgmArray<char, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10>();
        cout << "Maximum size of the array is " << q.max_size();
    }

    //////////////////////////////////////////////////////////////////////////
    /*
    see https://en.cppreference.com/w/cpp/container/array/swap
    
    Possible output:
    
    { 1 2 3 } { 4 5 6 } 1 4 2 5
    { 4 5 6 } { 1 2 3 } 4 1 5 2
    */
    {
        auto a1 = makePgmArray<int, 1, 2, 3>();
        auto a2 = makePgmArray<int, 4, 5, 6>();
        
        auto it1 = a1.begin();
        auto it2 = a2.begin();
        
        cout << '{' << a1 << "} {" << a2 << '}' << *it1 << ' ' << *it2;
        swap(a1,a2);
        cout << '{' << a1 << "} {" << a2 << '}' << *it1 << ' ' << *it2;
        
        // Note that after swap iterators and references stay associated with their original
        // array, e.g. `it1` still points to element a1[0], `ref1` still refers to a1[1].
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
        auto alice = makePgmArray<int, 1, 2, 3>();
        auto bob = makePgmArray<int, 7, 8, 9>();
        auto eve = makePgmArray<int, 1, 2, 3>();
        
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
void throw_out_of_range()
{
    while(true);
}

// debugPrinter specializations
template<typename T>
struct debugPrinter<PgmArray<T>>
{
    static void print(const PgmArray<T>& arg)
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
struct debugPrinter<uint16_t>
{
    static void print(const uint16_t arg)
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


