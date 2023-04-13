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

#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include <bits/c++config.h>
#include <bits/move.h>
#include <bits/new.h>
#include <exception.h>
#include <initializer_list>
#include <allocator.h>
#include <stdbool.h>

/**
@brief Template class implementing a queue of objects
@tparam T Type of queue elements
@tparam An allocator that is used to acquire/release memory and to construct/destroy the elements in that
*/
template <typename T, typename Allocator = HeapAllocator<>>
class ForwardList
{    
    class NodeBase;
    class Node;
    
    public:
     
    template <typename U>
    class Iterator;
   
    using value_type             = T;
    using allocator_type         = Allocator;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = typename Allocator::size_type;
    using difference_type        = ptrdiff_t;
    using iterator               = Iterator<value_type>;
    using const_iterator         = Iterator<const value_type>;

    /// Forward list iterator
    template <typename U>
    class Iterator
    {
        friend class ForwardList<T, Allocator>;

        public:
        
        /// Constructor
        CXX20_CONSTEXPR Iterator(NodeBase* node) : m_node(node)
        {}
        
        /// Constructor converting iterator to const_iterator
        CXX20_CONSTEXPR Iterator(const Iterator<typename remove_const<U>::type>& other) : Iterator(other.m_node)
        {}

        /// Destructor            
        CXX20_CONSTEXPR ~Iterator() = default;
        
        /// Increment operator
        CXX14_CONSTEXPR Iterator& operator++()
        {
            if (nullptr != m_node)
            {
                m_node = m_node->m_next;
            }
            return *this;
        }

        /// Dereference operator
        constexpr U& operator*() const
        {
            if (nullptr == m_node)
            {
                throw_nullptr_error();
            }
            return static_cast<Node*>(m_node)->m_data;
        }

        /// Inequality operator
        constexpr bool operator!=(const Iterator& other)
        {
            return m_node != other.m_node;
        }

        private:
        
        NodeBase* m_node = nullptr;
    };

    /**
    @brief constructs the ForwardList
    Constructs an empty container with the given allocator allocator
    @param allocator allocator to use for all memory allocations of this container
    */
    constexpr explicit ForwardList(const Allocator& allocator = Allocator()) : m_allocator(allocator)
    {}
    
    /**
    @brief constructs the ForwardList
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    @param value the value to initialize elements of the container with
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX14_CONSTEXPR ForwardList(size_type count, const value_type& value, const Allocator& allocator = Allocator()) : m_allocator(allocator)
    {
        resize(count, value);
    }
    
    /**
    @brief constructs the ForwardList
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param count the size of the container
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX14_CONSTEXPR ForwardList(size_type count, const Allocator& allocator = Allocator()) : m_allocator(allocator)
    {
        resize(count);
    }
    
    /**
    @brief constructs the ForwardList
    Constructs the container with the contents of the range [first, last).
    @param first, last the range to copy the elements from
    @param allocator allocator to use for all memory allocations of this container
    */
    template< class InputIt >
    CXX14_CONSTEXPR ForwardList(InputIt first, InputIt last, const Allocator& allocator = Allocator()) : m_allocator(allocator)
    {
        insertAfter(cbeforeBegin(), first, last);
    }
    
    /**
    @brief constructs the ForwardList
    Copy constructor. Constructs the container with the copy of the contents of other.
    @param other another container to be used as source to initialize the elements of the container with
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX14_CONSTEXPR ForwardList(const ForwardList& other, const Allocator& allocator = Allocator()) : ForwardList(other.begin(), other.end(), allocator)
    {}
    
    /**
    @brief constructs the ForwardList
    Constructs the container with count default-inserted instances of value_type. No copies are made.
    @param init initializer list to initialize the elements of the container with
    @param allocator allocator to use for all memory allocations of this container
    */
    CXX14_CONSTEXPR ForwardList(std::initializer_list<value_type> init, const Allocator& allocator = Allocator()) : ForwardList(init.begin(), init.end(), allocator)
    {}
    
    /**
    @brief assigns values to the container
    Move constructor. Constructs the container with the contents of other using move semantics. Allocator is obtained from the allocator belonging to other.
    @param other another container to be used as source to initialize the elements of the container with
    */
    CXX14_CONSTEXPR ForwardList(ForwardList&& other) : m_allocator(other.m_allocator)
    {
        swap(m_head.m_next, other.m_head.m_next);
    }
    
