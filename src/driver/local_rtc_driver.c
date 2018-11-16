/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "local_rtc_driver.h"

#define TRACES
#include <traces.h>

#ifndef config_RTC_TIMER_COUNTER_INTERVAL_MS
#define config_RTC_TIMER_COUNTER_INTERVAL_MS	5
#endif

#define RTC_TIMER_EXTERNAL_CLOCK_SOURCE		1

#define RTC_TIMER_RESET()			TIMSK2 = 0; 						\
						TCNT2 = 0

#define RTC_TIMER_SET_CLOCK_SOURCE(source)	if (source == RTC_TIMER_EXTERNAL_CLOCK_SOURCE) {	\
							ASSR =  (1 << AS2);				\
						} else {						\
													\
						}							\
						while (ASSR & (1 << TCR2BUB))

#define RTC_TIMER_SET_CLOCK_PRESCALER(pre)	TCCR2B = pre;						\
						while (ASSR & (1 << TCR2BUB))

#define RTC_TIMER_SET_OPERATION_MODE(mode)	TCCR2A = mode;						\
						while (ASSR & (1 << TCR2AUB))

#define RTC_TIMER_SET_COMPARE_A_VALUE(val)	OCR2A = val;						\
						while (ASSR & (1 << OCR2AUB))

#define RTC_RESTART_TIMER()			TCNT2 = 0

#define RTC_TIMER_SET_IRQ(mask)			TIMSK2 = (mask)
#define RTC_TIMER_CLEAR_IRQ_FLAGS()		TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2)
#define RTC_TIMER_GET_COUNTER_VALUE()		TCNT2

#define RTC_TIMER_OPERATION_MODE_NORMAL		0

#define RTC_TIMER_INCREMENT_VALUE		1

#define RTC_TIMER_CLK_NO_CLOCK			(0 << CS22) | (0 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_1		(0 << CS22) | (0 << CS21) | (1 << CS20)
#define RTC_TIMER_CLK_PRESCALER_8		(0 << CS22) | (1 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_32		(0 << CS22) | (1 << CS21) | (1 << CS20)
#define RTC_TIMER_CLK_PRESCALER_64		(1 << CS22) | (0 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_128		(1 << CS22) | (0 << CS21) | (1 << CS20)
#define RTC_TIMER_CLK_PRESCALER_256		(1 << CS22) | (1 << CS21) | (0 << CS20)
#define RTC_TIMER_CLK_PRESCALER_1024		(1 << CS22) | (1 << CS21) | (1 << CS20)

#define RTC_TIMER_IRQ_COMPARE_A			(1 << OCIE2A)
#define RTC_TIMER_IRQ_COMPARE_B			(1 << OCIE2B)
#define RTC_TIMER_IRQ_OVERFLOW			(1 << TOIE2)

#define RTC_TIMER_PERIOD			255
#define RTC_TIMER_TICKS_PER_PERIOD		4

/*!
 *
 */
static volatile u32 local_rtc_time = 0;

/*!
 *
 */
static volatile u8 local_rtc_ticker = 0;

/*
 *
 */
void local_rtc_timer_init(void) {

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
void local_rtc_timer_deinit(void) {

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
	actual_time <<= 2;

	actual_time += rtc_interval_ticker;
	actual_time <<= 8;

	actual_time += (u32)rtc_counter;

	return actual_time;
}

u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval) {
	return local_rtc_timer_gettime_u8() - time_reference > time_interval ? 1 : 0;
}

u16 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval) {
	return local_rtc_timer_gettime_u16() - time_reference > time_interval ? 1 : 0;
}

u32 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval) {
	return local_rtc_timer_gettime_u32() - time_reference > time_interval ? 1 : 0;
}

ISR(TIMER2_COMPA_vect) {

	//PASS(); // ISR(TIMER2_OVF_vect) -------
	RTC_RESTART_TIMER();

	if (local_rtc_ticker++ == RTC_TIMER_TICKS_PER_PERIOD - 1) {

		local_rtc_time += RTC_TIMER_INCREMENT_VALUE;
		local_rtc_ticker = 0;
	}

}
