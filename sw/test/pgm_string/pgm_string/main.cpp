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

//// std::new / std::delete prolog to satisfy compiler
//#include <cstdlib>
//__extension__ typedef int __guard __attribute__((mode (__DI__)));
//int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}
//void __cxa_guard_release (__guard *g) {*(char *)g = 1;}
//void __cxa_guard_abort (__guard *) {}
//
//void * operator new(size_t size)
//{
    //return malloc(size);
//}
//
//void * operator new[](size_t size)
//{
    //return malloc(size);
//}
//
//void operator delete(void * ptr)
//{
    //free(ptr);
//}
//
//void operator delete[](void * ptr)
//{
    //free(ptr);
//}
//
//void operator delete(void * ptr, size_t)
//{
    //free(ptr);
//}
//
//void operator delete[](void * ptr, size_t)
//{
    //free(ptr);
//}
//
//// Exception handlers to satisfy linker
//// See bits\functexcept.h
//namespace std
//{
    //void __throw_length_error(const char*)
    //{
        //while(true);
    //}
//
    //void  __throw_bad_alloc()
    //{
        //while(true);
    //}
//}


// Debug console printer as described here https://microchipsupport.force.com/s/article/print-out-the-string-variable-into-the-Atmel-studio-output-window
template<typename Arg, typename... Args>
struct debugPrinter
{
    static void print(Arg arg, Args ...args)
    {
        debugPrinter<Arg>::print(arg);
        debugPrinter<Args...>::print(args...);
    }
};

template<typename Arg>
struct debugPrinter<Arg>
{
    static void print(Arg arg)
    {
        // Put a tracepoint here and display {arg} in output window
    }
};

template<>
struct debugPrinter<char*>
{
    static void print(char* arg)
    {
        // Put a tracepoint here and display {arg, s} in output window
    }
};

template<>
struct debugPrinter<char>
{
    static void print(char arg)
    {
        // Put a tracepoint here and display {arg, c} in output window
    }
};

template<typename... Args>
void debugPrint(Args ...args)
{
    debugPrinter<Args...>::print(args...);
}

#include <type_traits>

namespace std
{
    struct DebugOutput
    {
        template<typename T>
        DebugOutput & operator<<(T t)
        {
            debugPrinter<typename std::remove_cv<T>::type>::print(t);
            return *this;
        }

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
    
    static DebugOutput cout;
}

template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first)
{
    for (; first != last; (void)++first, (void)++d_first)
    {
        *d_first = *first;
    }
    
    return d_first;
}

#include "pgm_string.h"

constexpr PgmString pgmString = "Hello World"_pgm;

int main()
{    
    std::cout << pgmString.size();
    copy(pgmString.begin(), pgmString.end(), std::cout.begin());

    while (true);
}