    /**
    @brief destructs the ForwardList
    Destructs the ForwardList. The destructors of the elements are called and the used storage is deallocated.
    Note, that if the elements are pointers, the pointed-to objects are not destroyed.
    */
    CXX20_CONSTEXPR ~ForwardList()
    {
        clear();
    }

    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param other another container to use as data source
    */
    CXX14_CONSTEXPR ForwardList& operator=(const ForwardList& other)
    {
        assign(other.begin(), other.end());
        return *this;
    }

    /**
    @brief assigns values to the container
    Move assignment operator. Replaces the contents with those of other using move semantics (i.e. the data in other is moved from other into this container).
    other is in a valid but unspecified state afterwards.
    @param other another container to use as data source
    */
    constexpr ForwardList& operator=(ForwardList&& other)
    {
        // Check if other uses the same allocator object
        if (m_allocator == other.m_allocator)
        {
            // other uses the same allocator object --> head nodes can simply be swapped
            swap(m_head.m_next, other.m_head.m_next);
        }
        else
        {
            // TODO use move semantic instead of copying the node data
            assign(other.begin(), other.end());
        }
        return *this;
    }

    /**
    @brief assigns values to the container
    Copy assignment operator. Replaces the contents with a copy of the contents of other.
    @param init initializer list to use as data source
    */
    CXX14_CONSTEXPR ForwardList& operator=(std::initializer_list<value_type> init)
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
        iterator dstPrev = beforeBegin();
        iterator dstCurrent = begin();
        const iterator dstEnd = end();
        
        while (dstCurrent != dstEnd && count > 0)
        {
            *dstCurrent = value;
            ++dstPrev;
            ++dstCurrent;
            --count;
        }
        
        eraseAfter(dstPrev, cend());
        insertAfter(dstPrev, count, value);
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
        iterator dstPrev = beforeBegin();
        iterator dstCurrent = begin();
        const iterator dstEnd = end();
        
        while (dstCurrent != dstEnd && first != last)
        {
            *dstCurrent = *first;
            ++dstPrev;
            ++dstCurrent;
            ++first;
        }
        
