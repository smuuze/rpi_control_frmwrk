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
 * @file    rtc_driver_atmega1284p.c
 * @author  Sebastian Lesse
 * @date    2023 / 09 / 25
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "driver/rtc/rtc_driver_interface.h"

// --------------------------------------------------------------------------------

#ifndef config_RTC_TIMER_COUNTER_INTERVAL_MS
#define config_RTC_TIMER_COUNTER_INTERVAL_MS    5
#endif

// --------------------------------------------------------------------------------

#define RTC_TIMER_EXTERNAL_CLOCK_SOURCE         1

#define RTC_TIMER_RESET()                       TIMSK2 = 0;                         \
                                                TCNT2 = 0

#define RTC_TIMER_SET_CLOCK_SOURCE(source)      if (source == RTC_TIMER_EXTERNAL_CLOCK_SOURCE) {    \
                                                    ASSR =  (1 << AS2);             \
                                                } else {                            \
                                                                                    \
                                                }                                   \
                                                while (ASSR & (1 << TCR2BUB))

#define RTC_TIMER_SET_CLOCK_PRESCALER(pre)      while (ASSR & (1 << TCR2BUB));      \
                                                TCCR2B = pre

#define RTC_TIMER_SET_OPERATION_MODE(mode)      while (ASSR & (1 << TCR2AUB));      \
                                                TCCR2A = mode

#define RTC_TIMER_SET_COMPARE_A_VALUE(val)      while (ASSR & (1 << OCR2AUB));      \
                                                OCR2A = val

#define RTC_RESTART_TIMER()                     TCNT2 = 0

#define RTC_TIMER_SET_IRQ(mask)                 TIMSK2 = (mask)
#define RTC_TIMER_CLEAR_IRQ_FLAGS()             TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2)
#define RTC_TIMER_GET_COUNTER_VALUE()           TCNT2

#define RTC_TIMER_OPERATION_MODE_NORMAL         0

#define RTC_TIMER_INCREMENT_VALUE               1

// --------------------------------------------------------------------------------

#define RTC_TIMER_CLK_NO_CLOCK                  (0 << CS22) | (0 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_1               (0 << CS22) | (0 << CS21) | (1 << CS20)
#define RTC_TIMER_CLK_PRESCALER_8               (0 << CS22) | (1 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_32              (0 << CS22) | (1 << CS21) | (1 << CS20)
#define RTC_TIMER_CLK_PRESCALER_64              (1 << CS22) | (0 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_128             (1 << CS22) | (0 << CS21) | (1 << CS20)
#define RTC_TIMER_CLK_PRESCALER_256             (1 << CS22) | (1 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_1024            (1 << CS22) | (1 << CS21) | (1 << CS20)

// --------------------------------------------------------------------------------

#define RTC_TIMER_IRQ_COMPARE_A                 (1 << OCIE2A)
#define RTC_TIMER_IRQ_COMPARE_B                 (1 << OCIE2B)
#define RTC_TIMER_IRQ_OVERFLOW                  (1 << TOIE2)

// --------------------------------------------------------------------------------

/**
 * RTC-Clock            : 32.768 MHz
 * Prescaler            : 32
 * Wakeup-Period        : 250 ms
 *
 * Tick-CLK             : RTC-Clock / Prescaler = 1024
 * Ticks-Per-Period     : Tick-CLK / Wakeup-Period = 4,096
 *
 *
 * RTC Clock / PRE
 */
// RTC Clock 32.768 MHz

// --------------------------------------------------------------------------------

#define RTC_TIMER_PERIOD                        31
#define RTC_TIMER_TICKS_PER_PERIOD              32  /* 1000 / RTC_TIMER_PERIOD */
#define RTC_TIMER_BIT_SHIFT_TICKS_PER_PERIOD    5   /* log2(RTC_TIMER_TICKS_PER_PERIOD) */
#define RTC_TIMER_BIT_SHIFT_RTC_COUNTER_VALUE   5   /* log2(RTC_TIMER_PERIOD) */

