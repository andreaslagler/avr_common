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

#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <bits/c++config.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//Here is a list of type traits intrinsics used by GCC as of version 4.7:
//__is_pod
//__is_trivial
//__is_standard_layout
//__is_literal_type
//__is_enum
//__is_union
//__is_class
//__is_empty
//__is_polymorphic
//__is_abstract
//__is_base_of
//__has_trivial_constructor
//__has_trivial_copy
//__has_trivial_assign
//__has_trivial_destructor
//__has_nothrow_constructor
//__has_nothrow_copy
//__has_nothrow_assign
//__has_virtual_destructor


    // helper class
    template <class T, T v>
    struct integral_constant {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant<T, v>;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };
    
    template<bool B>
    using bool_constant = integral_constant<bool, B>;
    
    using true_type  = bool_constant<true>;
    using false_type = bool_constant<false>;
    
    // const-volatile modifications
    template<class T> struct remove_cv                   { typedef T type; };
    template<class T> struct remove_cv<const T>          { typedef T type; };
    template<class T> struct remove_cv<volatile T>       { typedef T type; };
    template<class T> struct remove_cv<const volatile T> { typedef T type; };
    template<class T> using remove_cv_t = typename remove_cv<T>::type;
    
    template<class T> struct remove_const                { typedef T type; };
    template<class T> struct remove_const<const T>       { typedef T type; };
    template<class T> using remove_const_t = typename remove_const<T>::type;
    
    template<class T> struct remove_volatile             { typedef T type; };
    template<class T> struct remove_volatile<volatile T> { typedef T type; };
    template<class T> using remove_volatile_t = typename remove_volatile<T>::type;
    
    template<class T> struct add_cv { typedef const volatile T type; };
    template<class T> using add_cv_t = typename add_cv<T>::type;
    
    template<class T> struct add_const { typedef const T type; };
    template<class T> using add_const_t = typename add_const<T>::type;
    
    template<class T> struct add_volatile { typedef volatile T type; };
    template<class T> using add_volatile_t = typename add_volatile<T>::type;
    
    // reference modifications
    template<class T> struct remove_reference      { typedef T type; };
    template<class T> struct remove_reference<T&>  { typedef T type; };
    template<class T> struct remove_reference<T&&> { typedef T type; };
    template<class T> using remove_reference_t = typename remove_reference<T>::type;
    
    template<class T> struct add_lvalue_reference;
    template<class T> struct add_rvalue_reference;
    
    
    namespace detail
    {        
        template <class T> struct type_identity { using type = T; }; // or use type_identity (since C++20)
        
        template <class T> auto try_add_lvalue_reference(int) -> type_identity<T&>; // Note that `cv void&` is a substitution failure
        template <class T> auto try_add_lvalue_reference(...) -> type_identity<T>; // Handle T = cv void case
        
        template <class T> auto try_add_rvalue_reference(int) -> type_identity<T&&>;
        template <class T> auto try_add_rvalue_reference(...) -> type_identity<T>;        
    } // namespace detail
    
    template <class T> struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0)) {};
    template<class T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
    
    template <class T> struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0)) {};
    template<class T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
    
    // sign modifications
    // TODO template<class T> struct make_signed;
    // TODO template<class T> struct make_unsigned;
    // TODO 
    // TODO template<class T> using make_signed_t = typename make_signed<T>::type;
    // TODO template<class T> using make_unsigned_t = typename make_unsigned<T>::type;
    
    // array modifications
    template<class T> struct remove_extent { typedef T type; };
    template<class T> struct remove_extent<T[]> { typedef T type; };
    template<class T, size_t N> struct remove_extent<T[N]> { typedef T type; };
    template<class T> using remove_extent_t = typename remove_extent<T>::type;
        
    template<class T> struct remove_all_extents { typedef T type; };    
    template<class T> struct remove_all_extents<T[]> { typedef typename remove_all_extents<T>::type type; };
    template<class T, size_t N> struct remove_all_extents<T[N]> { typedef typename remove_all_extents<T>::type type; };
    template<class T> using remove_all_extents_t = typename remove_all_extents<T>::type;
    
    // pointer modifications
    template<class T> struct remove_pointer                    {typedef T type;};
    template<class T> struct remove_pointer<T*>                {typedef T type;};
    template<class T> struct remove_pointer<T* const>          {typedef T type;};
    template<class T> struct remove_pointer<T* volatile>       {typedef T type;};
    template<class T> struct remove_pointer<T* const volatile> {typedef T type;};
    template<class T> using remove_pointer_t = typename remove_pointer<T>::type;
    
    namespace detail
    {        
        // template <class T> struct type_identity { using type = T; }; // or use type_identity (since C++20)
        
        template <class T> auto try_add_pointer(int) -> type_identity<typename remove_reference<T>::type*>;
        template <class T> auto try_add_pointer(...) -> type_identity<T>;        
    } // namespace detail
    
    template <class T> struct add_pointer : decltype(detail::try_add_pointer<T>(0)) {};
    template<class T> using add_pointer_t = typename add_pointer<T>::type;
    
    // type relations
    template<class T, class U> struct is_same : false_type {};    
    template<class T> struct is_same<T, T> : true_type {};
    
    template<class Base, class Derived> struct is_base_of : public bool_constant<__is_base_of(Base, Derived)> {};
        
        
        
            
    
    template<class T, class U> struct is_layout_compatible;
    template<class Base, class Derived> struct is_pointer_interconvertible_base_of;
    
    template<class Fn, class... ArgTypes> struct is_invocable;
    template<class R, class Fn, class... ArgTypes> struct is_invocable_r;
    
    template<class Fn, class... ArgTypes> struct is_nothrow_invocable;
    template<class R, class Fn, class... ArgTypes> struct is_nothrow_invocable_r;
    
    // primary type categories
    template<class T> struct is_void : is_same<void, typename remove_cv<T>::type> {};
    template<class T> struct is_null_pointer : is_same<nullptr_t, remove_cv_t<T>> {};
    template<class T> struct is_integral;
    template<class T> struct is_floating_point;
    
    template<class T> struct is_array : false_type {};    
    template<class T> struct is_array<T[]> : true_type {};    
    template<class T, size_t N> struct is_array<T[N]> : true_type {};
        
    template<class T> struct is_pointer;
    template<class T> struct is_lvalue_reference;
    template<class T> struct is_rvalue_reference;
    template<class T> struct is_member_object_pointer;
    template<class T> struct is_member_function_pointer;
    template<class T> struct is_enum { static constexpr bool value = __is_enum(T); };
    template<class T> struct is_union;
    template<class T> struct is_class;
    
    
