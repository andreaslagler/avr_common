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

#ifndef QUEUE_H
#define QUEUE_H

#include <bits/c++config.h>
#include <bits/move.h>
#include <stdbool.h>

/**
@brief Template class implementing an adapter for a queue of objects based on a different container type
@tparam T Type of queue elements
@tparam Container type of underlying container
*/
template <typename T, typename Container>
class Queue
{
public:
    
    using container_type         =          Container;
    using value_type             = typename Container::value_type;
    using reference              = typename Container::reference;
    using const_reference        = typename Container::const_reference;
    using size_type              = typename Container::size_type;
    using pointer                = typename Container::pointer;
    using const_pointer          = typename Container::const_pointer;
    using difference_type        = typename Container::difference_type;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;
    
    /**
    @brief Constructor.
    Default constructor. Value-initializes the container.
    */
    CXX20_CONSTEXPR Queue() = default;
    
    /**
    @brief Initializing constructor.
    Copy-constructs the underlying container.
    @param container container to be used as source to initialize the underlying container
    */
    CXX20_CONSTEXPR explicit Queue(const container_type& container) : m_container(container)
    {}
    
    /**
    @brief Initializing constructor.
    Move-constructs the underlying container
    @param container container to be used as source to initialize the underlying container
    */
    CXX20_CONSTEXPR explicit Queue(container_type&& container) : m_container(forward<container_type>(container))
    {}
    
    /**
    @brief Initializing constructor.
    Constructs the underlying container with the contents of the range [first, last).
    @param first, last the range to copy the elements from
    */
    template<class InputIt>
    CXX20_CONSTEXPR Queue(InputIt first, InputIt last) : m_container(first, last)
    {}
    
