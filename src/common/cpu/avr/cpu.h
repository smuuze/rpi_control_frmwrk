#ifndef _CPU_H_
#define _CPU_H_

//-----------------------------------------------------------------------------

#include <stdint.h>
#include <string.h>

//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

//-----------------------------------------------------------------------------

#ifndef config_WATCHDOG_TIMEOUT_VALUE
#define config_WATCHDOG_TIMEOUT_VALUE		WDTO_8S
#endif

//-----------------------------------------------------------------------------

#define watchdog_enable()			wdt_enable(config_WATCHDOG_TIMEOUT_VALUE);
#define watchdog()             			 __asm__ __volatile__ ("wdr")

//-----------------------------------------------------------------------------

/*!
 * SLEEP_MODE_IDLE
 * SLEEP_MODE_ADC
 * SLEEP_MODE_PWR_DOWN
 * SLEEP_MODE_PWR_SAVE
 * SLEEP_MODE_STANDBY
 * SLEEP_MODE_EXT_STANDBY
 */

#ifndef config_SLEEP_MODE
#define config_SLEEP_MODE			SLEEP_MODE_PWR_SAVE
#endif

//-----------------------------------------------------------------------------

#define CPU_PREPARE_SLEEP_MODE()		set_sleep_mode(config_SLEEP_MODE);	\
						cli();					\
						sleep_enable();				\
						sleep_bod_disable();			\
						sei()		
#define CPU_ENTER_SLEEP_MODE()			sleep_cpu()
#define CPU_DEACTIVATE_SLEEP_MODE()		cli();					\
						sleep_disable();			\
						sei()

//-----------------------------------------------------------------------------

#define ATOMIC_OPERATION(operation)		cli();		\
						operation	\
						sei()

//-----------------------------------------------------------------------------	

typedef uint8_t u8;

typedef uint16_t u16;

typedef uint32_t u32;


typedef int8_t i8;

typedef int16_t i16;

typedef int32_t i32;


typedef uint_fast8_t ux8;

typedef uint_fast16_t ux16;

typedef uint_fast32_t ux32;

//-----------------------------------------------------------------------------

#endif // _CPU_H_
