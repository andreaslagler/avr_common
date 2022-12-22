/*
Copyright (C) 2020 Andreas Lagler

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

// Debug console printer as described here https://microchipsupport.force.com/s/article/print-out-the-string-variable-into-the-Atmel-studio-output-window
template<typename Arg, typename... Args>
struct DebugPrinter
{
    static void print(Arg arg, Args ...args) __attribute__((optimize("O0")))
    {
        DebugPrinter<Arg>::print(arg);
        DebugPrinter<Args...>::print(args...);
    }
};

template<typename Arg>
struct DebugPrinter<Arg>
{
    static void print(Arg arg) __attribute__((optimize("O0")))
    {
        // Put a tracepoint here and display {arg} in output window
    }
};

template<>
struct DebugPrinter<const char*>
{
    static void print(const char* arg)
    {
        // Put a tracepoint here and display {arg, s} in output window
    }
};

template<>
struct DebugPrinter<char>
{
    static void print(char arg)
    {
        // Put a tracepoint here and display {arg, c} in output window
    }
};

#include <type_traits>
#include <functional>

struct DebugOutput
{
    template<typename T>
    DebugOutput & operator<<(T t) __attribute__((optimize("O0")));

    template<typename T>
    DebugOutput & operator=(T t)
    {
        return *this << t;
    }
    
    class Iterator
    {
        public:
        Iterator(DebugOutput& debugOutput)
        :
        m_debugOutput(debugOutput)
        {}
        
        Iterator& operator++()
        {
            return *this;
        }
        
        DebugOutput& operator*()
        {
            return m_debugOutput;
        }
        
        
        private:
        DebugOutput& m_debugOutput;
    };

    Iterator begin()
    {
        return Iterator(*this);
    }
    
};


    template<typename T>
    DebugOutput & DebugOutput::operator<<(T t)
    {
        DebugPrinter<typename std::remove_cv<T>::type>::print(t);
        return *this;
    }

static DebugOutput cout;


// Test
// Expected output in debugging console:
// 42
// 43
// 44
// 45

static uint8_t f(const uint8_t i1, const uint8_t i2) {return i1 + i2;}

int main()
{
    // Non-capturing lambda
    std::function<uint8_t()> iv([]{return 42;});
    uint8_t x = iv();
    cout << x;
    
    //iv = nullptr; // Error: Upcast from void* to nullptr_t* does not evaluate to a function
    iv = []{return 43;};
    x = iv();
    cout << x;
    
    // Capturing lambda
    std::function<void()> vv; // OK, Function objects of type void may be assigned to nullptr in default constructor. Dispatcher will take care of nullptr
    vv();
    vv = nullptr; // OK, Function objects of type void may be assigned to nullptr. Dispatcher will take care of nullptr
    vv();
    vv = [&]{x++;};
    vv();
    cout << x;

    // Static function
    //std::function<uint8_t(uint8_t,uint8_t)> iii; // Error: deleted default constructor
    //std::function<uint8_t(uint8_t,uint8_t)> iii(nullptr); // Error: Upcast from void* to nullptr_t* does not evaluate to a function
    std::function<uint8_t(uint8_t,uint8_t)> iii(f);
    iii = f;
    //iii = nullptr; // Error: Upcast from void* to nullptr_t* does not evaluate to a function
    x = iii(x, 1);
    cout << x;
}

