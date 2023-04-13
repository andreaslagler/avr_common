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

#ifndef STATIC_LIST_H
#define STATIC_LIST_H

#include <bits/c++config.h>
#include <bits/move.h>
#include <bits/new.h>
#include <exception.h>
#include <initializer_list>
#include <stdbool.h>

/**
@brief Template class implementing a list of objects with static memory allocation
@tparam T Type of list elements
@tparam t_capacity Compile time constant capacity of the container in elements of type T
*/
template <typename T, size_t t_capacity>
class StaticList
{
    class NodeBase;
    class Node;
    
    public:
    
    template <bool t_const, bool t_reverse>
    class Iterator;
    
    using value_type             = T;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = Iterator<false, false>;
    using const_iterator         = Iterator<true, false>;
    using reverse_iterator       = Iterator<false, true>;
    using const_reverse_iterator = Iterator<true, true>;

    template <bool t_const, bool t_reverse>
    class Iterator
    {
        friend class StaticList<T, t_capacity>;

        CXX20_CONSTEXPR Iterator(NodeBase* node) : m_node(node)
        {}
            
        template <bool t_rhsConst, bool t_rhsReverse>
        CXX20_CONSTEXPR Iterator(const Iterator<t_rhsConst, t_rhsReverse>& rhs)
        {
            m_node = rhs.m_node;
        }

        template <bool t_rhsConst, bool t_rhsReverse>
        CXX20_CONSTEXPR Iterator& operator=(const Iterator<t_rhsConst, t_rhsReverse>& rhs)
        {
            m_node = rhs.m_node;
            return *this;
        }
         
        public:        
        
        CXX20_CONSTEXPR Iterator(const Iterator& rhs)
        {
            m_node = rhs.m_node;
        }

        CXX20_CONSTEXPR Iterator& operator=(const Iterator& rhs)
        {
            m_node = rhs.m_node;
            return *this;
        }
        
        CXX20_CONSTEXPR ~Iterator() = default;
        
        CXX14_CONSTEXPR Iterator& operator++()
        {
            if (nullptr != m_node)
            {
                if CXX17_CONSTEXPR(t_reverse)
                {
                    m_node = m_node->m_prev;
                }
                else
                {
                    m_node = m_node->m_next;
                }
            }
            return *this;
        }

        constexpr typename conditional<t_const, const value_type, value_type>::type& operator*() const
        {
            if (nullptr == m_node)
            {
                throw_nullptr_error();
            }
            return static_cast<Node*>(m_node)->m_data;
        }

        constexpr bool operator!=(const Iterator& other)
        {
            return m_node != other.m_node;
        }

        private:
        
        NodeBase* m_node = nullptr;
    };

    /**
    @brief constructs the StaticList
    Constructs an empty container with the given allocator allocator
    */
    CXX14_CONSTEXPR explicit StaticList()
    {
        init();
    }
    
    /**
    @brief constructs the StaticList
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    */
    CXX14_CONSTEXPR explicit StaticList(size_type count)
    {
        init();
        NodeBase* prev = &m_front;
        while (count--)
        {
            prev = new (allocateNode()) Node(prev, &m_back);
        }
    }
    
    /**
    @brief constructs the StaticList
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    @param value the value to initialize elements of the container with
    */
    CXX14_CONSTEXPR StaticList(size_type count, const value_type& value)
    {
        init();
        NodeBase* prev = &m_front;
        while (count--)
        {
            prev = new (allocateNode()) Node(prev, &m_back, value);
        }
    }
    
    /**
    @brief constructs the StaticList
    Constructs the container with the contents of the range [first, last).
    @param first, last the range to copy the elements from
    */
    template< class InputIt >
    CXX14_CONSTEXPR StaticList(InputIt first, InputIt last)
    {
        init();
        assign(first, last);
    }
    
    /**
    @brief constructs the StaticList
    Copy constructor. Constructs the container with the copy of the contents of other.
    @param other another container to be used as source to initialize the elements of the container with
    */
    constexpr StaticList(const StaticList& other) : StaticList(other.begin(), other.end())
    {}
    
