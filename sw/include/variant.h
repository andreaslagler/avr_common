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

#ifndef VARIANT_H
#define VARIANT_H

#include <stddef.h>
#include <bits/c++config.h>
#include <bits/new.h>
#include <bits/move.h>
#include <exception.h>

#include <type_traits.h>
#include <utility.h>

#if SINCE_CXX17

namespace variantHelper
{
    // Maximum size of the types of a variadic types pack
    template <typename Type, typename... OtherTypes>
    struct MaxSize
    {
        static constexpr size_t value = (sizeof(Type) > MaxSize<OtherTypes...>::value) ? sizeof(Type) : MaxSize<OtherTypes...>::value;
    };

    template <typename Type>
    struct MaxSize<Type>
    {
        static constexpr size_t value = sizeof(Type);
    };

    // Alias for MaxSize<>::value
    template <typename... Types>
    inline constexpr size_t MaxSizeV = MaxSize<Types...>::value;

    // Assignment of a type to its zero-based index within a variadic types pack
    template <typename Type, typename FirstType, typename... OtherTypes>
    struct TypeToIndex
    {
        static constexpr size_t value = is_same_v<Type, FirstType> ? 1 : 1 + TypeToIndex<Type, OtherTypes...>::value;
    };

    template <typename Type, typename FirstType>
    struct TypeToIndex<Type, FirstType>
    {
        static constexpr size_t value = 1;
    };

    // Alias for TypeToIndex<>::value
    template <typename Type, typename... Types>
    inline constexpr size_t typeToIndexV = TypeToIndex<Type, Types...>::value - 1;


    // Get a type of a variadic types pack by its zero-based index
    template <size_t t_index, size_t t_count, typename FirstType, typename... OtherTypes>
    struct IndexToType
    {
        using Type = conditional_t<t_index == t_count, FirstType, typename IndexToType<t_index, t_count + 1, OtherTypes...>::Type>;
    };

    template <size_t t_index, size_t t_count, typename FirstType>
    struct IndexToType<t_index, t_count, FirstType>
    {
        using Type = FirstType;
    };

    // Alias for IndexToType<>::Type
    template <size_t t_index, typename... Types>
    using IndexToTypeT = typename IndexToType<t_index, 0, Types...>::Type;

    // Check if a type is part of variadic types pack
    template <typename Type, typename FirstType, typename... OtherTypes>
    struct IsPartOf
    {
        static constexpr bool value = is_same_v<Type, FirstType> || IsPartOf<Type, OtherTypes...>::value;
    };

    template <typename Type, typename FirstType>
    struct IsPartOf<Type, FirstType>
    {
        static constexpr bool value = is_same_v<Type, FirstType>;
    };

    // Alias for IsPartOf<>::value
    template <typename Type, typename... Types>
    inline constexpr bool isPartOfV = IsPartOf<Type, Types...>::value;
}

// Forward declarations
template <typename... Types>
class Variant;

template <size_t t_index, typename... Types>
constexpr auto get(Variant<Types...>& v) -> variantHelper::IndexToTypeT<t_index, Types...>&;

template <size_t t_index, typename... Types>
constexpr auto get(Variant<Types...>&& v) -> variantHelper::IndexToTypeT<t_index, Types...>&&;

template <size_t t_index, typename... Types>
constexpr auto get(const Variant<Types...>& v) -> const variantHelper::IndexToTypeT<t_index, Types...>&;

template <size_t t_index, typename... Types>
constexpr auto get(const Variant<Types...>&& v) -> variantHelper::IndexToTypeT<t_index, Types...>&&;

namespace variantHelper
{
    // Simple invocation function
    // This function can be extended for other callables as needed
    template<class Functor, class... Args>
    constexpr auto invoke(Functor&& f, Args&&... args)
    {
        return forward<Functor>(f)(forward<Args>(args)...);
    }

    // Dispatcher struct for variant visitor as described here https://mpark.github.io/programming/2019/01/22/variant-visitation-v2/
    template <bool t_isValid, typename ReturnType>
    struct dispatcher;

    template <typename ReturnType>
    struct dispatcher<true, ReturnType>
    {
        template <std::size_t I, typename F, typename V>
        static constexpr ReturnType case_(F &&f, V &&v)
        {
            // Make sure all overloads of the functor return the same type
            using ExpectedReturnType = ReturnType;
            using ActualReturnType = decltype(invoke(forward<F>(f), get<I>(forward<V>(v))));
            static_assert(is_same_v<ExpectedReturnType, ActualReturnType>, "visit() requires the visitor to have a single return type");
            
            // Invoke the functor on the alternative I of variant v
            return invoke(forward<F>(f), get<I>(forward<V>(v)));
        }

