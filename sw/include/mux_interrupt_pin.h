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

#ifndef MUX_INTERRUPT_PIN_H
#define MUX_INTERRUPT_PIN_H

/**
@brief Generic driver for an interrupt multiplexer
@tparam Device Actual multiplexer device driver class implementing static methods getNofLines() and getLine()
@tparam PinConfig
*/
template <typename Device, uint8_t ... PinConfig>
class InterruptMux
{
    // Struct retrieving the pin usage from the pin configuration for a given pin index
    template <uint8_t t_pinIdx, uint8_t t_currentPinIdx, uint8_t ... t_nextPinIdx>
    struct PinUsed
    {
        // Search the parameter pack for configuration of given pin index
        static constexpr bool get()
        {
            check();
            return (t_pinIdx == t_currentPinIdx) || PinUsed<t_pinIdx, t_nextPinIdx ...>::get();
        }
        
        // Check validity of pin configuration, pins must not be used more than once
        static constexpr uint8_t check()
        {
            static_assert(t_currentPinIdx < Device::getNofLines(), "Invalid pin configuration, pin index out of bounds");
            constexpr uint8_t pinMask = _BV(t_currentPinIdx);
            static_assert((pinMask & PinUsed<t_pinIdx, t_nextPinIdx ...>::check()) == 0, "Invalid pin configuration, check for multiple use of pins");
            return pinMask | PinUsed<t_pinIdx, t_nextPinIdx ...>::check();
        }
    };
    
    // Struct retrieving the pin type from the pin configuration for a given pin index
    // Termination for last configured pin
    template <uint8_t t_pinIdx, uint8_t t_currentPinIdx>
    struct PinUsed <t_pinIdx, t_currentPinIdx>
    {
        // If a configuration for given pin index does not exist, the pin is considered as unused
        static constexpr bool get()
        {
            check();
            return t_pinIdx == t_currentPinIdx;
        }
        
        // Check validity of pin configuration
        static constexpr uint8_t check()
        {
            static_assert(t_currentPinIdx < Device::getNofLines(), "Invalid pin configuration, pin index out of bounds");
            constexpr uint8_t pinMask = _BV(t_currentPinIdx);
            return pinMask;
        }
    };

    public:
    
    // Pin driver base class
    template <uint8_t t_pinIdx, bool t_pinUsed = PinUsed<t_pinIdx, PinConfig ...>::get()>
    class Pin
    {};

    // Pin driver base class for an unused pin
    template <uint8_t t_pinIdx>
    class Pin<t_pinIdx, false>
    {
        // Parent interrupt multiplexer driver is allowed to notify pin observers
        friend class InterruptMux<Device, PinConfig ...>;

        // Dummy observer notification method
        static void notifyObserver(const uint8_t pinIdx)
        {
            if constexpr (t_pinIdx > 0)
            {
                Pin<t_pinIdx-1>::notifyObserver(pinIdx);
            }
        }
    };
    
    // Pin driver base class for a used pin
    template <uint8_t t_pinIdx>
    class Pin <t_pinIdx, true>
    {
        // Parent interrupt multiplexer driver is allowed to notify pin observers
        friend class InterruptMux<Device, PinConfig ...>;

        // Subject for observer registration
        static Subject<void> s_subject;
        
        // Interrupt enable flag
        static bool s_interruptEnabled;

        // Observer notification
        static constexpr void notifyObserver(const uint8_t pinIdx)
        {
            if (t_pinIdx == pinIdx && s_interruptEnabled)
            {
                s_subject.notifyObserver();
            }

            if constexpr (t_pinIdx > 0)
            {
                Pin<t_pinIdx-1>::notifyObserver(pinIdx);
            }
        }
        
        public:
        
        static constexpr void enableInterrupt()
        {
            s_interruptEnabled = true;
        }
        
        static constexpr void disableInterrupt()
        {
            s_interruptEnabled = false;
        }
        
        static constexpr void registerObserver(const Subject<void>::Observer& observer)
        {
            s_subject.registerObserver(observer);
        }
    };
    
    /**
    @brief Callback for interrupt
    */
    static void onInterrupt()
    {
        // Read interrupt flags and propagate interrupt to corresponding pin class
        Pin<Device::getNofLines()-1>::notifyObserver(Device::getLine());
    }
};

// Static initialization
template <typename Device, uint8_t ... PinConfig>
template <uint8_t t_pinIdx>
Subject<void> InterruptMux<Device, PinConfig ...>::Pin<t_pinIdx, true>::s_subject;

template <typename Device, uint8_t ... PinConfig>
template <uint8_t t_pinIdx>
bool InterruptMux<Device, PinConfig ...>::Pin<t_pinIdx, true>::s_interruptEnabled = false;

/**
@brief Driver for an individual pin of an interrupt multiplexer device
@tparam Mux Interrupt multiplexer device driver class implementing a Pin sub-class
@tparam t_pinIdx Pin index on the interrupt multiplexer device
*/
template <typename Mux, uint8_t t_pinIdx>
class MuxInterruptPin : public Mux::Pin<t_pinIdx>
{};

#endif