    /**
    @brief copy constructor.
    The adaptor is copy-constructed with the contents of other
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX20_CONSTEXPR Queue(const Queue& other) : Queue(other.m_container)
    {}
    
    /**
    @brief move constructor.
    The adaptor is move-constructed with the contents of other
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX20_CONSTEXPR Queue(Queue&& other) : Queue(move(other.m_container))
    {}
    
    /**
    @brief Destructor.
    Destructs the queue. The destructors of the elements are called and the used storage is deallocated.
    Note, that if the elements are pointers, the pointed-to objects are not destroyed.
    */
    CXX20_CONSTEXPR ~Queue()
    {}
    
    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR Queue& operator=(const Queue& other)
    {
        if (this != &other)
        {
            m_container = other.m_container;
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Move assignment operator. Replaces the contents with those of other using move semantics
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR Queue& operator=(Queue&& other)
    {
        if (this != &other)
        {
            m_container = move(other.m_container);
        }
        return *this;
    }

    /**
    @brief Get const iterator pointing first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return m_container.cbegin();
    }
    
    /**
    @brief Get const iterator pointing to first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator begin() const
    {
        return m_container.begin();
    }
    
    /**
    @brief Get iterator pointing to first element of queue
    @result Begin iterator
    */
    CXX14_CONSTEXPR iterator begin()
    {
        return m_container.begin();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return m_container.cend();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator end() const
    {
        return m_container.end();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue
    @result End iterator
    */
    CXX14_CONSTEXPR iterator end()
    {
        return m_container.end();
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crbegin() const
    {
        return m_container.crbegin();
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rbegin() const
    {
        return m_container.rbegin();
    }
    
    /**
    @brief Get iterator pointing to first element of queue in reverse order
    @result reverse begin iterator
    */
    CXX14_CONSTEXPR reverse_iterator rbegin()
    {
        return m_container.rbegin();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crend() const
    {
        return m_container.crend();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rend() const
    {
        return m_container.rend();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue in reverse order
    @result reverse end iterator
    */
    CXX14_CONSTEXPR reverse_iterator rend()
    {
        return m_container.rend();
    }

    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end()
    @result true if the container is empty, false otherwise
    */
    constexpr bool empty() const
    {
        return m_container.empty();
    }

    /**
    @brief Returns the number of elements
    Returns the number of elements in the container
    @result The number of elements in the container.
    */
    constexpr size_type size() const
    {
        return m_container.size();
    }

    
    /**
    @brief Access the first element
    Returns a reference to the frontPos element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR reference front()
    {
        return m_container.front();
    }
    
    /**
    @brief access the first element (read-only)
    Returns a const reference to the frontPos element in the container.
    Calling front on an empty container is undefined.
    @result Reference to the first element.
    */
    CXX14_CONSTEXPR const_reference front() const
    {
        return m_container.front();
    }
    
    /**
    @brief Access the last element
    Returns a reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR reference back()
    {
        return m_container.back();
    }
    
    /**
    @brief Access the last element (read-only)
    Returns a const reference to the last element in the container.
    Calling last on an empty container is undefined.
    @result Reference to the last element.
    */
    CXX14_CONSTEXPR const_reference back() const
    {
        return m_container.back();
    }
    
    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void push(const value_type& value)
    {
        m_container.pushBack(value);
    }

    /**
    @brief Adds an element to the end
    Appends the given element value to the end of the container using move semantics
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void push(value_type&& value)
    {
        m_container.pushBack(forward<value_type>(value));
    }

    /**
    @brief Constructs an element in-place at the end
    Appends a new element to the end of the container. The element is constructed through placement-new to construct the element in-place at the location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    @param arguments to forward to the constructor of the element
    */
    template <typename  ... Args>
    CXX14_CONSTEXPR reference emplace(Args&& ... args)
    {
        return m_container.emplaceBack(forward<Args>(args)...);
    }

    /**
    @brief Removes the front element
    Removes an element from the front of the queue
    */
    CXX14_CONSTEXPR void pop()
    {
        m_container.popFront();
    }
       
protected:

    // the underlying container
    container_type m_container;
};

/**
@brief Template class implementing an adapter for a priority queue of objects based on a different container type
A priority queue is a container adaptor that provides constant time lookup of the top (e.g. largest) element, at the expense of linear insertion and extraction.
A user-provided Compare can be supplied to change the ordering.
@tparam T The type of the stored elements. The behavior is undefined if T is not the same type as Container::value_type.
@tparam Container type of underlying container
@tparam Compare A Compare type providing a strict weak ordering. Note that the Compare parameter is defined such that it returns true if its first argument comes before its second argument in a weak ordering. But because the priority queue outputs largest elements first, the elements that "come before" are actually output last. That is, the front of the queue contains the "last" element according to the weak ordering imposed by Compare.
*/
template <typename T, typename Container, typename Compare>
class PriorityQueue
{
public:
    
    using compare_type           =          Compare;
    using container_type         =          Container;
    using value_type             = typename Container::value_type;
    using reference              = typename Container::reference;
    using const_reference        = typename Container::const_reference;
    using size_type              = typename Container::size_type;
    using pointer                = typename Container::pointer;
    using const_pointer          = typename Container::const_pointer;
    using difference_type        = typename Container::difference_type;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;
    
    /**
    @brief Constructor.
    Default constructor. Value-initializes the container.
    */
    CXX20_CONSTEXPR PriorityQueue() = default;
    
    /**
    @brief Initializing constructor.
    Copy-constructs the underlying container.
    @param compare the comparison function object to initialize the underlying comparison functor
    @param container container to be used as source to initialize the underlying container
    */
    CXX20_CONSTEXPR PriorityQueue(const compare_type compare, const container_type& container) : m_compare(compare)
    {
        for (const value_type& value : container)
        {
            push(value);
        }
    }
    
    /**
    @brief Initializing constructor.
    Move-constructs the underlying container
    @param compare the comparison function object to initialize the underlying comparison functor
    @param container container to be used as source to initialize the underlying container
    */
    CXX20_CONSTEXPR PriorityQueue(const compare_type compare, container_type&& container) : m_compare(compare)
    {
        for (value_type& value : container)
        {
            push(move(value));
        }
    }
    
    /**
    @brief Initializing constructor.
    Copy-constructs the underlying container.
    @param container container to be used as source to initialize the underlying container
    */
    CXX20_CONSTEXPR PriorityQueue(const container_type& container)
    {
        for (const value_type& value : container)
        {
            push(value);
        }
    }
    
    /**
    @brief Initializing constructor.
    Move-constructs the underlying container
    @param container container to be used as source to initialize the underlying container
    */
    CXX20_CONSTEXPR PriorityQueue(container_type&& container)
    {
        for (value_type& value : container)
        {
            push(move(value));
        }
    }
    
    /**
    @brief Initializing constructor.
    Constructs the underlying container with the contents of the range [first, last).
    @param first, last the range to copy the elements from
    @param compare the comparison function object to initialize the underlying comparison functor
    */
    template<class InputIt>
    CXX20_CONSTEXPR PriorityQueue(InputIt first, InputIt last, const compare_type& compare = compare_type()) : m_compare(compare)
    {
        while (first != last)
        {
            push(*first);
            ++first;
        }
    }
    
    /**
    @brief copy constructor.
    The adaptor is copy-constructed with the contents of other
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX20_CONSTEXPR PriorityQueue(const PriorityQueue& other) : m_compare(other.m_compare), m_container(other.m_container)
    {}
    
    /**
    @brief move constructor.
    The adaptor is move-constructed with the contents of other
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX20_CONSTEXPR PriorityQueue(PriorityQueue&& other) : m_compare(other.m_compare), m_container(move(other.m_container))
    {}
    
    /**
    @brief Destructor.
    Destructs the queue. The destructors of the elements are called and the used storage is deallocated.
    Note, that if the elements are pointers, the pointed-to objects are not destroyed.
    */
    CXX20_CONSTEXPR ~PriorityQueue()
    {}
    
    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR PriorityQueue& operator=(const PriorityQueue& other)
    {
        if (this != &other)
        {
            m_compare = other.m_compare;
            m_container = other.m_container;
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Move assignment operator. Replaces the contents with those of other using move semantics
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR PriorityQueue& operator=(PriorityQueue&& other)
    {
        if (this != &other)
        {
            m_compare = other.m_compare;
            m_container = move(other.m_container);
        }
        return *this;
    }

    /**
    @brief Get const iterator pointing first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return m_container.cbegin();
    }
    
    /**
    @brief Get const iterator pointing to first element of queue
    @result begin const iterator
    */
    CXX14_CONSTEXPR const_iterator begin() const
    {
        return m_container.begin();
    }
    
    /**
    @brief Get iterator pointing to first element of queue
    @result Begin iterator
    */
    CXX14_CONSTEXPR iterator begin()
    {
        return m_container.begin();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return m_container.cend();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue
    @result End const iterator
    */
    CXX14_CONSTEXPR const_iterator end() const
    {
        return m_container.end();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue
    @result End iterator
    */
    CXX14_CONSTEXPR iterator end()
    {
        return m_container.end();
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crbegin() const
    {
        return m_container.crbegin();
    }
    
    /**
    @brief Get const iterator pointing to frontPos element of queue in reverse order
    @result reverse begin const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rbegin() const
    {
        return m_container.rbegin();
    }
    
    /**
    @brief Get iterator pointing to first element of queue in reverse order
    @result reverse begin iterator
    */
    CXX14_CONSTEXPR reverse_iterator rbegin()
    {
        return m_container.rbegin();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator crend() const
    {
        return m_container.crend();
    }
    
    /**
    @brief Get const iterator pointing to last plus one element of queue in reverse order
    @result reverse end const iterator
    */
    CXX14_CONSTEXPR const_reverse_iterator rend() const
    {
        return m_container.rend();
    }
    
    /**
    @brief Get iterator pointing to last plus one element of queue in reverse order
    @result reverse end iterator
    */
    CXX14_CONSTEXPR reverse_iterator rend()
    {
        return m_container.rend();
    }

    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end()
    @result true if the container is empty, false otherwise
    */
    constexpr bool empty() const
    {
        return m_container.empty();
    }

    /**
    @brief Returns the number of elements
    Returns the number of elements in the container
    @result The number of elements in the container.
    */
    constexpr size_type size() const
    {
        return m_container.size();
    }

    
    /**
    @brief Access the top element
    Returns reference to the top element in the priority queue
    @result Reference to the top element.
    */
    CXX14_CONSTEXPR reference top()
    {
        return m_container.front();
    }
    
    /**
    @brief Access the top element
    Returns const reference to the top element in the priority queue
    @result Reference to the top element.
    */
    CXX14_CONSTEXPR const_reference top() const
    {
        return m_container.front();
    }
    
    /**
    @brief Adds an element to the queue keeping the sort order
    Appends the given element value to the container keeping the sort order according to used compare functor. The new element is initialized as a copy of value.
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void push(const value_type& value)
    {
        // TODO use binary heap ???
        const_iterator itCurrent = cbegin();
        const_iterator itEnd = cend();
        while (itCurrent != itEnd)
        {
            if (m_compare(value, *itCurrent))
            {
                break;
            }
            ++itCurrent;
        }
        m_container.insert(itCurrent, value);
    }

    /**
    @brief Adds an element to the queue keeping the sort order
    Appends the given element value to the container keeping the sort order according to used compare functor and using move semantics
    @param value The value of the element to append
    */
    CXX14_CONSTEXPR void push(value_type&& value)
    {
        // TODO use binary heap ???
        const_iterator itCurrent = cbegin();
        const_iterator itEnd = cend();
        while (itCurrent != itEnd)
        {
            if (m_compare(value, *itCurrent))
            {
                break;
            }
            ++itCurrent;
        }
        m_container.insert(itCurrent, forward<value_type>(value));
    }

    /**
    @brief Constructs an element in-place keeping the sort order
    Appends a new element to the end of the container. The element is constructed through placement-new to construct the element in-place at the location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    @param args arguments to forward to the constructor of the element
    */
    template <typename  ... Args>
    CXX14_CONSTEXPR void emplace(Args&& ... args)
    {
        push(value_type(forward<Args>(args)...));
    }

    /**
    @brief Removes the top element
    Removes an element from the top of the queue
    */
    CXX14_CONSTEXPR void pop()
    {
        m_container.popFront();
    }
       
protected:

    // the underlying compare functor
    compare_type m_compare;
    
    // the underlying container
    container_type m_container;
};

#endif