        template <size_t B, typename F, typename V>
        static constexpr ReturnType switch_(F &&f, V &&v)
        {
            constexpr size_t size = v.size();
            switch (v.index())
            {
                case B+0:
                return dispatcher<(B+0 < size), ReturnType>::template case_<B+0>(forward<F>(f), forward<V>(v));

                case B+1:
                return dispatcher<(B+1 < size), ReturnType>::template case_<B+1>(forward<F>(f), forward<V>(v));

                case B+2:
                return dispatcher<(B+2 < size), ReturnType>::template case_<B+2>(forward<F>(f), forward<V>(v));

                case B+3:
                return dispatcher<(B+3 < size), ReturnType>::template case_<B+3>(forward<F>(f), forward<V>(v));

                default:
                return dispatcher<(B+4 < size), ReturnType>::template switch_<B+4>(forward<F>(f), forward<V>(v));
            }
        }
    };

    // Invalid dispatcher is needed to terminate compile-time recursion
    template <typename ReturnType>
    struct dispatcher<false, ReturnType>
    {
        template <std::size_t I, typename F, typename V>
        static constexpr ReturnType case_(F &&, V &&)
        {
            __builtin_unreachable();
        }

        template <std::size_t B, typename F, typename V>
        static constexpr ReturnType switch_(F &&, V &&)
        {
            __builtin_unreachable();
        }
    };
}

/**
@brief Applies a functor to a variant
Calls the provided functor with the argument held by a variants
@tparam F functor type that accepts every possible alternative from the variant
@tparam V variant type
@param f functor to be applied to the variant
@param v variant object passed to the functor
@result The result of the invoke operation. The return type is the type obtained from applying decltype to the result.
*/
template <typename F, typename V>
constexpr decltype(auto) visit(F &&f, V &&v)
{
    using ReturnType = decltype(variantHelper::invoke(forward<F>(f), get<0>(forward<V>(v))));
    return variantHelper::dispatcher<true, ReturnType>::template switch_<0>(forward<F>(f), forward<V>(v));
}

/**
@brief constructs the variant object
The class template Variant represents a type-safe union.
An instance of Variant at any given time either holds a value of one of its alternative types, or in case of error - no value.
As with unions, if a variant holds a value of some object type T, the object representation of T is allocated directly within the object representation of the variant itself.
A variant is not permitted to hold references, arrays, or the type void. Empty variants are also ill-formed (Variant<Monostate> can be used instead).
A variant is permitted to hold the same type more than once, and to hold differently cv-qualified versions of the same type.
Consistent with the behavior of unions during aggregate initialization, a default-constructed variant holds a value of its first alternative, unless that alternative is not default-constructible (in which case the variant is not default-constructible either).
The helper class Monostate can be used to make such variants default-constructible.
@tparam Types the types that may be stored in this variant. All types must meet the Destructible requirements (in particular, array types and non-object types are not allowed).
*/
template <typename... Types>
class Variant
{
    private:

    // dispatcher needs access to size()
    template <bool t_isValid, typename R>
    friend struct variantHelper::dispatcher;

    // get functions need direct access to the data buffer via getPtr<>()
    template <size_t t_index, typename... ArgTypes>
    friend constexpr variantHelper::IndexToTypeT<t_index, ArgTypes...>& get(Variant<ArgTypes...>& v);

    template <size_t t_index, typename... ArgTypes>
    friend constexpr variantHelper::IndexToTypeT<t_index, ArgTypes...>&& get(Variant<ArgTypes...>&& v);

    template <size_t t_index, typename... ArgTypes>
    friend constexpr const variantHelper::IndexToTypeT<t_index, ArgTypes...>& get(const Variant<ArgTypes...>& v);

    template <size_t t_index, typename... ArgTypes>
    friend constexpr const variantHelper::IndexToTypeT<t_index, ArgTypes...>&& get(const Variant<ArgTypes...>&& v);
    
    // Class-specific aliases
    template <size_t t_index>
    using IndexToTypeT = variantHelper::IndexToTypeT<t_index, Types...>;

    template <typename Type>
    static constexpr size_t typeToIndexV = variantHelper::typeToIndexV<Type, Types...>;
    
    template <typename Type>
    static constexpr bool isPartOfV = variantHelper::isPartOfV<Type, Types...>;

    public:
    
    using size_type = size_t;
    
    /**
    @brief constructs the variant object
    Default constructor. Constructs a variant holding the value-initialized value of the first alternative (index() is zero).
    */
    constexpr Variant() : m_index(0)
    {
        new (getPtr<0>()) IndexToTypeT<0>;
    }
    
