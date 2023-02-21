/*
Copyright (C) 2022  Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include <type_traits>

// Debug console printer as described here https://microchipsupport.force.com/s/article/print-out-the-string-variable-into-the-Atmel-studio-output-window
template<typename T>
struct debugPrinter;

/*
Tracepoints don't seem to work in header files, so all used specializations of the debugPrinter must go into a cpp file
*/


//template<typename Arg>
//struct debugPrinter<Arg>
//{
    //static void print(Arg arg)
    //{
        //// Put a tracepoint here and display {arg} in output window
        //(void)arg;
    //}
//};
//
//template<>
//struct debugPrinter<const char*>
//{
    //static void print(const char* arg)
    //{
        //// Put a tracepoint here and display {arg, s} in output window
        //(void)arg;
    //}
//};
//
//template<>
//struct debugPrinter<char>
//{
    //static void print(char arg)
    //{
        //// Put a tracepoint here and display {arg, c} in output window
        //(void)arg;
    //}
//};

#include <type_traits>

struct DebugOutputStream
{
    template<typename T>
    DebugOutputStream & operator<<(const T& t)
    {
        debugPrinter<T>::print(t);
        return *this;
    }
        
    class Iterator
    {
        public:
        Iterator(DebugOutputStream& debugOutput)
        :
        m_debugOutput(debugOutput)
        {}
            
        Iterator& operator++()
        {
            return *this;
        }
            
        DebugOutputStream& operator*()
        {
            return m_debugOutput;
        }
            
        private:
            
        DebugOutputStream& m_debugOutput;
    };

    Iterator begin()
    {
        return Iterator(*this);
    }
        
};
    
inline static DebugOutputStream cout;

#endif