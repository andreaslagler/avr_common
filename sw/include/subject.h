/*
Copyright (C) 2022 Andreas Lagler

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

#ifndef SUBJECT_H
#define SUBJECT_H

/**
@brief Template class implementing a primitive subject base class according to the 'observer' design pattern.
The GoF implementation of the 'observer' design pattern suggests the use of an abstract observer base class.
Due to memory constraints (and missing std::function on avr-gcc), a plain function pointer is used as an observer in this implementation.
@note Since std::new is not available in avr-gcc, only one observer can be registered at a time
@tparam Arg Type(s) of the observed state(s)
*/
template <typename ... Arg>
class Subject
{
    public:

    /// @brief Observer data type is a function pointer accepting Arg ... as argument
    typedef void(*Observer)(Arg ...);

    /**
    @brief Constructor
    @param observer Observer to register
    */
    constexpr Subject(const Observer& observer = nullptr) : m_observer(observer)
    {}

    /**
    @brief Register an observer. A previously registered observer will be unregistered.
    @param observer Observer to register
    */
    constexpr void registerObserver(const Observer& observer)
    {
        m_observer = observer;
    }

    /**
    @brief Unregister the currently registered observer.
    */
    constexpr void unregisterObserver()
    {
        m_observer = nullptr;
    }

    /**
    @brief Notify the currently registered observer.
    @param arg Arguments to be passed to the observer on notification
    @note This method should always be inlined, because the actual observer notification will always be an indirect call
    */
    constexpr void notifiyObserver(const Arg ... arg) const __attribute__((always_inline))
    {
        if (nullptr != m_observer)
        {
            m_observer(arg ...);
        }
    }

    private:

    Observer m_observer {nullptr};
};

/**
Subject specialization for void subjects (e.g. interrupts)
*/
template <>
class Subject<void>
{
    public:

    /// @brief Observer data type is a function pointer without arguments
    typedef void(*Observer)();

    /**
    @brief Constructor
    @param observer Observer to register
    */
    constexpr Subject(const Observer& observer = nullptr) : m_observer(observer)
    {}

    /**
    @brief Register an observer. A previously registered observer will be unregistered.
    @param observer Observer to register
    */
    constexpr void registerObserver(const Observer observer)
    {
        m_observer = observer;
    }

    /**
    @brief Unregister the currently registered observer.
    */
    constexpr void unregisterObserver()
    {
        m_observer = nullptr;
    }

    /**
    @brief Notify the currently registered observer.
    @note This method should always be inlined, because the actual observer notification will always be an indirect call
    */
    constexpr void notifiyObserver() const __attribute__((always_inline))
    {
        if (nullptr != m_observer)
        {
            m_observer();
        }
    }

    private:

    Observer m_observer {nullptr};
};

#endif