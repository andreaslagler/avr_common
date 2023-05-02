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

#ifndef ATOMIC_H
#define ATOMIC_H

// See avr/atomic.h

/* Copyright (c) 2007 Dean Camera
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <register_access.h>

#if !defined(__DOXYGEN__)
/* Internal helper functions. */
static __inline__ uint8_t __iSeiRetVal(void)
{
    sei();
    return 1;
}

static __inline__ uint8_t __iCliRetVal(void)
{
    cli();
    return 1;
}

static __inline__ void __iSeiParam(const uint8_t *__s)
{
    sei();
    __asm__ volatile ("" ::: "memory");
    (void)__s;
}

static __inline__ void __iCliParam(const uint8_t *__s)
{
    cli();
    __asm__ volatile ("" ::: "memory");
    (void)__s;
}

static __inline__ void __iRestore(const  uint8_t *__s)
{
    SREG::write(*__s);
    __asm__ volatile ("" ::: "memory");
}
#endif	/* !__DOXYGEN__ */


/** See avr/atomic.h

    Creates a block of code that is guaranteed to be executed
    atomically. Upon entering the block the Global Interrupt Status
    flag in SREG is disabled, and re-enabled upon exiting the block
    from any exit path.

    Two possible macro parameters are permitted, ATOMIC_RESTORESTATE
    and ATOMIC_FORCEON.
*/
#if defined(__DOXYGEN__)
#define ATOMIC_BLOCK(type)
#else
#define ATOMIC_BLOCK(type) for ( type, __ToDo = __iCliRetVal(); __ToDo ; __ToDo = 0 )
#endif	/* __DOXYGEN__ */

/** See avr/atomic.h

    Creates a block of code that is executed non-atomically. Upon
    entering the block the Global Interrupt Status flag in SREG is
    enabled, and disabled upon exiting the block from any exit
    path. This is useful when nested inside ATOMIC_BLOCK sections,
    allowing for non-atomic execution of small blocks of code while
    maintaining the atomic access of the other sections of the parent
    ATOMIC_BLOCK.

    Two possible macro parameters are permitted,
    NONATOMIC_RESTORESTATE and NONATOMIC_FORCEOFF.
*/
#if defined(__DOXYGEN__)
#define NONATOMIC_BLOCK(type)
#else
#define NONATOMIC_BLOCK(type) for ( type, __ToDo = __iSeiRetVal(); __ToDo ;  __ToDo = 0 )
#endif	/* __DOXYGEN__ */

/** See avr/atomic.h

    This is a possible parameter for ATOMIC_BLOCK. When used, it will
    cause the ATOMIC_BLOCK to restore the previous state of the SREG
    register, saved before the Global Interrupt Status flag bit was
    disabled. The net effect of this is to make the ATOMIC_BLOCK's
    contents guaranteed atomic, without changing the state of the
    Global Interrupt Status flag when execution of the block
    completes.
*/
#if defined(__DOXYGEN__)
#define ATOMIC_RESTORESTATE
#else
#define ATOMIC_RESTORESTATE uint8_t sreg_save __attribute__((__cleanup__(__iRestore))) = SREG::read()
#endif	/* __DOXYGEN__ */

/** See avr/atomic.h

    This is a possible parameter for ATOMIC_BLOCK. When used, it will
    cause the ATOMIC_BLOCK to force the state of the SREG register on
    exit, enabling the Global Interrupt Status flag bit. This saves a
    small amout of flash space, a register, and one or more processor
    cycles, since the previous value of the SREG register does not need
    to be saved at the start of the block.

    Care should be taken that ATOMIC_FORCEON is only used when it is
    known that interrupts are enabled before the block's execution or
    when the side effects of enabling global interrupts at the block's
    completion are known and understood.
*/
#if defined(__DOXYGEN__)
#define ATOMIC_FORCEON
#else
#define ATOMIC_FORCEON uint8_t sreg_save __attribute__((__cleanup__(__iSeiParam))) = 0
#endif	/* __DOXYGEN__ */

/** See avr/atomic.h

    This is a possible parameter for NONATOMIC_BLOCK. When used, it
    will cause the NONATOMIC_BLOCK to restore the previous state of
    the SREG register, saved before the Global Interrupt Status flag
    bit was enabled. The net effect of this is to make the
    NONATOMIC_BLOCK's contents guaranteed non-atomic, without changing
    the state of the Global Interrupt Status flag when execution of
    the block completes.
*/
#if defined(__DOXYGEN__)
#define NONATOMIC_RESTORESTATE
#else
#define NONATOMIC_RESTORESTATE uint8_t sreg_save __attribute__((__cleanup__(__iRestore))) = SREG::read()
#endif	/* __DOXYGEN__ */

/** See avr/atomic.h

    This is a possible parameter for NONATOMIC_BLOCK. When used, it
    will cause the NONATOMIC_BLOCK to force the state of the SREG
    register on exit, disabling the Global Interrupt Status flag
    bit.  This saves a small amout of flash space, a register, and one
    or more processor cycles, since the previous value of the SREG
    register does not need to be saved at the start of the block.

    Care should be taken that NONATOMIC_FORCEOFF is only used when it
    is known that interrupts are disabled before the block's execution
    or when the side effects of disabling global interrupts at the
    block's completion are known and understood.
*/
#if defined(__DOXYGEN__)
#define NONATOMIC_FORCEOFF
#else
#define NONATOMIC_FORCEOFF uint8_t sreg_save __attribute__((__cleanup__(__iCliParam))) = 0
#endif	/* __DOXYGEN__ */

#endif