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

#include <stdint.h>
#include "buffered_usart.h"

void printTx(const uint8_t data)
{
    // Put a tracepoint here
    // Tx data = {data}
}

// UDR empty interrupt
void UDREInterrupt();

// Dummy emulating an ATmega USART
class DummyUSART
{
public:
    
    // USART Tx interrupt
    static void txInterrupt()
    {
        if (s_interruptEnabled)
        {
            UDREInterrupt();
        }
    }

protected:

    static void put(const uint8_t data)
    {
        printTx(data);
        s_data = data;
    }
    
    // USART source echoing the last transmitted byte
    static uint8_t get()
    {
        return s_data;
    }
    
    static void startTransmission()
    {
        s_interruptEnabled = true;
    }

    static void stopTransmission()
    {
        s_interruptEnabled = false;
    }
    
    static uint8_t s_data;
    static bool s_interruptEnabled;
};

uint8_t DummyUSART::s_data = 0;
bool DummyUSART::s_interruptEnabled = false;

using TestUSART = BufferedUSART<DummyUSART, 4>;

int main(void)
{
    uint8_t data = 42;
    
    // Single byte
    TestUSART::put(data++);
    DummyUSART::txInterrupt();
    
    // Multiple bytes
    TestUSART::put(data++);
    TestUSART::put(data++);
    TestUSART::put(data++);
    TestUSART::put(data++);
    TestUSART::put(data++);
    TestUSART::put(data++);
    TestUSART::put(data++);
    TestUSART::put(data++);
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    
    data = 0;
    
    // This should output numbers 0 to 14
    while(TestUSART::put(data++));    
    
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();
    DummyUSART::txInterrupt();

    
    while (true)
    {
    }
}

void UDREInterrupt()
{
    TestUSART::transmitNextByte();
}