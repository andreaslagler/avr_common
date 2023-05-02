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

#ifndef POTENTIOMETER_SCANNER_H
#define POTENTIOMETER_SCANNER_H

/**
@brief Potentiometer scanner class scanning one or more potentiometers
The actual potentiometer scanning is done synchronously (i.e. actively waiting for AD conversion result)
@tparam Potentiometers Pack of potentiometer driver classes implementing static methods startConversion(), wait(), template<>read() and check(uint16_t). See potentiometer.h
*/
template <typename ... Potentiometers>
class PotentiometerScannerSync
{
    public:
    
    /**
    @brief Scan all attached potentiometers
    This method will start A/D conversion and notify any registered observer on a value change for every attached potentiometer
    */
    static constexpr void scanPotentiometers()
    {
        PotentiometerScannerSyncImpl<Potentiometers ...>::update();
    }
    
    /**
    @brief Initialize all attached potentiometer
    This method will force an update of all potentiometer readout values
    */
    static constexpr void init()
    {
        PotentiometerScannerSyncImpl<Potentiometers ...>::forceUpdate();
    }
    
    private:

    // Private implementation class
    template <typename CurrentPot, typename ... NextPot>
    class PotentiometerScannerSyncImpl
    {
        public:
        
        static constexpr void update() __attribute__((always_inline))
        {
            CurrentPot::updateSync();
            PotentiometerScannerSyncImpl<NextPot ...>::update();
        }
        
        static constexpr void forceUpdate() __attribute__((always_inline))
        {
            CurrentPot::forceUpdateSync();
            PotentiometerScannerSyncImpl<NextPot ...>::forceUpdate();
        }
    };

    // Last potentiometer
    template <typename CurrentPot>
    class PotentiometerScannerSyncImpl<CurrentPot>
    {
        public:
        
        // This method is called to finish the processing. The actual interleaving and potentiometer iteration logic is implemented here
        static constexpr void update() __attribute__((always_inline))
        {
            CurrentPot::updateSync();
        }
        
        static constexpr void forceUpdate() __attribute__((always_inline))
        {
            CurrentPot::forceUpdateSync();
        }
    };
};


#include <functional.h>

/**
@brief Potentiometer scanner class scanning one or more potentiometers
The actual potentiometer scanning is done synchronously (i.e. actively waiting for AD conversion result)
@tparam Potentiometers Pack of potentiometer driver classes implementing static methods startConversion(), wait(), template<>read() and check(uint16_t). See potentiometer.h
*/
template <typename ... Potentiometers>
class PotentiometerScannerAsync
{
    public:
    
    /**
    @brief Initialize all attached potentiometers
    This method will force an update of all potentiometer readout values
    */
    static constexpr void init()
    {
        PotentiometerScannerSync<Potentiometers ...>::init();
    }
    
    /**
    @brief Scan all attached potentiometers
    This method will start A/D conversion and notify any registered observer on a value change for every attached potentiometer
    */
    static constexpr void startOnce()
    {
        s_continue = false;
        PotentiometerScannerAsyncImpl<Potentiometers ...>::start();
    }
    
    /**
    @brief Scan all attached potentiometers
    This method will start A/D conversion and notify any registered observer on a value change for every attached potentiometer
    */
    static constexpr void startContinuous()
    {
        s_continue = true;
        PotentiometerScannerAsyncImpl<Potentiometers ...>::start();
    }
    
    /**
    @brief Scan all attached potentiometers
    This method will start A/D conversion and notify any registered observer on a value change for every attached potentiometer
    */
    static constexpr void stop()
    {
        s_continue = false;
    }
    
    static constexpr void onADCInterrupt()
    {
        s_callback();
    }
    
    private:
    
    // Callback for ADC interrupt
    static function<void()> s_callback;
    
    // Flag indicating continuous operation of potentiometer scanner
    static bool s_continue;

    // Private implementation class
    template <typename CurrentPot, typename ... NextPot>
    class PotentiometerScannerAsyncImpl
    {
        public:
        
        static constexpr void start()
        {
            s_callback = [] {CurrentPot::updateAsync(); PotentiometerScannerAsyncImpl<NextPot...>::start();};
            CurrentPot::Pin::startConversion();
        }
    };

    // Last potentiometer
    template <typename CurrentPot>
    class PotentiometerScannerAsyncImpl<CurrentPot>
    {
        public:
        
        static constexpr void start()
        {
            if (s_continue)
            {
                s_callback = [] {CurrentPot::updateAsync(); PotentiometerScannerAsyncImpl<Potentiometers...>::start();};
            }
            else
            {
                s_callback = [] {CurrentPot::updateAsync();};
            }
            
            CurrentPot::Pin::startConversion();
        }
    };
};

template <typename ... Potentiometers>
function<void()> PotentiometerScannerAsync<Potentiometers...>::s_callback;

template <typename ... Potentiometers>
bool PotentiometerScannerAsync<Potentiometers...>::s_continue = false;


#endif /* POTSCANNER_H_ */