    /**
    @brief constructs the variant object
    Converting constructor. Constructs a variant holding the alternative type Type 
    */
    template <typename Type, typename... Args >
    constexpr explicit Variant(in_place_type_t<Type>, Args&&... args)
    {
        static_assert(isPartOfV<Type>, "Invalid type");
        new (getPtr<Type>()) Type(forward<Args>(args)...);
        m_index = typeToIndexV<Type>;
    }
    
    /**
    @brief constructs the variant object
    Converting constructor. Constructs a variant holding the alternative type Types[t_index]
    */
    template <size_type t_index, typename... Args >
    constexpr explicit Variant(in_place_index_t<t_index>, Args&&... args)
    {
        using Type = IndexToTypeT<t_index>;
        static_assert(isPartOfV<Type>, "Invalid type");
        new (getPtr<Type>()) Type(forward<Args>(args)...);
        m_index = t_index;
    }

    /**
    @brief constructs the variant object
    */
    constexpr Variant(const Variant& other) : m_index(other.m_index)
    {
        copyConstruct(other);
    }
    
    /**
    @brief constructs the variant object
    */
    constexpr Variant(Variant&& other) : m_index(other.m_index)
    {
        moveConstruct(forward<Variant>(other));
    }
    
    /**
    @brief destroys the variant, along with its contained value
    If variant object is valueless, does nothing. Otherwise, destroys the currently contained value.
    */
    CXX20_CONSTEXPR ~Variant()
    {
        if (m_index != static_cast<size_type>(-1))
        {
            destroy();
        }
    }
    
    /**
    @brief assigns a variant
    Copy assignment of other variant
    */
    constexpr Variant& operator=(const Variant& other)
    {
        if (other.m_index == m_index)
        {
            // This and other are holding the same alternative --> copy-assign value if not valueless
            if (m_index != static_cast<size_type>(-1))
            {
                copyAssign(other);
            }
        }
        else
        {
            // This and other are not holding the same alternative
            // Destroy value if this not valueless
            if (m_index != static_cast<size_type>(-1))
            {
                destroy();
            }
            
            m_index = other.m_index;
            
            // Copy-construct value if other is not valueless
            if (m_index != static_cast<size_type>(-1))
            {
                copyConstruct(other);
            }
        }
        return *this;
    }
    
    /**
    @brief assigns a variant
    Move assignment of other variant
    */
    constexpr Variant& operator=(Variant&& other)
    {
        if (other.m_index == m_index)
        {
            // This and other are holding the same alternative --> move-assign value if not valueless
            if (m_index != static_cast<size_type>(-1))
            {
                moveAssign(forward<Variant>(other));
            }
        }
        else
        {
            // This and other are not holding the same alternative
            // Destroy value if this not valueless
            if (m_index != static_cast<size_type>(-1))
            {
                destroy();
            }
            
            m_index = other.m_index;
            
            // Move-construct value if other is not valueless
            if (m_index != static_cast<size_type>(-1))
            {
                moveConstruct(forward<Variant>(other));
            }
        }
        return *this;
    }
    
    /**
    @brief assigns a variant
    Converting assignment.
    If variant already holds a value of alternative type Type, copy-assigns the contained value to value.
    If variant does not already hold a value of alternative type Type, copy-constructs the contained value from value.
    */
    template <typename Type>
    constexpr enable_if_t<isPartOfV<remove_cvref_t<Type>>, Variant&> operator=(const Type& value)
    {
        constexpr size_t newId = typeToIndexV<Type>;
        if (newId != m_index)
        {
            emplace<remove_cvref_t<Type>>(value);
        }
        else
        {
            get<remove_cvref_t<Type>>() = value;
        }
        
        return *this;
    }
    
    /**
    @brief assigns a variant
    Converting assignment.
    If variant already holds a value of alternative type Type, move-assigns the contained value to value.
    If variant does not already hold a value of alternative type Type, move-constructs the contained value from value.
    */
    template <typename Type>
    constexpr enable_if_t<isPartOfV<remove_cvref_t<Type>>, Variant&> operator=(Type&& value)
    {
        constexpr size_t newId = typeToIndexV<remove_cvref_t<Type>>;
        if (newId != m_index)
        {
            emplace<remove_cvref_t<Type>>(forward<Type>(value));
        }
        else
        {
            *getPtr<remove_cvref_t<Type>>() = forward<Type>(value);
        }
        return *this;
    }
    
    
    /**
    @brief constructs a value in the variant, in place
    First, destroys the currently contained value (if any). Then direct-initializes the contained value as if constructing a value of type T_I with the arguments Forward<Args>(args)...
    @tparam Type alternative type to emplace
    @tparam Args variadic type pack for initializing constructor call of selected alternative type
    @param args constructor arguments to use when constructing the new value
    */
    template <typename Type, typename... Args>
    constexpr Type& emplace(Args&&... args)
    {
        static_assert(isPartOfV<Type>, "Invalid type");
        destroy();
        m_index = typeToIndexV<Type>;
        return *(new (getPtr<Type>()) Type(forward<Args>(args)...));
    }
    
