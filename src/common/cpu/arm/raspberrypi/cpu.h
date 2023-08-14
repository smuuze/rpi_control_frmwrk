/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	rpi_arm/cpu.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _CPU_H_
#define _CPU_H_

// --------------------------------------------------------------------------------

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------------------

#ifndef config_WATCHDOG_TIMEOUT_VALUE
#define config_WATCHDOG_TIMEOUT_VALUE		
#endif

// --------------------------------------------------------------------------------

#define watchdog_enable()			do{}while(0)
#define watchdog()				do{}while(0)

// --------------------------------------------------------------------------------

#ifndef config_SLEEP_MODE
#define config_SLEEP_MODE
#endif

// --------------------------------------------------------------------------------

#define CPU_PREPARE_SLEEP_MODE()		do{}while(0)	
#define CPU_ENTER_SLEEP_MODE()			usleep(5000);
#define CPU_DEACTIVATE_SLEEP_MODE()		do{}while(0)

// --------------------------------------------------------------------------------

#define ATOMIC_OPERATION(operation)		operation

// --------------------------------------------------------------------------------

typedef uint8_t u8;

typedef uint16_t u16;

typedef uint32_t u32;


typedef int8_t i8;

typedef int16_t i16;

typedef int32_t i32;

typedef uint64_t u64;


typedef uint8_t ux8;

typedef uint16_t ux16;

typedef uint32_t ux32;

typedef int64_t i64;

// --------------------------------------------------------------------------------

#ifndef __UNUSED__
#define __UNUSED__ __attribute__((unused))
#endif

// --------------------------------------------------------------------------------

#endif // _CPU_H_
