/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static volatile u32 local_rtc_timer = 0;

/*
 *
 */
void local_rtc_timer_init(void) {

	// 1. disable IRQ for Timer2
	
	// 2. Select Clock-SOurce
	
	// 3. Write new Configuration-Values
	
	// 4. Switch to Async OP-Mode
	
	// 5. CLear IRQ FLags
	
	// 6. Enable IRQ

	// Output Compare Register A
	OCR2A = 0;
	
	// Select Clock-Source
	ASSR = (1 << EXCLK) | (1 << AS2);
	
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
	
	TCNT2 = 0;
	
	TIMSK2 = (1 << OCIE2A);
}

/*
 *
 */
void local_rtc_timer_deinit(void) {

}

u8 local_rtc_timer_gettime_u8(void) {
	return (u8)local_rtc_timer;
}

u16 local_rtc_timer_gettime_u16(void) {
	return (u16)local_rtc_timer;
}

u32 local_rtc_timer_gettime_u32(void) {
	return local_rtc_timer;
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

ISR(TIMER2_COMPA) {

}