//#define RTC_TIMER_PERIOD                      250
//#define RTC_TIMER_TICKS_PER_PERIOD            4   /* 1000 / RTC_TIMER_PERIOD */
//#define RTC_TIMER_BIT_SHIFT_TICKS_PER_PERIOD  2   /* log2(RTC_TIMER_TICKS_PER_PERIOD) */

// --------------------------------------------------------------------------------

/*!
 *
 */
static volatile u32 local_rtc_time = 0;

/*!
 *
 */
static volatile u8 local_rtc_ticker = 0;

// --------------------------------------------------------------------------------

/*
 *
 */
void rtc_driver_init(void) {

    // Prescaler
    // Compare-Register

    local_rtc_time = 0;
    local_rtc_ticker = 0;

    PASS(); // 1. Reset
    RTC_TIMER_RESET();

    PASS(); // 2. Select Clock-Source
    RTC_TIMER_SET_CLOCK_SOURCE(RTC_TIMER_EXTERNAL_CLOCK_SOURCE);

    PASS(); // 3. Switch to Async OP-Mode
    RTC_TIMER_SET_CLOCK_PRESCALER(RTC_TIMER_CLK_PRESCALER_32);
    RTC_TIMER_SET_OPERATION_MODE(RTC_TIMER_OPERATION_MODE_NORMAL);

    PASS(); // 4. Write new Configuration-Values
    RTC_TIMER_SET_COMPARE_A_VALUE(RTC_TIMER_PERIOD);

    PASS(); // 5. Enable IRQs
    RTC_TIMER_CLEAR_IRQ_FLAGS();
    RTC_TIMER_SET_IRQ(RTC_TIMER_IRQ_COMPARE_A);
}

/*
 *
 */
void rtc_driver_deinit(void) {

}

u8 local_rtc_timer_gettime_u8(void) {
    return (u8)local_rtc_timer_gettime_u32();
}

u16 local_rtc_timer_gettime_u16(void) {
    return (u16)local_rtc_timer_gettime_u32();
}

u32 local_rtc_timer_gettime_u32(void) {

    u32 rtc_time = local_rtc_time;
    u32 rtc_interval_ticker = (u32)local_rtc_ticker;
    u32 rtc_counter = (u32)RTC_TIMER_GET_COUNTER_VALUE();

    u32 actual_time = rtc_time;
    actual_time <<= RTC_TIMER_BIT_SHIFT_TICKS_PER_PERIOD;

    actual_time += rtc_interval_ticker;
    actual_time <<= RTC_TIMER_BIT_SHIFT_RTC_COUNTER_VALUE;

    actual_time += (u32)rtc_counter;

    return actual_time;
}

u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval) {
    return ((local_rtc_timer_gettime_u8() - time_reference) > time_interval) ? 1 : 0;
}

u8 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval) {
    return local_rtc_timer_gettime_u16() - time_reference > time_interval ? 1 : 0;
}

u8 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval) {
    return local_rtc_timer_gettime_u32() - time_reference > time_interval ? 1 : 0;
}

u8 rtc_timer_elapsed_u8(u8 time_reference) {
    return local_rtc_timer_gettime_u8() - time_reference;
}

u16 rtc_timer_elapsed_u16(u16 time_reference) {
    return local_rtc_timer_gettime_u16() - time_reference;
}

u32 rtc_timer_elapsed_u32(u32 time_reference) {
    return local_rtc_timer_gettime_u32() - time_reference;
}

// --------------------------------------------------------------------------------

/**
 * @see driver/rtc/rtc_interface.h#rtc_timer_get_usec
 */
u64 rtc_timer_get_usec(void) {
    return 0;
}

// --------------------------------------------------------------------------------

ISR(TIMER2_COMPA_vect) {

    RTC_RESTART_TIMER();

    if (++local_rtc_ticker == (RTC_TIMER_TICKS_PER_PERIOD - 1)) {

        local_rtc_time += RTC_TIMER_INCREMENT_VALUE;
        local_rtc_ticker = 0;
    }
}

// --------------------------------------------------------------------------------
