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

#ifndef VECTOR_SORTED_H
#define VECTOR_SORTED_H

#include "Vector.h"
#include <stdbool.h>

enum class SortOrder
{
    ASCENDING,
    DESCENDING
} ;

/**
@brief Helper class for greater/less than comparison
@tparam T Working type
@tparam t_sortOrder Sort order which yields a true comparison result of two elements a,b
*/
template <typename T, SortOrder t_sortOrder>
struct Comparison;

template <typename T>
struct Comparison<T, SortOrder::ASCENDING>
{
    static constexpr bool compare(const T & a, const T & b)
    {
        return a < b;
    }
};

template <typename T>
struct Comparison<T, SortOrder::DESCENDING>
{
    static constexpr bool compare(const T & a, const T & b)
    {
        return a > b;
    }
};

/**
@brief Vector container class with runtime length but compile-time capacity (i.e. maximum length)
Elements are sorted on insertion.
@tparam Elem Type of vector elements
@tparam Len Type of vector length
@tparam t_capacity Vector capacity in elements
*/
template <typename Elem, typename Len, Len t_capacity, SortOrder t_sortOrder>
class VectorSorted : public Vector<Elem, Len, t_capacity>
{
    public:
    
    // Make necessary base class methods public
    using Vector<Elem, Len, t_capacity>::length;
    using Vector<Elem, Len, t_capacity>::operator[];
    using Vector<Elem, Len, t_capacity>::begin;
    using Vector<Elem, Len, t_capacity>::end;
    using Vector<Elem, Len, t_capacity>::clear;
    using Vector<Elem, Len, t_capacity>::isEmpty;
    using Vector<Elem, Len, t_capacity>::isFull;
    
    /**
    @brief Insertion of an element preserving the sort order of the vector
    @param elem Element to be inserted
    @result Flag indicating if insertion of the element was successful
    */
    constexpr bool insert(const Elem elem)
    {
        if (isFull())
        {
            return false;
        }
        
        if (isEmpty())
        {
            // Increase size by one element
            Vector<Elem, Len, t_capacity>::m_len++;
            
            // Insert the element
            Vector<Elem, Len, t_capacity>::m_data[0] = elem;
        }
        else
        {
            // Find position to insert the new element
            Elem * pos = begin();
            Elem * last = end();
            for (; pos < last; ++pos)
            {
                if (Comparison<Elem, t_sortOrder>::compare(elem, *pos))
                {
                    // All elements pos .. last have to move up by one position, so the new element can be inserted
                    // Start with last noticing last now points to the actual last element
                    // TODO optimize???
                    for (; last > pos; --last)
                    {
                        *last = *(last-1);
                    }
                    
                    break;
                }
            }

            // Increase size by one element
            Vector<Elem, Len, t_capacity>::m_len++;
            
            // Insert the element
            *pos = elem;
        }
        
        return true;
    }

    /**
    @brief Removal of an element(s)
    @param elem Element to be removed. All instances of this element are removed from the vector
    @result Flag indicating if at least one instance of the element has been removed successfully
    */
    constexpr bool remove(const Elem elem)
    {
        // Find position of the element
        Elem * pos = begin();
        Elem * last = end();
        for (; pos < last; ++pos)
        {
            if (*pos == elem)
            {
                // Element value has been found, now count the number of occurrences
                Len nofElems = 0;
                for (; (pos < last) && (*pos == elem); ++pos, ++nofElems);
                
                // Move remaining elements forwards by the number of elements to be removed
                for (; pos < last; ++pos)
                {
                    *(pos-nofElems) = *pos;
                }

                // Decrease size by number of found elements
                Vector<Elem, Len, t_capacity>::m_len -= nofElems;
                
                return true;
            }
        }
        
        return false;
    }
};

#endif