    /**
    @brief constructs a value in the variant, in place
    First, destroys the currently contained value (if any). Then direct-initializes the contained value as if constructing a value of type T_I with the arguments Forward<Args>(args)...
    @tparam t_index zero-based index of the alternative type to emplace
    @tparam Args variadic type pack for initializing constructor call of selected alternative type
    @param args constructor arguments to use when constructing the new value
    */
    template <size_type t_index, typename... Args >
    constexpr IndexToTypeT<t_index>& emplace(Args&&... args)
    {
        return emplace<IndexToTypeT<t_index>>(forward<Args>(args)...);
    }
    
    /**
    @brief returns the zero-based index of the alternative held by the variant
    Returns the zero-based index of the alternative that is currently held by the variant.
    If the variant is valueless, returns -1.
    */
    constexpr size_type index() const
    {
        return m_index;
    }

    private:
    
    // Size of variadic type pack
    static constexpr size_type size()
    {
        return sizeof...(Types);
    }
    
    // Copy-construction functor
    struct CopyConstructor
    {
        CopyConstructor(const Variant& other) : m_other(other)
        {}
            
        template <typename Type>
        void operator()(Type& t)
        {
            new (&t) Type(::get<typeToIndexV<Type>>(m_other));
        }
        
        const Variant& m_other;
    };

    // Copy-construct form other
    template <typename Type>
    constexpr void copyConstruct(const Type& other)
    {
        // Use destruction functor as visitor
        visit(move(CopyConstructor(other)), *this);
    }
    
    // Move-construction functor
    struct MoveConstructor
    {
        MoveConstructor(Variant&& other) : m_other(forward<Variant>(other))
        {}
        
        template <typename Type>
        void operator()(Type& t)
        {
            new (&t) Type(move(::get<typeToIndexV<Type>>(m_other)));
        }
        
        Variant&& m_other;
    };

    // Move-construct form other
    template <typename Type>
    constexpr void moveConstruct(Type&& other)
    {
        // Use destruction functor as visitor
        visit(move(MoveConstructor(forward<Variant>(other))), *this);
    }

    // Destruction functor
    struct Destructor
    {
        template <typename Type>
        void operator()(Type& t)
        {
            t.~Type();
        }
    };

    // Destroy current value
    constexpr void destroy()
    {
        // Use destruction functor as visitor
        visit(move(Destructor()), *this);
        m_index = -1;
    }

    // Copy-assignment functor
    struct CopyAssignment
    {
        CopyAssignment(const Variant& other) : m_other(other)
        {}
        
        template <typename Type>
        void operator()(Type& t)
        {
            t = ::get<typeToIndexV<Type>>(m_other);
        }
        
        const Variant& m_other;
    };

    // Copy-assign to other
    constexpr void copyAssign(const Variant& other)
    {
        // Use destruction functor as visitor
        visit(move(CopyAssignment(other)), *this);
    }

    // Move-assignment functor
    struct MoveAssignment
    {
        MoveAssignment(Variant&& other) : m_other(forward<Variant>(other))
        {}
    
        template <typename Type>
        void operator()(Type& t)
        {
            t = move(::get<typeToIndexV<Type>>(m_other));
        }
    
        Variant&& m_other;
    };

    // Move-assign form other
    constexpr void moveAssign(Variant&& other)
    {
        // Use destruction functor as visitor
        visit(move(MoveAssignment(forward<Variant>(other))), *this);
    }

    // Access to underlying buffer        
    template <size_t t_index>
    constexpr auto* getPtr()
    {
        static_assert(t_index < size(), "Id out of range");
        return getPtr<IndexToTypeT<t_index>>();
    }
    
    // Access to underlying buffer
    template <typename Type>
    constexpr auto* getPtr()
    {
        static_assert(isPartOfV<Type>, "Invalid type");
        return reinterpret_cast<Type*>(m_buffer);
    }
    
    // Access to underlying buffer
    template <size_t t_index>
    constexpr const auto* getPtr() const
    {
        static_assert(t_index < size(), "Id out of range");
        return getPtr<IndexToTypeT<t_index>>();
    }
    
