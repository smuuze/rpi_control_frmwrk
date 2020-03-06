/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "driver/timer/timer_configuration.h"
#include "driver/timer/timer0_driver.h"

//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

//-----------------------------------------------------------------------------

#define TIMER0_TCCR0A_COM0A1						(1 << 7)
#define TIMER0_TCCR0A_COM0A0						(1 << 6)
#define TIMER0_TCCR0A_COM0B1						(1 << 5)
#define TIMER0_TCCR0A_COM0B0						(1 << 4)

#define TIMER0_TCCR0B_CS02						(1 << 2)
#define TIMER0_TCCR0B_CS01						(1 << 1)
#define TIMER0_TCCR0B_CS00						(1 << 0)

#define TIMER0_TCCR0B_WGM2						(1 << 3)
#define TIMER0_TCCR0A_WGM1						(1 << 1)
#define TIMER0_TCCR0A_WGM0						(1 << 0)

#define TIMER0_TIFR0_OCF0A						(1 << 1)
#define TIMER0_TIFR0_OCF0B						(1 << 2)

#define TIMER0_CLEAR_COUNTER_ON_COMPARE_MATCH_A				(TIMER0_TCCR0A_WGM1)

#define TIMER0_OC0A_ON_COMPARE_MATCH_SET				(TIMER0_TCCR0A_COM0A1 | TIMER0_TCCR0A_COM0A0)
#define TIMER0_OC0A_ON_COMPARE_MATCH_CLEAR				TIMER0_TCCR0A_COM0A1
#define TIMER0_OC0A_ON_COMPARE_MATCH_TOGGLE				TIMER0_TCCR0A_COM0A0

#define TIMER0_TIMSK0_IE_COMPARE_MATCH_A				(1 << 1)
#define TIMER0_TIMSK0_IE_COMPARE_MATCH_B				(1 << 2)

#define TIMER0_CLOCK_SOURCE_MASK					(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS01 | TIMER0_TCCR0B_CS00)
#define TIMER0_CLOCK_SOURCE_NONE					(0)
#define TIMER0_CLOCK_SOURCE_CLK_IO					TIMER0_TCCR0B_CS00
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_8					TIMER0_TCCR0B_CS01
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_64				(TIMER0_TCCR0B_CS00 | TIMER0_TCCR0B_CS01)
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_256				TIMER0_TCCR0B_CS02
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_1024				(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS00)
#define TIMER0_CLOCK_SOURCE_CLK_T0_FALLING_EDGE				(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS01)
#define TIMER0_CLOCK_SOURCE_CLK_T0_RISING_EDGE				(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS01 | TIMER0_TCCR0B_CS00)

#define TIMER0_CLEAR_INTERRUPT_COMPARE_MATCH_A()			TIFR0 &= ~(TIMER0_TIFR0_OCF0A)
#define TIMER0_STOP_TIMER()						TCCR0B = 0; TCCR0A = 0; OCR0A = 0; OCR0B = 0; TCNT0 = 0

#define TIMER0_OCR0B_MAX_VALUE						255

//-----------------------------------------------------------------------------

static u8 TCCR0A_backup = 0;
static u8 TCCR0B_backup = 0;
static u8 OCR0A_backup = 0;
static u8 OCR0B_backup = 0;
static u8 TIMSK0_backup = 0;

//-----------------------------------------------------------------------------

static TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK p_irq_callback = 0;

//-----------------------------------------------------------------------------

extern void timer0_channela_irq_callback(void);

//-----------------------------------------------------------------------------

void timer0_driver_init(void) {

	DEBUG_PASS("timer0_driver_init()");
	
	timer0_driver_stop();
}

void timer0_driver_deinit(void) {

}

void timer0_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration) {
	
	timer0_driver_stop();

	switch (p_configuration->frequency) {

		default: /* no break */

		case TIMER_FREQUENCY_36kHz:
			DEBUG_PASS("timer0_driver_configure() - TIMER_FREQUENCY_36kHz");
			TCCR0B_backup |= TIMER0_CLOCK_SOURCE_CLK_IO;
			OCR0A_backup = 102;
			break;

		case TIMER_FREQUENCY_38kHz :
			DEBUG_PASS("timer0_driver_configure() - TIMER_FREQUENCY_38kHz");
			TCCR0B_backup |= TIMER0_CLOCK_SOURCE_CLK_IO;
			OCR0A_backup = 97;
			break;

		case TIMER_FREQUENCY_42kHz :
			DEBUG_PASS("timer0_driver_configure() - TIMER_FREQUENCY_42kHz");
			TCCR0B_backup |= TIMER0_CLOCK_SOURCE_CLK_IO;
			OCR0A_backup = 88;
			break;
	}

	switch (p_configuration->mode) {
		case TIMER_MODE_TIMER :
			TCCR0A_backup  = TIMER0_TCCR0A_COM0A0 | TIMER0_TCCR0A_WGM1 | TIMER0_TCCR0A_WGM0;
			TCCR0B_backup |= TIMER0_TCCR0B_WGM2;
			break;

		case TIMER_MODE_FREQUENCY:
			TCCR0A_backup = TIMER0_TCCR0A_COM0A0 | TIMER0_TCCR0A_WGM1 | TIMER0_TCCR0A_WGM0;
			TCCR0B_backup |= TIMER0_CLOCK_SOURCE_CLK_IO | TIMER0_TCCR0B_WGM2;
			break;
	}

	if (p_configuration->irq_callback != 0) {
		p_irq_callback = p_configuration->irq_callback;
	} else {
		p_irq_callback = 0;
	}
}

void timer0_driver_start(u32 time_us) {

	(void) time_us;
	DEBUG_PASS("timer0_driver_start()");

	TCNT0 = 0;
	TIMSK0 = TIMSK0_backup;
	OCR0A  = OCR0A_backup;
	OCR0B  = OCR0B_backup;
	TCCR0A = TCCR0A_backup;
	TCCR0B = TCCR0B_backup;
}

void timer0_driver_stop(void) {

	DEBUG_PASS("timer0_driver_stop()");
	TIMER0_STOP_TIMER();
}

ISR(TIMER0_COMPA_vect) {
	if (p_irq_callback != 0) {
		p_irq_callback();
	}
}

ISR(TIMER0_COMPB_vect) {
 }

ISR (TIMER0_OVF_vect) {

}