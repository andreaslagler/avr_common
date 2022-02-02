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

#ifndef LIST_NODE_H
#define LIST_NODE_H

/**
@brief Template class implementing a node to be used with linked list containers like Queue or Stack.
@tparam Elem Type of queue elements
@note Because of the missing support of std::new in avr-gcc, this node implementation is not self-contained and does not store copies of the node elements. SingleLinkedNode<Elem> has to be a public base class of Elem.
*/
template <typename Elem>
class SingleLinkedNode
{
    public:

    /// @brief  Constructor
    constexpr SingleLinkedNode() = default;

    /**
    @brief Iterate to next node
    @result Pointer to next node attached to this node
    @note If no node is attached to this node, nullptr is returned
    */
    constexpr SingleLinkedNode<Elem> * next() const
    {
        return m_next;
    }

    /**
    @brief Append a node
    @param node Node to be attached to this node
    */
    constexpr void append(SingleLinkedNode<Elem> * node)
    {
        m_next = node;
    }
    
    /**
    @brief Cast to element type (this is always possible when CRTP pattern is used, i.e. Elem derives from SingleLinkedNode<Elem>)
    */
    constexpr operator Elem & ()
    {
        return *static_cast<Elem*>(this);
    }

    /**
    @brief Cast to element type (this is always possible when CRTP pattern is used, i.e. Elem derives from SingleLinkedNode<Elem>)
    */
    constexpr operator const Elem & () const
    {
        return *static_cast<Elem*>(this);
    }
    
    private:

    // Pointer to next list node
    SingleLinkedNode<Elem> * m_next {nullptr};
};

#endif