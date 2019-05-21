#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

#define watchdog()              __asm__ __volatile__ ("wdr")

typedef uint8_t u8;

typedef uint16_t u16;

typedef uint32_t u32;


typedef int8_t i8;

typedef int16_t i16;

typedef int32_t i32;


typedef uint_fast8_t ux8;

typedef uint_fast16_t ux16;

typedef uint_fast32_t ux32;

#endif // _CPU_H_
