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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "list_node.h"
#include "subject.h"
#include "queue.h"

/**
@brief This class implements a simple queue-based task scheduler.
Scheduling and executing tasks can be made interrupt-safe using an Atomic class implementation as described below
@tparam Atomic The Atomic template parameter is supposed to work as follows:
- In the Atomic constructor, the ISR calling Scheduler::Clock() should be locked, so the queue access is atomic regarding interrupts.
- In the Atomic destructor, the interrupt flags are restored to  allow  for regular operation.
The implementation of Atomic (i.e. the access to interrupt enable flags) is device-specific.
In case Atomic is void, the scheduler has zero overhead, but interrupt-safety must be ensured outside the scope of Scheduler.
*/
template <typename Atomic = void>
class Scheduler;

/**
@brief Default scheduler class
*/
template <>
class Scheduler<void>
{
    public:

    /**
    @brief Scheduler delay data type
    Scheduler clock resolution of 1 ms allows delay in the range of 1ms ... 1min
    */
    typedef uint16_t Delay;
    
    /**
    @brief The task class combines a linked list node for insertion into the scheduler queue and an observable subject, so it can notify an observer when executed
    @todo Consider an abstract base class and dynamic polymorphism
    */
    class Task : public SingleLinkedNode<Task>, public Subject<void>
    {
        public:

        /**
        @brief Default constructor
        @param observer Callback for task execution 
        */
        constexpr Task(typename Subject<void>::Observer observer = nullptr) : Subject<void>(observer)
        {}

        /// @brief Task execution callback
        constexpr void execute() const
        {
            Subject<void>::notifiyObserver();
        }

        private:

        // Allow the scheduler to alter the relative delay when scheduling tasks
        friend class Scheduler;

        // Relative delay wrt previous task
        Delay m_delay {0};
    };

    /**
    @brief Move the scheduler clock forward by one tick, i.e. decrement the relative delay of the next scheduled task. Move task(s) with zero relative delay to execution queue.
    */
    void clock()
    {
        Task * task;
        while (nullptr != (task = m_schedulerHead))
        {
            // Check if tasks are scheduled now, i.e. their delay is zero
            if (0 == task->m_delay)
            {
                // Move current task from scheduler queue to execution queue
                m_schedulerHead = static_cast<Task*>(task->next());
                m_executionQueue.push(*task);
            }
            else
            {
                // Decrement delay of next task
                --task->m_delay;
                break;
            }
        }
    }
    
    /**
    @brief Schedule task, i.e. insert task into scheduler queue according to its delay
    @param task Task to be scheduled
    @param delay Execution delay of the task in clock ticks
    */
    void schedule(Task & task, Delay delay)
    {
        Task *pos = m_schedulerHead;
        if (nullptr == pos)
        {
            // Scheduler queue is empty --> queue current task directly
            m_schedulerHead = &task;

            // This will be the new tail anyway --> Append NULL
            task.append(nullptr);
        }
        else if (delay < pos->m_delay)
        {
            // Current task has shorter delay than next scheduled task
            // --> replace next scheduled task by current task
            
            // Adjust delay of the the replaced task
            pos->m_delay -= delay;

            // Append front to new node
            task.append(pos);

            // Replace head of the queue
            m_schedulerHead = &task;
        }
        else
        {
            // Current task has longer (or same) delay than next scheduled task
            // --> skip scheduled tasks according to their delay
            Task * next = nullptr;
            while (true)
            {
                // Decrease relative delay to previous task
                delay -= pos->m_delay;

                // Check next task in queue
                next = static_cast<Task*>(pos->next());
                if (nullptr != next)
                {
                    // End of queue not reached yet --> Check delay of next task
                    if (delay >= next->m_delay)
                    {
                        // New task is scheduled after next --> Iterate to next task
                        pos = next;
                    }
                    else
                    {
                        // Adjust delay of next task
                        next->m_delay -= delay;
                        break;
                    }
                }
                else
                {
                    // End of queue reached --> break
                    break;
                }
            }

            // Insert new task between pos and next
            task.append(next);
            pos->append(&task);
        }

        // Finally, set the delay
        task.m_delay = delay;
    }

    /**
    @brief Get pointer to the first task in the execution queue (if available)
    */
    Task * getNextTask()
    {
        return static_cast<Task*>(m_executionQueue.pop());
    }

    /**
    @brief Execute next task in the execution queue
    */
    void executeNextTask()
    {
        Task * task = getNextTask();
        if (nullptr != task)
        {
            task->execute();
        }
    }

    private:

    // Scheduler head. This points to the next scheduled task
    Task * volatile m_schedulerHead;
    
    // Queue of tasks ready for execution. Query into this queue to execute tasks
    Queue<Task, true> m_executionQueue;
};

// Implementation
template <typename Atomic>
class Scheduler : public Scheduler<void>
{
    public:

    using Scheduler<void>::Delay;
    using typename Scheduler<void>::Task;

    /**
    @brief Schedule task, i.e. insert task into scheduler queue according to its delay
    @param task Task to be scheduled
    @param delay Execution delay of the task in clock ticks
    */
    void schedule(Task & task, const Delay delay)
    {
        const Atomic atomic;
        Scheduler<void>::schedule(task, delay);
    }

    /**
    @brief Get pointer to the first task in the execution queue (if available)
    */
    Task * getNextTask()
    {
        const Atomic atomic;
        return Scheduler<void>::getNextTask();
    }
};

#endif