    // Access to underlying buffer
    template <typename Type>
    constexpr const auto* getPtr() const
    {
        static_assert(isPartOfV<Type>, "Invalid type");
        return reinterpret_cast<const Type*>(m_buffer);
    }
    
    // Members
    size_t m_index = -1;
    char m_buffer[variantHelper::MaxSizeV<Types...>];
};


/**
@brief reads the value of the variant given the index
Index-based value accessor: If v.index() == t_index, returns a reference to the value stored in v. Otherwise, throws bad_variant_access. The call is ill-formed if t_index is not a valid index in the variant.
@tparam t_index to look up
@tparam Types variadic types pack used by variant v
@param v variant to access
@result Reference to the value stored in the variant
*/
template <size_t t_index, typename... Types>
constexpr variantHelper::IndexToTypeT<t_index, Types...>& get(Variant<Types...>& v)
{
    return *v.template getPtr<t_index>();
}

/**
@brief reads the value of the variant given the index
Index-based value accessor: If v.index() == t_index, returns a reference to the value stored in v. Otherwise, throws bad_variant_access. The call is ill-formed if t_index is not a valid index in the variant.
@tparam t_index to look up
@tparam Types variadic types pack used by variant v
@param v variant to access
@result Reference to the value stored in the variant
*/
template <size_t t_index, typename... Types>
constexpr variantHelper::IndexToTypeT<t_index, Types...>&& get(Variant<Types...>&& v)
{
    return move(*v.template getPtr<t_index>());
}

/**
@brief reads the value of the variant given the index
Index-based value accessor: If v.index() == t_index, returns a reference to the value stored in v. Otherwise, throws bad_variant_access. The call is ill-formed if t_index is not a valid index in the variant.
@tparam t_index to look up
@tparam Types variadic types pack used by variant v
@param v variant to access
@result Reference to the value stored in the variant
*/
template <size_t t_index, typename... Types>
constexpr const variantHelper::IndexToTypeT<t_index, Types...>& get(const Variant<Types...>& v )
{
    return *v.template getPtr<t_index>();
}

/**
@brief reads the value of the variant given the type
Type-based value accessor: If v holds the alternative Type, returns a reference to the value stored in v. Otherwise, throws bad_variant_access. The call is ill-formed if Type is not an element of Types....
@tparam Type type to look up
@tparam Types variadic types pack used by variant v
@param v variant to access
@result Reference to the value stored in the variant
*/
template <typename Type, typename... Types>
constexpr Type& get(Variant<Types...>& v)
{
    return get<variantHelper::typeToIndexV<Type, Types...>>(v);
}

/**
@brief reads the value of the variant given the type
Type-based value accessor: If v holds the alternative Type, returns a reference to the value stored in v. Otherwise, throws bad_variant_access. The call is ill-formed if Type is not an element of Types....
@tparam Type type to look up
@tparam Types variadic types pack used by variant v
@param v variant to access
@result Reference to the value stored in the variant
*/
template <typename Type, typename... Types>
constexpr Type&& get(Variant<Types...>&& v)
{
    return move(get<variantHelper::typeToIndexV<Type, Types...>>(forward<Variant<Types...>>(v)));
}

/**
@brief reads the value of the variant given the type
Type-based value accessor: If v holds the alternative Type, returns a reference to the value stored in v. Otherwise, throws bad_variant_access. The call is ill-formed if Type is not an element of Types....
@tparam Type type to look up
@tparam Types variadic types pack used by variant v
@param v variant to access
@result Reference to the value stored in the variant
*/
template <typename Type, typename... Types>
constexpr const Type& get(const Variant<Types...>& v )
{
    return get<variantHelper::typeToIndexV<Type, Types...>>(v);
}

/**
@brief checks if a variant currently holds a given type
Checks if the variant v holds the alternative Type. The call is ill-formed if T does not appear exactly once in Types...
@tparam Type alternative to check for
@tparam Types package of types used by variant v
@param v variant to examine
@result true if the variant currently holds the alternative Type, false otherwise.
*/
template <typename Type, typename... Types >
constexpr bool holdsAlternative(const Variant<Types...>& v)
{
    return v.index() == variantHelper::typeToIndexV<Type>;
}

/**
@brief placeholder type for use as the first alternative in a variant of non-default-constructible types
Unit type intended for use as a well-behaved empty alternative in Variant.
In particular, a variant of non-default-constructible types may list Monostate as its first alternative: this makes the variant itself default-constructible.
*/
struct Monostate
{};

#else

// TODO compiler message

#endif

#endif