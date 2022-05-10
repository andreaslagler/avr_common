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

#include <stdint.h>
#include <stdbool.h>

/**
@brief Potentiometer scanner class scanning one or more potentiometers
Since the actual potentiometer scanning is done synchronously (i.e. actively waiting for AD conversion result), the idle time during potentiometer value retrieval is used to process the observer notification on value change of a different potentiometer
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
    static void scanPotentiometers(const bool forceUpdate = false)
    {
        PotentiometerScannerSyncImpl<Potentiometers ...>::startProcessing();
        PotentiometerScannerSyncImpl<Potentiometers ...>::finishProcessing(forceUpdate);
    }
    
    /**
    @brief Initialize all attached potentiometer
    This method will force an update of all potentiometer readout values
    */
    static void init()
    {
        scanPotentiometers(true);
    }
    
    private:

    // Private implementation class implementing the interleaved/pipeline sequence of A/D conversion/readout/notification calls
    template <typename CurrentPot, typename ... NextPot>
    class PotentiometerScannerSyncImpl
    {
        public:
        
        // This method will be the entry point for any number of potentiometers attached
        static void startProcessing() __attribute__((always_inline))
        {
            // Start AD conversion for current potentiometer
            CurrentPot::startConversion();
        }
        
        // This method is called to finish the processing. The actual interleaving and potentiometer iteration logic is implemented here
        static void finishProcessing(const bool forceUpdate) __attribute__((always_inline))
        {
            // Wait for AD conversion for current potentiometer
            CurrentPot::wait();
            
            // Read pot value
            const uint16_t value = CurrentPot::template read<uint16_t>();
            
            // Start processing of the next potentiometer before triggering the notification on potentiometer value change
            PotentiometerScannerSyncImpl<NextPot ...>::startProcessing();
            
            // Notify any registered observer
            if (forceUpdate)
            {
                CurrentPot::set(value);                
            }
            else
            {
                CurrentPot::check(value);                
            }
            
            // Iterate to next potentiometer
            PotentiometerScannerSyncImpl<NextPot ...>::finishProcessing(forceUpdate);
        }
    };

    // Last potentiometer
    template <typename CurrentPot>
    class PotentiometerScannerSyncImpl<CurrentPot>
    {
        public:
        
        // This method will be the entry point for any number of potentiometers attached
        static void startProcessing() __attribute__((always_inline))
        {
            // Start A/D conversion for current potentiometer
            CurrentPot::startConversion();
        }

        // This method is called to finish the processing. The potentiometer iteration loop is terminated here
        static void finishProcessing(const bool forceUpdate) __attribute__((always_inline))
        {
            // Wait for A/D conversion for last potentiometer
            CurrentPot::wait();
            
            // Read the value and notify any registered observer
            const uint16_t value = CurrentPot::template read<uint16_t>();
            if (forceUpdate)
            {
                CurrentPot::set(value);
            }
            else
            {
                CurrentPot::check(value);
            }

        }
    };
};

#endif /* POTSCANNER_H_ */