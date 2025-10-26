/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    cpu.h
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
 * @brief   cpu.h of AVR ATmega1284p mcu
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_cpu_
#define _H_cpu_

// --------------------------------------------------------------------------------

#include <stdint.h>
#include <string.h>

// --------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

// --------------------------------------------------------------------------------

#ifndef config_WATCHDOG_TIMEOUT_VALUE
#define config_WATCHDOG_TIMEOUT_VALUE       WDTO_8S
#endif

// --------------------------------------------------------------------------------

#define watchdog_enable()                   wdt_enable(config_WATCHDOG_TIMEOUT_VALUE);
#define watchdog()                          __asm__ __volatile__ ("wdr")

// --------------------------------------------------------------------------------

/*!
 * SLEEP_MODE_IDLE
 * SLEEP_MODE_ADC
 * SLEEP_MODE_PWR_DOWN
 * SLEEP_MODE_PWR_SAVE
 * SLEEP_MODE_STANDBY
 * SLEEP_MODE_EXT_STANDBY
 */

#ifndef config_SLEEP_MODE
#define config_SLEEP_MODE            SLEEP_MODE_PWR_SAVE
#endif

// --------------------------------------------------------------------------------

#define CPU_PREPARE_SLEEP_MODE()                            \
                    set_sleep_mode(config_SLEEP_MODE);      \
                    cli();                                  \
                    sleep_enable();                         \
                    sleep_bod_disable();                    \
                    sei()

#define CPU_ENTER_SLEEP_MODE()                              \
                    sleep_cpu()

#define CPU_DEACTIVATE_SLEEP_MODE()                         \
                    cli();                                  \
                    sleep_disable();                        \
                    sei()

// --------------------------------------------------------------------------------

#define ATOMIC_OPERATION(operation)                         \
                    cli();                                  \
                    operation                               \
                    sei();

// --------------------------------------------------------------------------------

typedef uint8_t u8;

typedef uint16_t u16;

typedef uint32_t u32;

typedef uint64_t u64;


typedef int8_t i8;

typedef int16_t i16;

typedef int32_t i32;


typedef uint_fast8_t ux8;

typedef uint_fast16_t ux16;

typedef uint_fast32_t ux32;

// --------------------------------------------------------------------------------

#ifndef __UNUSED__
#define __UNUSED__ __attribute__((unused))
#endif

// --------------------------------------------------------------------------------

#endif // _H_cpu_

// --------------------------------------------------------------------------------