        eraseAfter(dstPrev, cend());
        insertAfter(dstPrev, first, last);
    }
    
    /**
    @brief assigns values to the container
    Replaces the contents with copies of those in the range [first, last).
    The behavior is undefined if either argument is an iterator into *this.
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
    CXX14_CONSTEXPR value_type& front()
    {
        // Get head node (no matter if it's NULL)
        return static_cast<Node*>(m_head.m_next)->m_data;
    }
    
    /**
    @brief access the first element
    Returns a reference to the first element in the container.
    Calling front on an empty container causes undefined behavior.
    @result reference to the first element
    */
    CXX14_CONSTEXPR const value_type& front() const
    {
        // Get head node (no matter if it's NULL)
        return static_cast<Node*>(m_head.m_next)->m_data;
    }
    
    /**
    @brief returns the associated allocator
    Returns the allocator associated with the container.
    @result The associated allocator.
    */
    constexpr const allocator_type& getAllocator() const
    {
        return m_allocator;
    }

    /**
    @brief Returns an iterator to the element before beginning
    Returns an iterator to the element before the first element of the container.
    This element acts as a placeholder, attempting to access it results in undefined behavior.
    The only usage cases are in functions insertAfter(), emplaceAfter(), eraseAfter(), spliceAfter() and the increment operator:
    incrementing the before-begin iterator gives exactly the same iterator as obtained from begin()/cbegin().
    @result Iterator to the element before the first element.
    */
    CXX14_CONSTEXPR const_iterator cbeforeBegin() const
    {
        return const_cast<NodeBase*>(&m_head);
    }
    
    /**
    @brief Returns an iterator to the element before beginning
    Returns an iterator to the element before the first element of the container.
    This element acts as a placeholder, attempting to access it results in undefined behavior.
    The only usage cases are in functions insertAfter(), emplaceAfter(), eraseAfter(), spliceAfter() and the increment operator:
    incrementing the before-begin iterator gives exactly the same iterator as obtained from begin()/cbegin().
    @result Iterator to the element before the first element.
    */
    CXX14_CONSTEXPR const_iterator beforeBegin() const
    {
        return const_cast<NodeBase*>(&m_head);
    }
    
    /**
    @brief Returns an iterator to the element before beginning
    Returns an iterator to the element before the first element of the container.
    This element acts as a placeholder, attempting to access it results in undefined behavior.
    The only usage cases are in functions insertAfter(), emplaceAfter(), eraseAfter(), spliceAfter() and the increment operator:
    incrementing the before-begin iterator gives exactly the same iterator as obtained from begin()/cbegin().
    @result Iterator to the element before the first element.
    */
    CXX14_CONSTEXPR iterator beforeBegin()
    {
        return &m_head;
    }
    
    /**
    @brief Returns an iterator to the beginning
    Returns an iterator to the first element of the ForwardList.
    If the ForwardList is empty, the returned iterator will be equal to end().
    @result Iterator to the first element.
    */
    CXX14_CONSTEXPR const_iterator cbegin() const
    {
        return m_head.m_next;
    }
    
    /**
    @brief Returns an iterator to the beginning
    Returns an iterator to the first element of the ForwardList.
    If the ForwardList is empty, the returned iterator will be equal to end().
    @result Iterator to the first element.
    */
    CXX14_CONSTEXPR const_iterator begin() const
    {
        return m_head.m_next;
    }
    
    /**
    @brief Returns an iterator to the beginning
    Returns an iterator to the first element of the ForwardList.
    If the ForwardList is empty, the returned iterator will be equal to end().
    @result Iterator to the first element.
    */
    CXX14_CONSTEXPR iterator begin()
    {
        return m_head.m_next;
    }
    
    /**
    @brief Returns an iterator to the end
    Returns an iterator to the element following the last element of the ForwardList.
    This element acts as a placeholder; attempting to access it results in undefined behavior.
    @result Iterator to the element following the last element.
    */
    CXX14_CONSTEXPR const_iterator cend() const
    {
        return nullptr;
    }
    
    /**
    @brief Returns an iterator to the end
    Returns an iterator to the element following the last element of the ForwardList.
    This element acts as a placeholder; attempting to access it results in undefined behavior.
    @result Iterator to the element following the last element.
    */
    CXX14_CONSTEXPR const_iterator end() const
    {
        return nullptr;
    }
    
    /**
    @brief Returns an iterator to the end
    Returns an iterator to the element following the last element of the ForwardList.
    This element acts as a placeholder; attempting to access it results in undefined behavior.
    @result Iterator to the element following the last element.
    */
    CXX14_CONSTEXPR iterator end()
    {
        return nullptr;
    }

    /**
    @brief Checks whether the container is empty
    Checks if the container has no elements, i.e. whether begin() == end().
    @result true if the container is empty, false otherwise
    */
    constexpr bool empty() const
    {
        return (nullptr == m_head.m_next);
    }
    
    /**
    @brief Clears the contents
    Erases all elements from the container.
    Invalidates any references, pointers, or iterators referring to contained elements. Any past-the-end iterator remains valid.
    */
    CXX14_CONSTEXPR void clear()
    {
        while (!empty())
        {
            Node * node = static_cast<Node*>(m_head.m_next);
            m_head.m_next = deleteNode(node);
        }
    }
    
    /**
    @brief Inserts elements after an element
    inserts value after the element pointed to by pos
    @param pos Iterator after which the content will be inserted
    @param value Element value to insert
    @result Iterator to the inserted element.
    */
    constexpr iterator insertAfter(const_iterator pos, const value_type& value)
    {
        return pos.m_node->m_next = new (allocateNode()) Node(pos.m_node->m_next, value);
    }

    /**
    @brief Inserts elements after an element
    inserts value after the element pointed to by pos
    @param pos Iterator after which the content will be inserted
    @param value Element value to insert
    @result Iterator to the inserted element.
    */
    constexpr iterator insertAfter(const_iterator pos, value_type&& value)
    {
        return pos.m_node->m_next = new (allocateNode()) Node(pos.m_node->m_next, forward<value_type>(value));
    }
    
    /**
    @brief Inserts elements after an element
    inserts count copies of the value after the element pointed to by pos
    @param pos Iterator after which the content will be inserted
    @param value Element value to insert
    @param count number of copies to insert
    @result Iterator to the last element inserted, or pos if count==0
    */
    constexpr iterator insertAfter(const_iterator pos, size_type count, const value_type& value)
    {
        while (count--)
        {
            insertAfter(pos, value);
            ++pos;
        }
        return pos.m_node;
    }
    
    /**
    @brief Inserts elements after an element
    inserts elements from range [first, last) after the element pointed to by pos.
    @note The behavior is undefined if first and last are iterators into *this.
    @param pos Iterator after which the content will be inserted
    @param first, last The range of elements to insert
    @result Iterator to the last element inserted, or pos if first==last
    */
    template< class InputIt >
    constexpr iterator insertAfter(const_iterator pos, InputIt first, InputIt last)
    {
        while (first != last)
        {
            insertAfter(pos, *first);
            ++pos;
            ++first;
        }        
        return pos.m_node;
    }
        
    /**
    @brief Inserts elements after an element
    Inserts a new element into a position after the specified position in the container.
    The element is constructed in-place, i.e. no copy or move operations are performed. The constructor of the element is called with exactly the same arguments, as supplied to the function.
    @param pos Iterator after which the content will be inserted
    @param args Arguments to forward to the constructor of the element
    @result Iterator to the new element.
    */
    constexpr iterator insertAfter(const_iterator pos, std::initializer_list<value_type> ilist)
    {
        return insertAfter(pos, ilist.begin(), ilist.end());
    }
    
    /**
    @brief Inserts elements after an element
    Inserts a new element into a position after the specified position in the container.
    The element is constructed in-place, i.e. no copy or move operations are performed. The constructor of the element is called with exactly the same arguments, as supplied to the function.
    @param pos Iterator after which the content will be inserted
    @param args Arguments to forward to the constructor of the element
    @result Iterator to the new element.
    */
    template <typename ... Args>
    constexpr iterator emplaceAfter(const_iterator pos, Args&&... args)
    {
        return iterator(pos.m_node->m_next = new (allocateNode()) Node(pos.m_node->m_next, forward<Args...>(args...)));
    }

    /**
    @brief Erase an element after an element
    Removes specified elements from the container. Removes the element following pos.
    @param pos iterator to the element preceding the element to remove
    @result Iterator to the element following the erased one, or end() if no such element exists.
    */
    constexpr iterator eraseAfter(const_iterator pos)
    {
        Node* node = static_cast<Node*>(pos.m_node->m_next);
        if (nullptr != node)
        {
            node = deleteNode(node);
            pos.m_node->m_next = node;
        }
        return node;
    }    

    /**
    @brief Erase an element after an element
    Removes specified elements from the container. Removes the elements following first until last.
    @param first, last range of elements to remove
    @result last
    */
    constexpr iterator eraseAfter(const_iterator first, const_iterator last)
    {
        Node* node = static_cast<Node*>(first.m_node->m_next);
        while (nullptr != node && last.m_node != node)
        {
            node = deleteNode(node);
        }
        first.m_node->m_next = node;
        return node;
    }

    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    No iterators or references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(const value_type& value)
    {
        m_head.m_next = new (allocateNode()) Node(m_head.m_next, value);
    }

    /**
    @brief Inserts an element to the beginning
    Prepends the given element value to the beginning of the container.
    No iterators or references are invalidated.
    @param value the value of the element to prepend
    */
    CXX14_CONSTEXPR void pushFront(value_type&& data)
    {
        m_head.m_next = new (allocateNode()) Node(m_head.m_next, forward<value_type>(data));
    }

    /**
    @brief Constructs an element in-place at the beginning
    Inserts a new element to the beginning of the container. The element is constructed through std::allocator_traits::construct, which typically uses placement-new to construct the element in-place at the location provided by the container. The arguments args... are forwarded to the constructor as std::forward<Args>(args)....
    No iterators or references are invalidated.
    @param args arguments to forward to the constructor of the element
    */
    template<typename ... Args>
    CXX14_CONSTEXPR void emplaceFront(Args&&... args)
    {
        m_head.m_next = new (allocateNode()) Node(m_head.m_next, forward<Args...>(args...));
    }
    
    /**
    @brief Removes the first element
    Removes the first element of the container. If there are no elements in the container, the behavior is undefined.
    References and iterators to the erased element are invalidated.
    */
    CXX14_CONSTEXPR void popFront()
    {
        if (!empty())
        {
            Node * node = static_cast<Node*>(m_head.m_next);
            m_head.m_next = deleteNode(node);
        }
    }
    
    /**
    @brief Changes the number of elements stored
    Resizes the container to contain count elements.
    If the current size is greater than count, the container is reduced to its first count elements.
    If the current size is less than count, additional default-inserted elements are appended
    @param count new size of the container
    */
    constexpr void resize(size_type count)
    {
        // Rescue up to *count* nodes
        const_iterator pos = cbeforeBegin();
        while (pos.m_node->m_next != nullptr && count > 0)
        {
            ++pos;
            --count;
        }
        
        // Delete remaining elements
        eraseAfter(pos, cend());

        // Insert missing elements
        while (count--)
        {
            emplaceAfter(pos);
        }
    }

    /**
    @brief Changes the number of elements stored
    Resizes the container to contain count elements.
    If the current size is greater than count, the container is reduced to its first count elements.
    If the current size is less than count, additional copies of value are appended.
    @param count new size of the container
    @param value the value to initialize the new elements with
    */
    constexpr void resize(size_type count, const value_type& value)
    {
        // Rescue up to *count* nodes
        const_iterator pos = cbeforeBegin();
        while (pos.m_node->m_next != nullptr && count > 0)
        {
            ++pos;
            --count;
        }
        
        // Delete remaining elements
        eraseAfter(pos, cend());

        // Insert missing elements
        insertAfter(pos, count, value);
    }
        
    /**
    @brief Moves elements from another ForwardList to *this.
    No elements are copied. pos must be either a dereferenceable valid iterator into *this or the beforeBegin() iterator (in particular, end() is not a valid argument for pos).
    The behavior is undefined if getAllocator() != other.getAllocator(). No iterators or references become invalidated, the iterators to the moved elements now refer into *this, not into other.
    Moves all elements from other into *this. The elements are inserted after the element pointed to by pos.
    The container other becomes empty after the operation. The behavior is undefined if other refers to the same object as *this.
    @param pos element after which the content will be inserted
    @param other another container to move the content from
    */
    constexpr void spliceAfter(const_iterator pos, ForwardList& other)
    {
        spliceAfter(pos, forward<ForwardList>(other));
    }        
        
    /**
    @brief Moves elements from another ForwardList to *this.
    No elements are copied. pos must be either a dereferenceable valid iterator into *this or the beforeBegin() iterator (in particular, end() is not a valid argument for pos).
    The behavior is undefined if getAllocator() != other.getAllocator(). No iterators or references become invalidated, the iterators to the moved elements now refer into *this, not into other.
    Moves all elements from other into *this. The elements are inserted after the element pointed to by pos.
    The container other becomes empty after the operation. The behavior is undefined if other refers to the same object as *this.
    @param pos element after which the content will be inserted
    @param other another container to move the content from
    */
    constexpr void spliceAfter(const_iterator pos, ForwardList&& other)
    {
        // Check if pos is a valid list node
        NodeBase* node = pos.m_node;
        if (nullptr == node)
        {
            throw_nullptr_error();
        }

        // Append other to pos node and store the original successor of pos        
        NodeBase* next = node->m_next;
        node->m_next = other.m_head.m_next;        

        // Uncouple moved list nodes from other
        other.m_head.m_next = nullptr;
         
        // Append remaining nodes of this to other
        while (nullptr != node->m_next)
        {
            node = node->m_next;
        }
        node->m_next = next;
    }
        
    private:
    
    // Forward list node base class
    struct NodeBase
    {
        constexpr NodeBase(NodeBase * next = nullptr) : m_next(next)
        {}
        
        NodeBase* m_next = nullptr;
    };

    // Forward list node class
    struct Node : public NodeBase
    {
        constexpr Node(NodeBase* next, const value_type& data) : NodeBase(next), m_data(data)
        {}
        
        constexpr Node(NodeBase* next, value_type&& data) : NodeBase(next), m_data(forward<value_type>(data))
        {}
        
        template <typename ... Args>
        constexpr Node(NodeBase* next, Args&&... args) : NodeBase(next), m_data(forward<Args...>(args...))
        {}

        constexpr Node(NodeBase* next) : NodeBase(next)
        {}
        
        value_type m_data;
    };

    // Overload of emplaceAfter() for default-constructed inserts
    constexpr iterator emplaceAfter(const_iterator pos)
    {
        return pos.m_node->m_next = new (allocateNode()) Node(pos.m_node->m_next);
    }
    
    constexpr Node* deleteNode(Node* node)
    {
        Node* next = static_cast<Node*>(node->m_next);
        node->~Node();
        m_allocator.deallocate(node);
        return next;
    }
    
    constexpr void* allocateNode()
    {
        void * ptr = m_allocator.allocate(sizeof(Node));
        //if (nullptr == ptr)
        //{
            //throw_bad_alloc();
        //}
        return ptr;
    }

    NodeBase m_head;
    Allocator m_allocator;
};

#endif