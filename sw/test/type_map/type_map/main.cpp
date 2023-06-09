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

// Test for type map. Test is passed if this cpp file compiles
#include <type_map.h>
#include <variant.h>

template <typename T>
struct TypePackToVariant;

template <typename ... T>
struct TypePackToVariant<TypePack<T...>>
{
    using type = Variant<T...>;
};

struct Index
{
    enum
    {
    _1 = 0,
    _2,
    _3,
    _4,
    NOF_ENTRIES
    };
};

int main(void)
{
    // Specify the type pack based on of length 4:
    // 4th element (one-based) --> float
    // 1st element (one-based) --> double
    // Otherwise --> int
    using myTypePack = typename TypeMap<static_cast<size_t>(Index::NOF_ENTRIES), int,
    TypeMapElem<Index::_4, float>,
    TypeMapElem<Index::_1, double>
    >::type;
    
    // Create a variant using the type pack
    typename TypePackToVariant<myTypePack>::type v;
    
    // Make sure v is of the expected type
    static_assert(is_same_v<Variant<double, int, int, float>, decltype(v)>, "error");

    while (true)
    {
    }
}