    /**
    @brief constructs the StaticList
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param init initializer list to initialize the elements of the container with
    */
    constexpr StaticList(std::initializer_list<value_type> init) : StaticList(init.begin(), init.end())
    {}
    
    /**
    @brief constructs the StaticList
    Move constructor. Constructs the container with the contents of other using move semantics. Allocator is obtained from the allocator belonging to other.
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX14_CONSTEXPR StaticList(StaticList&& other)
    {
        init();
        for (value_type& value : other)
        {
            pushBack(move(value));
        }            
    }
    
    /**
    @brief destructs the StaticList
    Destructs the StaticList. The destructors of the elements are called and the used storage is deallocated.
    Note, that if the elements are pointers, the pointed-to objects are not destroyed.
    */
    CXX20_CONSTEXPR ~StaticList()
    {
        clear();
    }

    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR StaticList& operator=(const StaticList& other)
    {
        if (this != &other)
        {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Move assignment operator. Replaces the contents with those of other using move semantics (i.e. the data in other is moved from other into this container).
    other is in a valid but unspecified state afterwards.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR StaticList& operator=(StaticList&& other)
    {
        if (this != &other)
        {
            // Reuse existing nodes of this container
            iterator srcCurrent = other.begin();
            const iterator srcEnd = other.end();
            iterator dstCurrent = begin();
            const iterator dstEnd = end();
            while (dstCurrent != dstEnd && srcCurrent != srcEnd)
            {
                *dstCurrent = move(*srcCurrent);
                ++dstCurrent;
                ++srcCurrent;
            }
        
            // Erase excess nodes
            erase(dstCurrent, cend());
        
            // Insert new nodes as needed
            while (srcCurrent != srcEnd)
            {
                pushBack(move(*srcCurrent));
                ++srcCurrent;
            }
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Replaces the contents with those identified by initializer list init.
    @param init initializer list to use as data source
    */
    CXX14_CONSTEXPR StaticList& operator=(std::initializer_list<value_type> init)
    {
        assign(init);
        return *this;
    }

    /**
    @brief assigns values to the container
    Replaces the contents with count copies of value value
    @param count the new size of the container
    @param value the value to initialize elements of the container with
    */
    CXX14_CONSTEXPR void assign(size_type count, const value_type& value)
    {
        // Reuse existing nodes of this container
        iterator dstCurrent = begin();
        const iterator dstEnd = end();
        while (dstCurrent != dstEnd && count > 0)
        {
            *dstCurrent = value;
            ++dstCurrent;
            --count;
        }
        
        // Erase excess nodes
        erase(dstCurrent, cend());
        
        // Insert new nodes as needed
        insert(dstCurrent, count, value);
    }

    /**
    @brief assigns values to the container
    Replaces the contents with copies of those in the range [first, last).
    The behavior is undefined if either argument is an iterator into *this.
    @param first, last the range to copy the elements from
    */
    template <class InputIt>
    CXX14_CONSTEXPR void assign(InputIt first, InputIt last)
    {
        // Reuse existing nodes of this container
        iterator dstCurrent = begin();
        const iterator dstEnd = end();
        while (dstCurrent != dstEnd && first != last)
        {
            *dstCurrent = *first;
            ++dstCurrent;
            ++first;
        }
        
        // Erase excess nodes
        erase(dstCurrent, cend());
        
        // Insert new nodes as needed
        insert(dstCurrent, first, last);
    }
    
    /**
    @brief assigns values to the container
    Replaces the contents with the elements from the initializer list init.
    @param init initializer list to copy the values from
    */
    CXX14_CONSTEXPR void assign(std::initializer_list<value_type> init)
    {
        assign(init.begin(), init.end());
    }
       
    /**
    @brief access the first element
    Returns a reference to the first element in the container.
    Calling front on an empty container causes undefined behavior.
    @result reference to the first element
    */
    CXX14_CONSTEXPR reference front()
    {
        return static_cast<Node*>(m_front.m_next)->m_data;
    }
    
    /**
    @brief access the first element
    Returns a reference to the first element in the container.
    Calling front on an empty container causes undefined behavior.
    @result reference to the first element
    */
    CXX14_CONSTEXPR const_reference front() const
    {
        return static_cast<Node*>(m_front.m_next)->m_data;
    }
    
    /**
    @brief access the last element
    Returns a reference to the last element in the container.
    Calling back on an empty container causes undefined behavior.
    @result reference to the last element
    */
    CXX14_CONSTEXPR reference back()
    {
        return static_cast<Node*>(m_back.m_prev)->m_data;
    }
    
    /**
    @brief access the last element
    Returns a reference to the last element in the container.
    Calling back on an empty container causes undefined behavior.
    @result reference to the last element
    */
    CXX14_CONSTEXPR const_reference back() const
    {
        return static_cast<Node*>(m_back.m_prev)->m_data;
    }
    
    /**
    @brief Returns an iterator to the beginning
    Returns an iterator to the first element of the StaticList.
    If the StaticList is empty, the returned iterator will be equal to end().
    @result Iterator to the first element.
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return m_front.m_next;
    }
    
    /**
    @brief Returns an iterator to the beginning
    Returns an iterator to the first element of the StaticList.
    If the StaticList is empty, the returned iterator will be equal to end().
    @result Iterator to the first element.
    */
    CXX14_CONSTEXPR const_iterator begin() const
    {
        return m_front.m_next;
    }
    
    /**
    @brief Returns an iterator to the beginning
    Returns an iterator to the first element of the StaticList.
    If the StaticList is empty, the returned iterator will be equal to end().
    @result Iterator to the first element.
    */
    CXX14_CONSTEXPR iterator begin()
    {
        return m_front.m_next;
    }
    
    /**
    @brief Returns an iterator to the end
    Returns an iterator to the element following the last element of the StaticList.
    This element acts as a placeholder; attempting to access it results in undefined behavior.
    @result Iterator to the element following the last element.
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return const_cast<NodeBase*>(&m_back);
    }
    
    /**
    @brief Returns an iterator to the end
    Returns an iterator to the element following the last element of the StaticList.
    This element acts as a placeholder; attempting to access it results in undefined behavior.
    @result Iterator to the element following the last element.
    */
    CXX14_CONSTEXPR const_iterator end() const
    {
        return const_cast<NodeBase*>(&m_back);
    }
    
    /**
    @brief Returns an iterator to the end
    Returns an iterator to the element following the last element of the StaticList.
    This element acts as a placeholder; attempting to access it results in undefined behavior.
    @result Iterator to the element following the last element.
    */
    CXX14_CONSTEXPR iterator end()
    {
        return &m_back;
    }

    /**
    @brief Returns a reverse iterator to the beginning
    Returns a reverse iterator to the first element of the reversed list.
    It corresponds to the last element of the non-reversed list.
    If the list is empty, the returned iterator is equal to rend().
    @result Reverse iterator to the first element.
    */
    CXX14_CONSTEXPR const_reverse_iterator crbegin() const
    {
        return m_back.m_prev;
    }
    
    /**
    @brief Returns a reverse iterator to the beginning
    Returns a reverse iterator to the first element of the reversed list.
    It corresponds to the last element of the non-reversed list.
    If the list is empty, the returned iterator is equal to rend().
    @result Reverse iterator to the first element.
    */
    CXX14_CONSTEXPR const_reverse_iterator rbegin() const
    {
        return m_back.m_prev;
    }
    
    /**
    @brief Returns a reverse iterator to the beginning
    Returns a reverse iterator to the first element of the reversed list.
    It corresponds to the last element of the non-reversed list.
    If the list is empty, the returned iterator is equal to rend().
    @result Reverse iterator to the first element.
    */
    CXX14_CONSTEXPR reverse_iterator rbegin()
    {
        return m_back.m_prev;
    }
    
    /**
    @brief Returns a reverse iterator to the end
    Returns a reverse iterator to the element following the last element of the reversed list.
    It corresponds to the element preceding the first element of the non-reversed list.
    This element acts as a placeholder, attempting to access it results in undefined behavior.
    @result Reverse iterator to the element following the last element.
    */
    CXX14_CONSTEXPR const_reverse_iterator crend() const
    {
        return const_cast<NodeBase*>(&m_front);
    }
    
    /**
    @brief Returns a reverse iterator to the end
    Returns a reverse iterator to the element following the last element of the reversed list.
    It corresponds to the element preceding the first element of the non-reversed list.
    This element acts as a placeholder, attempting to access it results in undefined behavior.
    @result Reverse iterator to the element following the last element.
    */
    CXX14_CONSTEXPR const_reverse_iterator rend() const
    {
        return const_cast<NodeBase*>(&m_front);
    }
    
    /**
    @brief Returns a reverse iterator to the end
    Returns a reverse iterator to the element following the last element of the reversed list.
    It corresponds to the element preceding the first element of the non-reversed list.
    This element acts as a placeholder, attempting to access it results in undefined behavior.
    @result Reverse iterator to the element following the last element.
    */
    CXX14_CONSTEXPR reverse_iterator rend()
    {
        return &m_front;
    }

    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end().
    @result true if the container is empty, false otherwise
    */
    [[nodiscard]] constexpr bool empty() const
    {
        return (m_front.m_next == &m_back);
    }
    
    /**
    @brief returns the number of elements
    @result The number of elements in the container.
    */
    [[nodiscard]] constexpr size_type size() const
    {
        size_type nofElems = 0;
        const_iterator itCurrent = cbegin();
        const_iterator itEnd = cend();
        
        while (itCurrent != itEnd)
        {
            ++nofElems;
            ++itCurrent;
        }
        return nofElems;
    }
    
    /**
    @brief Constructs an element in-place at the beginning
    Inserts a new element to the beginning of the container.
    The element is constructed through placement-new to construct the element in-place at the location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    No iterators or references are invalidated.
    @param args arguments to forward to the constructor of the element
    @result A reference to the inserted element
    */
    template<typename ... Args>
    CXX14_CONSTEXPR reference emplaceFront(Args&&... args)
    {
        // Create new node after dummy front node
        return (new (allocateNode()) Node(&m_front, m_front.m_next, forward<Args...>(args...)))->m_data;
    }
    
    /**
    @brief Constructs an element in-place at the end
    Inserts a new element to the end of the container.
    The element is constructed through placement-new to construct the element in-place at the location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    No iterators or references are invalidated.
    @param args arguments to forward to the constructor of the element
    @result A reference to the inserted element
    */
    template<typename ... Args>
    CXX14_CONSTEXPR reference emplaceBack(Args&&... args)
    {
        // Create new node before dummy back node
        return (new (allocateNode()) Node(m_back.m_prev, &m_back, forward<Args...>(args...)))->m_data;
    }
    
    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    No iterators or references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(const value_type& value)
    {
        // Create new node after dummy front node
        new (allocateNode()) Node(&m_front, m_front.m_next, value);
    }

    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    No iterators or references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(value_type&& value)
    {
        // Create new node after dummy front node
        new (allocateNode()) Node(&m_front, m_front.m_next, forward<value_type>(value));
    }

    /**
    @brief Removes the first element
    Removes the first element of the container. If there are no elements in the container, the behavior is undefined.
    References and iterators to the erased element are invalidated.
    */
    CXX14_CONSTEXPR void popFront()
    {
        NodeBase* node = m_front.m_next;
        if (&m_back != node)
        {
            link(m_front, *node->m_next);
            deleteNode(static_cast<Node*>(node));
        }
    }
    
    /**
    @brief adds an element to the end
    Appends the given element value to the end of the container.
    No iterators or references are invalidated.
    @param value the value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(const value_type& value)
    {
        // Create new node after dummy front node
        new (allocateNode()) Node(m_back.m_prev, &m_back, value);
    }

    /**
    @brief adds an element to the end
    Appends the given element value to the end of the container.
    No iterators or references are invalidated.
    @param value the value of the element to append
    */
    CXX14_CONSTEXPR void pushBack(value_type&& value)
    {
        // Create new node after dummy front node
        new (allocateNode()) Node(m_back.m_prev, &m_back, forward<value_type>(value));
    }

    /**
    @brief Removes the last element
    Removes the last element of the container. If there are no elements in the container, the behavior is undefined.
    References and iterators to the erased element are invalidated.
    */
    CXX14_CONSTEXPR void popBack()
    {
        NodeBase* node = m_back.m_prev;
        if (&m_front != node)
        {
            link(m_back, *node->m_prev);
            deleteNode(static_cast<Node*>(node));
        }
    }
    
    /**
    @brief Constructs element in-place
    Inserts a new element to the beginning of the container.
    The element is constructed through placement-new to construct the element in-place at a location provided by the container.
    The arguments args... are forwarded to the constructor as forward<Args>(args)....
    No iterators or references are invalidated.
    @param args arguments to forward to the constructor of the element
    @result Iterator pointing to the emplaced element
    */
    template<class... Args>
    CXX14_CONSTEXPR iterator emplace(const_iterator pos, Args&&... args)
    {
        // Create new node before pos
        return new (allocateNode()) Node(pos.m_node->m_prev, pos.m_node, forward<Args...>(args...));
    }
    
    /**
    @brief Inserts elements
    inserts value before pos
    @param pos Iterator before which the content will be inserted
    @param value Element value to insert
    @result Iterator pointing to the inserted element.
    */
    constexpr iterator insert(const_iterator pos, const value_type& value)
    {
        // Create new node before pos
        return new (allocateNode()) Node(pos.m_node->m_prev, pos.m_node, value);
    }

    /**
    @brief Inserts elements
    inserts value before pos
    @param pos Iterator before which the content will be inserted
    @param value Element value to insert
    @result Iterator pointing to the inserted element.
    */
    constexpr iterator insert(const_iterator pos, value_type&& value)
    {
        // Create new node before pos
        return new (allocateNode()) Node(pos.m_node->m_prev, pos.m_node, forward<value_type>(value));
    }
    
    /**
    @brief Inserts elements
    inserts count copies of the value before pos
    @param pos Iterator before which the content will be inserted
    @param value Element value to insert
    @param count number of copies to insert
    @result Iterator to the last element inserted, or pos if count==0
    */
    constexpr iterator insert(const_iterator pos, size_type count, const value_type& value)
    {
        while (count--)
        {
            insert(pos, value);
        }
        return pos.m_node;
    }
    
    /**
    @brief Inserts elements
    inserts elements from range [first, last) before pos.
    @note The behavior is undefined if first and last are iterators into *this.
    @param pos Iterator after which the content will be inserted
    @param first, last The range of elements to insert
    @result Iterator to the last element inserted, or pos if first==last
    */
    template< class InputIt >
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        while (first != last)
        {
            insert(pos, *first);
            ++first;
        }
        return pos.m_node;
    }
    
    /**
    @brief Inserts elements
    Inserts elements from initializer list init before pos.
    @param pos Iterator after which the content will be inserted
    @param args Arguments to forward to the constructor of the element
    @result Iterator to the last element inserted.
    */
    constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> init)
    {
        return insert(pos, init.begin(), init.end());
    }

    /**
    @brief Erases elements
    Removes the element following pos.
    @param pos iterator to the element to remove
    @result Iterator to the element following the erased one, or end() if no such element exists.
    */
    constexpr iterator erase(const_iterator pos)
    {
        ++pos;
        deleteNode(static_cast<Node*>(pos.m_node->m_prev));
        return pos.m_node;
    }

    /**
    @brief Erases elements
    Removes the elements in the range [first, last).
    @param first, last range of elements to remove
    @result last
    */
    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        while (first != last)
        {
            first = erase(first);
        }
        return last.m_node;
    }

    /**
    @brief Clears the contents
    Erases all elements from the container. After this call, size() returns zero.
    Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-end iterator remains valid.
    */
    CXX14_CONSTEXPR void clear()
    {
        NodeBase* node = m_front.m_next;
        while (&m_back != node)
        {
            NodeBase* next = node->m_next;
            deleteNode(static_cast<Node*>(node));
            node = next;
        }
    }

    /**
    @brief removes elements satisfying specific criteria
    Removes all elements that are equal to value.
    @param value value of the elements to remove
    @result The number of elements removed
    */
    constexpr size_type remove(const value_type& value)
    {
        size_type nofRemovedElems = 0;
        const_iterator itCurrent = cbegin();
        const_iterator itEnd = cend();
        while (itCurrent != itEnd)
        {
            if (value == *itCurrent)
            {
                itCurrent = erase(itCurrent);
                ++nofRemovedElems;
            }
            else
            {
                ++itCurrent;
            }
        }
        
        return nofRemovedElems;
    }
    

    /**
    @brief removes elements satisfying specific criteriaRemoves all elements for which predicate pred returns true.
    Removes all elements that are equal to value.
    @param pred unary predicate which returns ​true if the element should be removed.
    The expression pred(v) must be convertible to bool for every argument v of type (possibly const) T, regardless of value category, and must not modify v.
    @result The number of elements removed
    */
    template<typename Predicate>
    constexpr size_type remove_if(Predicate pred)
    {
        size_type nofRemovedElems = 0;
        const_iterator itCurrent = cbegin();
        const_iterator itEnd = cend();
        while (itCurrent != itEnd)
        {
            if (pred(*itCurrent))
            {
                itCurrent = erase(itCurrent);
                ++nofRemovedElems;
            }
            else
            {
                ++itCurrent;
            }
        }
        
        return nofRemovedElems;
    }
    
    /**
    @brief reverses the order of the elements
    Reverses the order of the elements in the container. No references or iterators become invalidated.
    */
    CXX14_CONSTEXPR void reverse()
    {
        NodeBase* node = m_front.m_next;
        while (node != &m_back)
        {
            swap(node->m_prev, node->m_next);
            node = node->m_prev;
        }
        swap(m_front.m_next, m_back.m_prev);
        link(m_front, *m_front.m_next);
        link(*m_back.m_prev, m_back);
    }
    
    private:
    
    // StaticList node base class
    struct NodeBase
    {
        CXX14_CONSTEXPR NodeBase(NodeBase* prev = nullptr, NodeBase * next = nullptr) : m_prev(prev), m_next(next)
        {
            if (nullptr != prev)
            {
                prev->m_next = this;
            }
            
            if (nullptr != next)
            {
                next->m_prev = this;
            }
        }
        
        CXX20_CONSTEXPR ~NodeBase()
        {
            if (nullptr != m_prev)
            {
                m_prev->m_next = m_next;
            }
            
            if (nullptr != m_next)
            {
                m_next->m_prev = m_prev;
            }
        }
        
        NodeBase* m_prev = nullptr;
        NodeBase* m_next = nullptr;
    };

    // StaticList node class
    struct Node : public NodeBase
    {
        constexpr Node(NodeBase* prev, NodeBase* next, const value_type& data) : NodeBase(prev, next), m_data(data)
        {}
        
        constexpr Node(NodeBase* prev, NodeBase* next, value_type&& data) : NodeBase(prev, next), m_data(forward<value_type>(data))
        {}
        
        template <typename ... Args>
        constexpr Node(NodeBase* prev, NodeBase* next, Args&&... args) : NodeBase(prev, next), m_data(forward<Args...>(args...))
        {}

        constexpr Node(NodeBase* prev, NodeBase* next) : NodeBase(prev, next)
        {}
        
        value_type m_data;
    };
    
    CXX14_CONSTEXPR void init()
    {
        // Link front and back node
        link(m_front, m_back);
        
        // set up an internal available-list allocator
        for (size_t cnt = 0; cnt < t_capacity; ++cnt)
        {
            NodeBase* node = reinterpret_cast<NodeBase*>(&m_buffer[cnt][0]);
            node->m_next = m_available;
            m_available = node;
        }        
    }
    
    constexpr static void link(NodeBase& prev, NodeBase& next)
    {
        // Link two nodes of the list
        prev.m_next = &next;
        next.m_prev = &prev;
    }

    constexpr void deleteNode(Node* node)
    {
        // Destruct the node and its content
        node->~Node();
        
        // Attach node to available-list
        node->m_next = m_available;
        m_available = node;
    }
    
    constexpr void* allocateNode()
    {
        // Check available-list
        NodeBase * available = m_available;
        if (nullptr == available)
        {
            throw_bad_alloc();
        }
        
        // Detach node from available-list
        m_available = available->m_next;
        return available;
    }

    NodeBase m_front;
    NodeBase m_back;
    
    uint8_t m_buffer[t_capacity][sizeof(Node)];
    NodeBase* m_available = nullptr;
};

#endif