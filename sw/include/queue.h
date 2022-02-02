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

#ifndef QUEUE_H
#define QUEUE_H

#include "list_node.h"
#include "type_traits.h"
#include <stdbool.h>

/**
@brief Template class implementing a queue of objects
@tparam Elem Type of queue elements
@tparam t_volatile Flag indicating if head and tail pointer should be volatile
@note Because of the missing support of std::new in avr-gcc, this queue implementation is not self-contained and does not store copies of the node elements. SingleLinkedNode<Elem> has to be a public base class of Elem.
*/
template <typename Elem, bool t_volatile = false>
class Queue
{
    public:
    
    /// @brief List node type
    typedef SingleLinkedNode<Elem> Node;

    /// @brief Constructor
    constexpr Queue() = default;

    /**
    @brief Append node to the tail of the queue, i.e. the appended node will be the new tail of the queue
    @param node Node to be appended
    */
    constexpr void push(Node & node)
    {
        // This will be the new tail --> Append NULL
        node.append(nullptr);

        if (isEmpty())
        {
            // Queue is empty --> Also replace head of the queue
            m_head = &node;
        }
        else
        {
            // Queue is not empty --> Append to tail
            m_tail->append(&node);
        }

        // Replace tail of the queue
        m_tail = &node;
    }

    /**
    @brief Peek into queue, i.e. get the head of the queue with out detaching it
    @result Pointer to head of the queue
    @note The result will be nullptr if queue is empty
    */
    constexpr Node * peek() const
    {
        return m_head;
    }
    
    /**
    @brief Detach next node from the head of the queue
    @result Pointer to head of the queue
    @note The result will be nullptr if queue is empty
    */
    constexpr Node * pop()
    {
        // Get head node (no matter if it's NULL)
        Node * const head = m_head;
        if (nullptr != head)
        {
            // Queue is not empty --> Replace head by next node
            m_head = head->next();
        }

        return head;
    }

    /**
    @brief Check if queue is empty
    @result Flag indicating if queue is empty
    */
    constexpr bool isEmpty() const
    {
        return (nullptr == m_head);
    }
    
    private:

    // Head of queue
    typename MakeVolatile<Node *, t_volatile>::type m_head {nullptr};
    
    // Tail of queue
    typename MakeVolatile<Node *, t_volatile>::type m_tail {nullptr};
};

#endif