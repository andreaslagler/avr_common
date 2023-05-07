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

#ifndef FUNCTION_H
#define FUNCTION_H

#include <bits/c++config.h>
#include <type_traits.h>
#include <stdbool.h>


namespace functionHelper
{
/**
@brief Helper struct for dispatching an invokable passed as void pointer
@tparam Invokable Type of invokable to dispatch
@tparam Ret Return type of invokable to dispatch
@tparam Args Argument type of invokable to dispatch
*/
template<typename Invokable, typename Ret, typename ... Args>
struct Dispatcher
{
    /**
    @brief Dispatcher method calling an invokable
    The invokable is passed as a void pointer, then up-cast to its actual type
    @param invokable Invokable to dispatch
    @param args Arguments passed to invokable
    @result Result of invokable call
    */
    static CXX14_CONSTEXPR Ret dispatch (void* invokable, Args... args)
    {
        // Due to lack of exceptions, invokable is not checked against nullptr
        return (*(Invokable*)invokable)(args...);
    }
};

/**
@brief Helper struct for dispatching an invokable passed as void pointer
Specialization for invokable of type nullptr_t returning void.
In this special case, a call of the invokable can simply be skipped without the need of exceptions, since the result is void.
@param Args Argument type of invokable to dispatch
*/
template<typename ...Args>
struct Dispatcher<nullptr_t, void, Args...>
{
    /**
    @brief Dispatcher method calling an invokable
    @param invokable Invokable to dispatch
    @param args Arguments passed to invokable
    */
    static CXX14_CONSTEXPR void dispatch (void* , Args...)
    {
        // Skip call of nullptr
    }
};
}


/**
@brief Lightweight replacement for function
*/
template<typename>
class function; // intentionally not defined

/**
@brief Lightweight replacement for function
@tparam Ret Return type of invokable to dispatch
@tparam Args Argument type of invokable to dispatch
*/
template<typename Ret, typename ...Args>
class function<Ret(Args...)>
{
    public:
    
    typedef Ret result_type;

    /**
    @brief Default constructor is not available
    Since exceptions are not available, a valid invokable must be set by the non-standard constructor
    */
    function() = delete;

    /**
    @brief Constructor
    Non-standard constructor for invokables like lambdas or functors
    @tparam Invokable Type of invokable to call
    @param invokable Invokable to call
    */
    template<typename Invokable>
    CXX14_CONSTEXPR function(Invokable&& invokable)
    :
    m_dispatcher(functionHelper::Dispatcher<Invokable, Ret, Args...>::dispatch),
    m_invokable(&invokable)
    {}

    /**
    @brief Constructor
    Non-standard constructor for function pointers
    @param invokable Invokable to call
    */
    CXX14_CONSTEXPR function(Ret invokable(Args...))
    :
    m_dispatcher(functionHelper::Dispatcher<Ret(Args...), Ret, Args...>::dispatch),
    m_invokable((void*)invokable)
    {
        static_assert(sizeof(void*) == sizeof(invokable), "Functions cannot be passed by reference");
    }

    /**
    @brief Assignment operator
    Assignment operator for invokables like lambdas or functors
    @tparam Invokable Type of invokable to call
    @param invokable Invokable to call
    */
    template<typename Invokable>
    CXX14_CONSTEXPR function<Ret(Args...)>& operator=(Invokable&& invokable)
    {
        m_invokable = &invokable;
        m_dispatcher = functionHelper::Dispatcher<typename decay<Invokable>::type, Ret, Args...>::dispatch;
        return *this;
    }

    /**
    @brief Assignment operator
    Assignment operator for function pointers
    @param invokable Invokable to call
    */
    CXX14_CONSTEXPR function<Ret(Args...)>& operator=(Ret invokable(Args...))
    {
        m_invokable = (void*)invokable;
        m_dispatcher = functionHelper::Dispatcher<Ret(Args...), Ret, Args...>::dispatch;
        return *this;
    }
    
    /**
    @brief Call operator
    @param args Argument type of invokable to dispatch
    @result Return value of invokable to dispatch
    */
    CXX14_CONSTEXPR Ret operator()(Args... args) const
    {
        return m_dispatcher(m_invokable, args...);
    }
    
    /**
    @brief boolean operator
    @result Flag indicating if the stored invokable is valid
    */
    CXX14_CONSTEXPR explicit operator bool() const noexcept
    {
        return m_invokable != nullptr;
    }
    
    private:
    
    // Dispatcher
    Ret (*m_dispatcher)(void*, Args...);

    // The actual invokable, down-cast to a void pointer
    void* m_invokable;
};

/**
@brief Lightweight replacement for function
Specialization for void() functions
@tparam Args Argument type of invokable to dispatch
*/
template<typename ...Args>
class function<void(Args...)>
{
    public:
    
    /**
    @brief Default constructor
    */
    CXX14_CONSTEXPR function() = default;

    /**
    @brief Constructor
    Non-standard constructor for invokables like lambdas or functors
    @tparam Invokable Type of invokable to call
    @param invokable Invokable to call
    */
    template<typename Invokable>
    CXX14_CONSTEXPR function(Invokable&& invokable)
    :
    m_dispatcher(functionHelper::Dispatcher<typename decay<Invokable>::type, void, Args...>::dispatch),
    m_invokable(&invokable)
    {}

    /**
    @brief Constructor
    Non-standard constructor for function pointers
    @param invokable Invokable to call
    */
    CXX14_CONSTEXPR function(void invokable(Args...))
    :
    m_dispatcher(functionHelper::Dispatcher<void(Args...), void, Args...>::dispatch),
    m_invokable((void*)invokable)
    {
        static_assert(sizeof(void*) == sizeof(invokable), "Functions cannot be passed by reference");
    }

    /**
    @brief Assignment operator
    Assignment operator for invokables like lambdas or functors
    @tparam Invokable Type of invokable to call
    @param invokable Invokable to call
    */
    template<typename Invokable>
    CXX14_CONSTEXPR function<void(Args...)>& operator=(Invokable&& invokable)
    {
        m_invokable = &invokable;
        m_dispatcher = functionHelper::Dispatcher<typename decay<Invokable>::type, void, Args...>::dispatch;
        return *this;
    }

    /**
    @brief Assignment operator
    Assignment operator for function pointers
    @param invokable Invokable to call
    */
    CXX14_CONSTEXPR function<void(Args...)>& operator=(void invokable(Args...))
    {
        m_invokable = (void*)invokable;
        m_dispatcher = functionHelper::Dispatcher<void(Args...), void, Args...>::dispatch;
        return *this;
    }

    /**
    @brief Assignment operator
    Assignment operator for null pointers
    */
    CXX14_CONSTEXPR function<void(Args...)>& operator=(nullptr_t)
    {
        m_invokable = nullptr;
        m_dispatcher = functionHelper::Dispatcher<nullptr_t, void, Args...>::dispatch;
        return *this;
    }
    
    /**
    @brief Call operator
    @param args Argument type of invokable to dispatch
    */
    CXX14_CONSTEXPR void operator()(Args... args) const
    {
        return m_dispatcher(m_invokable, args...);
    }
    
    /**
    @brief boolean operator
    @result Flag indicating if the stored invokable is valid
    */
    CXX14_CONSTEXPR explicit operator bool() const noexcept
    {
        return m_invokable != nullptr;
    }

    private:

    // Dispatcher method which is calling the actual invokable
    void (*m_dispatcher)(void*, Args...) = functionHelper::Dispatcher<nullptr_t, void, Args...>::dispatch;

    // The actual invokable, down-cast to a void pointer
    void* m_invokable = nullptr;
};

#endif