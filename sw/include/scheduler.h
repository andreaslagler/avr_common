/*
Copyright (C) 2023  Andreas Lagler

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

#include <bits/c++config.h>
#include <queue.h>
#include <list.h>
#include <static_list.h>
#include <utility.h>

#include <stdbool.h>
#include <atomic.h>

/**
@brief Implementation of a simple queue-based task scheduler.
This implementation is interrupt-safe (i.e. call schedule() and execute() in application code and clock() in ISR)
@tparam Task task type to be scheduled. Task must specify operator()(void) or equivalent
@tparam Delay delay clock tick type
@tparam t_capacity Maximum number of tasks scheduled at the same time. If t_capacity is 0, the actual maximum number of tasks is limited by available heap memory
*/
template <typename Task, typename Delay, size_t t_capacity = 0>
class Scheduler
{
    public:
    
    /**
    @brief Schedule a task
    If two tasks have the same delay, the task scheduled first will be executed first
    @param task task to be scheduled
    @param delay delay of task given in clock ticks
    */
    CXX14_CONSTEXPR void schedule(const Task& task, const Delay delay)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            // Check delay
            if (0 == delay)
            {
                // Delay is 0 --> Append task to queue of due tasks
                m_dueTasks.push(task);
            }
            else
            {
                // Schedule task
                m_scheduledTasks.emplace(delay, task);
            }
        }        
    }
    
    /**
    @brief Execute next task
    Execute next due task (if there is any)
    @result true if a task has been executed, false otherwise
    */
    CXX14_CONSTEXPR bool execute()
    {
        // Check if a task is due (atomic)
        bool tasksDue = false;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            tasksDue = !m_dueTasks.empty();
        }
        
        if (tasksDue)
        {
            // Get next task from queue (atomic)
            Task* task = nullptr;            
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                task = &m_dueTasks.front();
            }                
            
            // Execute the task, non-atomic
            task->operator()();            
            
            // Delete the task after execution (atomic)
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                m_dueTasks.pop();
            }            
            
            // Indicate that a task has been executed
            return true;
        }
        
        // Indicate that no task has been executed
        return false;
    }
    
    /**
    @brief Clock the scheduler
    Increase the scheduler clock by one clock tick. Move due tasks to the queue of due tasks. This method can be used as a callback for a timer interrupt, all operations ar non-atomic
    */
    CXX14_CONSTEXPR void clock()
    {
        // Check for scheduled tasks
        if (!m_scheduledTasks.empty())
        {
            // Check delay of next task
            ScheduledTask& task = m_scheduledTasks.top();
            if (0 == (--task.first))
            {
                // Task is due --> move task from queue of scheduled tasks to queue of due tasks
                m_dueTasks.push(move(task.second));
                m_scheduledTasks.pop();
            }
        }
        
        // Check for more scheduled tasks with same delay
        while (!m_scheduledTasks.empty())
        {
            // Check delay of next task
            ScheduledTask& task = m_scheduledTasks.top();
            if (0 == task.first)
            {
                // Task is due --> move task from queue of scheduled tasks to queue of due tasks
                m_dueTasks.push(move(task.second));
                m_scheduledTasks.pop();
            }
            else
            {
                // No more due tasks
                break;
            }
        }
    }
    
    private:
    
    using ScheduledTask = Pair<Delay,Task>;
    
    // Compare functor used to schedule tasks
    struct Compare
    {
        CXX14_CONSTEXPR bool operator()(ScheduledTask& task, const ScheduledTask& nextTask)
        {
            if (task.first >= nextTask.first)
            {
                // Decrease relative delay of task with respect to next task
                task.first -= nextTask.first;
                
                // Schedule task AFTER nextTask
                return false;
            }
            
            // Schedule task BEFORE nextTask
            return true;
        }
    };
    
    // Queue of scheduled (i.e. delayed) tasks
    PriorityQueue<ScheduledTask, typename conditional<t_capacity == 0, List<ScheduledTask>, StaticList<ScheduledTask, t_capacity>>::type, Compare> m_scheduledTasks;
    
    // Queue of due tasks
    Queue<Task, typename conditional<t_capacity == 0, List<Task>, StaticList<Task, t_capacity>>::type> m_dueTasks;
};

#endif