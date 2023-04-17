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

#include "scheduler.h"

#include "../../common/debug_print.h"

class Task
{
    public:
    
    Task(const uint8_t id) : m_id(id)
    {}
    
    void operator()()
    {
        cout << m_id;
    }
    
    private:
    
    uint8_t m_id;
};

/*
Expected debug console output:
1
2
3
4
5
6
7
8
9
*/
int main(void)
{
    Scheduler<Task, uint8_t,10> testScheduler;
    
    testScheduler.schedule(Task(4), 12);
    testScheduler.schedule(Task(1), 0);
    testScheduler.schedule(Task(5), 23);
    testScheduler.schedule(Task(6), 23);
    testScheduler.schedule(Task(2), 0);
    testScheduler.schedule(Task(9), 34);
    testScheduler.schedule(Task(7), 23);
    testScheduler.schedule(Task(3), 0);
    testScheduler.schedule(Task(8), 23);

    while (true)
    {
        testScheduler.clock();
        testScheduler.execute();
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

template <>
struct debugPrinter<uint8_t>
{
    static void print(const uint8_t arg)
    {
        // Put a tracepoint here and display {arg} in output window
        (void)arg;
    }
};


void throw_nullptr_error()
{
    cout << (const char*)"NULL POINTER !!!";
    while(true);
}

void throw_bad_alloc()
{
    cout << (const char*)"NULL POINTER !!!";
    while(true);
}