// primary template
template<class>
struct is_function : false_type { };

// specialization for regular functions
template<class Ret, class... Args> struct is_function<Ret(Args...)> : true_type {};

// specialization for variadic functions such as printf
template<class Ret, class... Args> struct is_function<Ret(Args......)> : true_type {};

// specialization for function types that have cv-qualifiers
template<class Ret, class... Args> struct is_function<Ret(Args...) const> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) volatile> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const volatile> : true_type {};

// specialization for function types that have ref-qualifiers
template<class Ret, class... Args> struct is_function<Ret(Args...) &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) volatile &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const volatile &> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) volatile &&> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const volatile &&> : true_type {};

// specializations for noexcept versions of all the above (C++17 and later)
#if SINCE_CXX17
template<class Ret, class... Args> struct is_function<Ret(Args...) noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) volatile noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const volatile noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) volatile & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const volatile & noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) volatile && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args...) const volatile && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) volatile && noexcept> : true_type {};
template<class Ret, class... Args> struct is_function<Ret(Args......) const volatile && noexcept> : true_type {};
    #endif // Since C++17
        
    // composite type categories
    template<class T> struct is_reference;
    template<class T> struct is_arithmetic;
    template<class T> struct is_fundamental;
    template<class T> struct is_object;
    template<class T> struct is_scalar;
    template<class T> struct is_compound;
    template<class T> struct is_member_pointer;
    
    // type properties
    template<class T> struct is_const;
    template<class T> struct is_volatile;
    template<class T> struct is_trivial;
    template<class T> struct is_trivially_copyable;
    template<class T> struct is_standard_layout;
    template<class T> struct is_empty;
    template<class T> struct is_polymorphic;
    template<class T> struct is_abstract;
    template<class T> struct is_final;
    template<class T> struct is_aggregate;
    
    template<class T> struct is_signed;
    template<class T> struct is_unsigned;
    template<class T> struct is_bounded_array;
    template<class T> struct is_unbounded_array;
    template<class T> struct is_scoped_enum;
    
    
    
    template<typename _Tp>
    typename add_rvalue_reference<_Tp>::type declval() noexcept;
    
    namespace detail
    {
        template<class T> auto test_returnable(int) -> decltype(void(static_cast<T(*)()>(nullptr)), true_type{});
        template<class> auto test_returnable(...) -> false_type;
        
        template<class From, class To> auto test_implicitly_convertible(int) -> decltype(void(declval<void(&)(To)>()(declval<From>())), true_type{});
        template<class, class> auto test_implicitly_convertible(...) -> false_type;
    } // namespace detail

    template<class From, class To>
    struct is_convertible : integral_constant<bool,
    (decltype(detail::test_returnable<To>(0))::value &&
    decltype(detail::test_implicitly_convertible<From, To>(0))::value) ||
    (is_void<From>::value && is_void<To>::value)
    > {};
    
    
            
            
    
    // C++20
    // template<class From, class To> struct is_nothrow_convertible;

    template<class T, class... Args> struct is_constructible;
    template<class T> struct is_default_constructible;
    template<class T> struct is_copy_constructible;
    template<class T> struct is_move_constructible;
    
    template<class T, class U> struct is_assignable;
    template<class T> struct is_copy_assignable;
    template<class T> struct is_move_assignable;
    
    template<class T, class U> struct is_swappable_with;
    template<class T> struct is_swappable;
    
    template<class T> struct is_destructible;
    
    template<class T, class... Args> struct is_trivially_constructible;
    template<class T> struct is_trivially_default_constructible;
    template<class T> struct is_trivially_copy_constructible;
    template<class T> struct is_trivially_move_constructible;
    
    template<class T, class U> struct is_trivially_assignable;
    template<class T> struct is_trivially_copy_assignable;
    template<class T> struct is_trivially_move_assignable;
    template<class T> struct is_trivially_destructible;
    
    template<class T, class... Args> struct is_nothrow_constructible;
    template<class T> struct is_nothrow_default_constructible;
    template<class T> struct is_nothrow_copy_constructible;
    template<class T> struct is_nothrow_move_constructible;
    
    template<class T, class U> struct is_nothrow_assignable;
    template<class T> struct is_nothrow_copy_assignable;
    template<class T> struct is_nothrow_move_assignable;
    
    template<class T, class U> struct is_nothrow_swappable_with;
    template<class T> struct is_nothrow_swappable;
    
    template<class T> struct is_nothrow_destructible;
    
    template<class T> struct has_virtual_destructor;
    
    template<class T> struct has_unique_object_representations;
    
    template<class T, class U> struct reference_constructs_from_temporary;
    template<class T, class U> struct reference_converts_from_temporary;
    
    // type property queries
    template<class T> struct alignment_of;
    template<class T> struct rank;
    template<class T, unsigned I = 0> struct extent;
    
    // other transformations
    template<class T> struct type_identity;
    template<class T> struct remove_cvref
	{
		typedef typename remove_reference<typename remove_cv<T>::type>::type type;
	};
    
    template<bool, class T, class F>
    struct conditional { using type = T; };
    
    template<class T, class F>
    struct conditional<false, T, F> { using type = F; };

    template<bool b, class T, class F>
    using conditional_t      = typename conditional<b, T, F>::type;
    
    template<class T> struct decay
    {
        private:
        typedef typename remove_reference<T>::type U;
        public:
        typedef typename conditional<
        is_array<U>::value,
        typename add_pointer<typename remove_extent<U>::type>::type,
        typename conditional<
        is_function<U>::value,
        typename add_pointer<U>::type,
        typename remove_cv<U>::type
        >::type
        >::type type;
    };
    
    template<bool, class T = void> struct enable_if;
    template<class T> struct enable_if<false, T> {};
    template<class T> struct enable_if<true, T> {typedef T type;};


    template<class... T> struct common_type;
    template<class T, class U, template<class> class TQual, template<class> class UQual>
    struct basic_common_reference { };
    template<class... T> struct common_reference;
    template<class T> struct underlying_type : public conditional<is_enum<T>::value && (sizeof(T) < 9), conditional_t<(sizeof(T) > 4), uint64_t, conditional_t<(sizeof(T) > 2), uint32_t, conditional_t<(sizeof(T) > 1), uint16_t, uint8_t>>>, T> {};
    template<class Fn, class... ArgTypes> struct invoke_result;
    template<class T> struct unwrap_reference;
    template<class T> struct unwrap_ref_decay;
    
    template<class T>
    using type_identity_t    = typename type_identity<T>::type;
    template<class T>
    using remove_cvref_t     = typename remove_cvref<T>::type;
    template<class T>
    using decay_t            = typename decay<T>::type;
    template<bool b, class T = void>
    using enable_if_t        = typename enable_if<b, T>::type;
    template<class... T>
    using common_type_t      = typename common_type<T...>::type;
    template<class... T>
    using common_reference_t = typename common_reference<T...>::type;
    template<class T>
    using underlying_type_t  = typename underlying_type<T>::type;
    template<class Fn, class... ArgTypes>
    using invoke_result_t    = typename invoke_result<Fn, ArgTypes...>::type;
    template<class T>
    using unwrap_reference_t = typename unwrap_reference<T>::type;
    template<class T>
    using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
    template<class...>
    using void_t             = void;
    
    // logical operator traits
    template<class... B> struct conjunction;
    template<class... B> struct disjunction;
    template<class B> struct negation;
    #if SINCE_CXX17
    // primary type categories
    template<class T>
    inline constexpr bool is_void_v = is_void<T>::value;
    template<class T>
    inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
    template<class T>
    inline constexpr bool is_integral_v = is_integral<T>::value;
    template<class T>
    inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
    template<class T>
    inline constexpr bool is_array_v = is_array<T>::value;
    template<class T>
    inline constexpr bool is_pointer_v = is_pointer<T>::value;
    template<class T>
    inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
    template<class T>
    inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
    template<class T>
    inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
    template<class T>
    inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
    template<class T>
    inline constexpr bool is_enum_v = is_enum<T>::value;
    template<class T>
    inline constexpr bool is_union_v = is_union<T>::value;
    template<class T>
    inline constexpr bool is_class_v = is_class<T>::value;
    template<class T>
    inline constexpr bool is_function_v = is_function<T>::value;
    
    // composite type categories
    template<class T>
    inline constexpr bool is_reference_v = is_reference<T>::value;
    template<class T>
    inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
    template<class T>
    inline constexpr bool is_fundamental_v = is_fundamental<T>::value;
    template<class T>
    inline constexpr bool is_object_v = is_object<T>::value;
    template<class T>
    inline constexpr bool is_scalar_v = is_scalar<T>::value;
    template<class T>
    inline constexpr bool is_compound_v = is_compound<T>::value;
    template<class T>
    inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;
    
    // type properties
    template<class T>
    inline constexpr bool is_const_v = is_const<T>::value;
    template<class T>
    inline constexpr bool is_volatile_v = is_volatile<T>::value;
    template<class T>
    inline constexpr bool is_trivial_v = is_trivial<T>::value;
    template<class T>
    inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
    template<class T>
    inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;
    template<class T>
    inline constexpr bool is_empty_v = is_empty<T>::value;
    template<class T>
    inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
    template<class T>
    inline constexpr bool is_abstract_v = is_abstract<T>::value;
    template<class T>
    inline constexpr bool is_final_v = is_final<T>::value;
    template<class T>
    inline constexpr bool is_aggregate_v = is_aggregate<T>::value;
    template<class T>
    inline constexpr bool is_signed_v = is_signed<T>::value;
    template<class T>
    inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
    template<class T>
    inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
    template<class T>
    inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;
    template<class T>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
    template<class T, class... Args>
    inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
    template<class T>
    inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
    template<class T>
    inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
    template<class T>
    inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;
    template<class T, class U>
    inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
    template<class T>
    inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
    template<class T>
    inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;
    template<class T, class U>
    inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
    template<class T>
    inline constexpr bool is_swappable_v = is_swappable<T>::value;
    template<class T>
    inline constexpr bool is_destructible_v = is_destructible<T>::value;
    template<class T, class... Args>
    inline constexpr bool is_trivially_constructible_v
    = is_trivially_constructible<T, Args...>::value;
    template<class T>
    inline constexpr bool is_trivially_default_constructible_v
    = is_trivially_default_constructible<T>::value;
    template<class T>
    inline constexpr bool is_trivially_copy_constructible_v
    = is_trivially_copy_constructible<T>::value;
    template<class T>
    inline constexpr bool is_trivially_move_constructible_v
    = is_trivially_move_constructible<T>::value;
    template<class T, class U>
    inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
    template<class T>
    inline constexpr bool is_trivially_copy_assignable_v
    = is_trivially_copy_assignable<T>::value;
    template<class T>
    inline constexpr bool is_trivially_move_assignable_v
    = is_trivially_move_assignable<T>::value;
    template<class T>
    inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
    template<class T, class... Args>
    inline constexpr bool is_nothrow_constructible_v
    = is_nothrow_constructible<T, Args...>::value;
    template<class T>
    inline constexpr bool is_nothrow_default_constructible_v
    = is_nothrow_default_constructible<T>::value;
    template<class T>
    inline constexpr bool is_nothrow_copy_constructible_v
    = is_nothrow_copy_constructible<T>::value;
    template<class T>
    inline constexpr bool is_nothrow_move_constructible_v
    = is_nothrow_move_constructible<T>::value;
    template<class T, class U>
    inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
    template<class T>
    inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;
    template<class T>
    inline constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;
    template<class T, class U>
    inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
    template<class T>
    inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
    template<class T>
    inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;
    template<class T>
    inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
    template<class T>
    inline constexpr bool has_unique_object_representations_v
    = has_unique_object_representations<T>::value;
    template<class T, class U>
    inline constexpr bool reference_constructs_from_temporary_v
    = reference_constructs_from_temporary<T, U>::value;
    template<class T, class U>
    inline constexpr bool reference_converts_from_temporary_v
    = reference_converts_from_temporary<T, U>::value;
    
    // type property queries
    template<class T>
    inline constexpr size_t alignment_of_v = alignment_of<T>::value;
    template<class T>
    inline constexpr size_t rank_v = rank<T>::value;
    template<class T, unsigned I = 0>
    inline constexpr size_t extent_v = extent<T, I>::value;
    
    // type relations
    template<class T, class U>
    inline constexpr bool is_same_v = is_same<T, U>::value;
    template<class Base, class Derived>
    inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
    template<class From, class To>
    inline constexpr bool is_convertible_v = is_convertible<From, To>::value;
    
    // C++20
    //template<class From, class To>
    //inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;
    
    template<class T, class U>
    inline constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;
    template<class Base, class Derived>
    inline constexpr bool is_pointer_interconvertible_base_of_v
    = is_pointer_interconvertible_base_of<Base, Derived>::value;
    template<class Fn, class... ArgTypes>
    inline constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;
    template<class R, class Fn, class... ArgTypes>
    inline constexpr bool is_invocable_r_v = is_invocable_r<R, Fn, ArgTypes...>::value;
    template<class Fn, class... ArgTypes>
    inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, ArgTypes...>::value;
    template<class R, class Fn, class... ArgTypes>
    inline constexpr bool is_nothrow_invocable_r_v
    = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;
    
    // logical operator traits
    template<class... B>
    inline constexpr bool conjunction_v = conjunction<B...>::value;
    template<class... B>
    inline constexpr bool disjunction_v = disjunction<B...>::value;
    template<class B>
    inline constexpr bool negation_v = negation<B>::value;
    #endif // Since C++17
    
    // member relationships
    template<class S, class M>
    constexpr bool is_pointer_interconvertible_with_class(M S::*m) noexcept;
    template<class S1, class S2, class M1, class M2>
    constexpr bool is_corresponding_member(M1 S1::*m1, M2 S2::*m2) noexcept;
    
    // constant evaluation context
    constexpr bool is_constant_evaluated() noexcept;

	// Cast a size_t value to the smallest possible integer type that can represent the value
	template <size_t t_number, bool t_isUint16 = t_number < 65536, bool t_isUint8 = t_number < 256>
	struct DownCast;

	template <size_t t_number>
	struct DownCast<t_number, false, false>
	{
		typedef size_t type;
		static constexpr size_t value = static_cast<size_t>(t_number);
	};

	template <size_t t_number>
	struct DownCast<t_number, true, false>
	{
		typedef uint16_t type;
		static constexpr type value = static_cast<type>(t_number);
	};

	template <size_t t_number>
	struct DownCast<t_number, true, true>
	{
		typedef uint8_t type;
		static constexpr type value = static_cast<type>(t_number);
	};

	template< typename from, typename to >
	struct copy_cv
	{
		using type = to;
	};

	template< typename from, typename to >
	struct copy_cv< from const, to > : copy_cv< from, to const >
	{};

	template< typename from, typename to >
	struct copy_cv< volatile from, to > : copy_cv< from, volatile to >
	{};

	template< typename from, typename to >
	struct copy_cv< volatile from const, to > : copy_cv< from, volatile to const >
	{};

#endif