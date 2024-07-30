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

#ifndef TYPE_MAP_H
#define TYPE_MAP_H

#include <bits/c++config.h>
#include <type_traits.h>
#include <stddef.h>

/**
@brief Element of a type map
This class represents a (used) element of a type map. An integer index is associated with a type.
@tparam t_index zero-based index of the element inside the type map
@tparam Type Data type assoiciated with t_index
*/
template <size_t t_index, typename Type>
struct TypeMapElem
{
    static constexpr size_t index = t_index;
    using type = Type;
};

/**
@brief Dispatcher for a type map element
This class represents the actual type associated to a given index.
If the index is assigned by a type map element, the resulting type will be the type specified by the (used) element,
otherwise the resulting type will be the default type of the type map
@tparam t_index zero-based type map index
@tparam Default Type used if t_index is not used by any element
@tparam Element Dispatched type map element
@tparam OtherElements All other elements to dispatch
*/
template <size_t t_index, typename Default, typename Element, typename... OtherElements>
struct TypeMapElemDispatcher
{
    using type = conditional_t<t_index == Element::index, typename Element::type, typename TypeMapElemDispatcher<t_index, Default, OtherElements...>::type>;
};

/**
@brief Dispatcher for a type map element. Template specialization for end of recursion
This class represents the actual type associated to a given index.
If the index is assigned by a type map element, the resulting type will be the type specified by the (used) element,
otherwise the resulting type will be the default type of the type map
@tparam t_index zero-based type map index
@tparam Default Type used if t_index is not used by any element
@tparam Element Dispatched type map element
*/
template <size_t t_index, typename Default, typename Element>
struct TypeMapElemDispatcher<t_index, Default, Element>
{
    using type = conditional_t<t_index == Element::index, typename Element::type, Default>;
};

/**
@brief Type pack
Variadic packs cannot be used as a "naked" type. This helper class represents a type pack.
@tparam Type Variadic type pack
*/
template <typename... Types>
struct TypePack
{};

/**
@brief Type map
This class represents a type map of given length. For each index, the type is either set to the type specified by the used element or set to a default type.
@tparam t_length type map length
@tparam Default Type used if t_index is not used by any element
@tparam UsedElements Used elements specified as variadic pack of specialized TypeMapElem classes
*/
template <size_t t_length, typename Default, typename... UsedElements>
class TypeMap
{
    private:
    
    // The resulting type pack consists of a previous (ie. shorter) type pack and current type pack (which consists of only one type)
    using PrevTypePack = typename TypeMap<t_length-1, Default, UsedElements...>::type;
    using CurrTypePack = TypePack<typename TypeMapElemDispatcher<t_length-1, Default, UsedElements...>::type>;
    
    // Use decltype with this function declaration to compose the type pack from individual types
    template <typename... Types1, typename... Types2>
    static TypePack<Types1..., Types2...> ConcatenateTypePacks(const TypePack<Types1...>&, const TypePack<Types2...>&);

    public:
    
    // This is the resulting type pack
    using type = decltype(ConcatenateTypePacks(PrevTypePack(), CurrTypePack()));

    // This the type at given index in the map
    template <size_t t_index>
    using IndexToTypeT = TypeMapElemDispatcher<t_index, Default, UsedElements...>::type;
};
    
/**
@brief Type map. Template specialization for end of recursion
@tparam Default Type used if t_index is not used by any element
@tparam UsedElements Used elements specified as variadic pack of specialized TypeMapElem classes
*/
template <typename Default, typename... UsedElements>
class TypeMap <1, Default, UsedElements...>
{
    public:
    
    using type = TypePack<typename TypeMapElemDispatcher<0, Default, UsedElements...>::type>;

    template <size_t t_index>
    using IndexToTypeT = TypeMapElemDispatcher<t_index, Default, UsedElements...>::type;
};

/**
@brief Type map. Template specialization for empty type maps
@tparam Default Type used if t_index is not used by any element
@tparam UsedElements Used elements specified as variadic pack of specialized TypeMapElem classes
*/
template <typename Default, typename... UsedElements>
class TypeMap <0, Default, UsedElements...>
{};

#endif