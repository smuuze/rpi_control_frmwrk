/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "driver/timer/timer_configuration.h"
#include "driver/timer/timer1_driver.h"

//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

//-----------------------------------------------------------------------------

#define TIMER1_TCCRnA_COMnA1						(1 << 7)
#define TIMER1_TCCRnA_COMnA0						(1 << 6)
#define TIMER1_TCCRnA_COMnB1						(1 << 5)
#define TIMER1_TCCRnA_COMnB0						(1 << 4)
#define TIMER1_TCCRnA_WGM1						(1 << 1)
#define TIMER1_TCCRnA_WGM0						(1 << 0)

#define TIMER1_TCCRnB_WGM3						(1 << 4)
#define TIMER1_TCCRnB_WGM2						(1 << 3)
#define TIMER1_TCCRnB_CS2						(1 << 2)
#define TIMER1_TCCRnB_CS1						(1 << 1)
#define TIMER1_TCCRnB_CS0						(1 << 0)

#define TIMER1_TIFRn_OCFnA						(1 << 1)
#define TIMER1_TIFRn_OCFnB						(1 << 2)

#define TIMER1_CLEAR_COUNTER_ON_COMPARE_MATCH_A				(TIMER1_TCCRnA_WGM1)

#define TIMER1_OCnA_ON_COMPARE_MATCH_SET				(TIMER1_TCCRnA_COM0A1 | TIMER1_TCCRnA_COMnA0)
#define TIMER1_OCnA_ON_COMPARE_MATCH_CLEAR				TIMER1_TCCRnA_COM0A1
#define TIMER1_OCnA_ON_COMPARE_MATCH_TOGGLE				TIMER1_TCCRnA_COM0A0

#define TIMER1_TIMSKn_IRQ_ENABLED					(1 << 5)
#define TIMER1_TIMSKn_IE_COMPARE_MATCH_A				(1 << 1)
#define TIMER1_TIMSKn_IE_COMPARE_MATCH_B				(1 << 2)

#define TIMER1_CLOCK_SOURCE_MASK					(TIMER1_TCCRnB_CS2 | TIMER1_TCCRnB_CS1 | TIMER1_TCCRnB_CS0)
#define TIMER1_CLOCK_SOURCE_NONE					(0)
#define TIMER1_CLOCK_SOURCE_CLK_IO					TIMER1_TCCRnB_CS0
#define TIMER1_CLOCK_SOURCE_NO_PRESCALER				(0)
#define TIMER1_CLOCK_SOURCE_CLK_IO_BY_8					TIMER1_TCCRnB_CS1
#define TIMER1_CLOCK_SOURCE_CLK_IO_BY_64				(TIMER1_TCCRnB_CS0 | TIMER1_TCCRnB_CS1)
#define TIMER1_CLOCK_SOURCE_CLK_IO_BY_256				TIMER1_TCCRnB_CS2
#define TIMER1_CLOCK_SOURCE_CLK_IO_BY_1024				(TIMER1_TCCRnB_CS2 | TIMER1_TCCRnB_CS0)
#define TIMER1_CLOCK_SOURCE_CLK_T0_FALLING_EDGE				(TIMER1_TCCRnB_CS2 | TIMER1_TCCRnB_CS1)
#define TIMER1_CLOCK_SOURCE_CLK_T0_RISING_EDGE				(TIMER1_TCCRnB_CS2 | TIMER1_TCCRnB_CS1 | TIMER1_TCCRnB_CS0)

#define TIMER1_CLEAR_INTERRUPT_COMPARE_MATCH_A()			TIFR0 &= ~(TIMER1_TIFR0_OCF0A)
#define TIMER1_STOP_TIMER()						TCCR1B = 0;

#define TIMER1_OCRnB_MAX_VALUE						255

#define TIMER1_CLEAR_IRQ_FLAG()						TIFR1 |= (1 << ICF1)

#define TIMER1_CLK_OP_TIME						0

//-----------------------------------------------------------------------------

static u8 TCCRA_backup = 0;
static u8 TCCRB_backup = 0;
static u8 TCCRC_backup = 0;
static u16 OCRA_backup = 0;
static u16 OCRB_backup = 0;
static u8 TIMSK_backup = 0;

static u32 run_time_us;
static u32 interval_time_us;

//-----------------------------------------------------------------------------

static TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK p_irq_callback = 0;

//-----------------------------------------------------------------------------

void timer1_driver_init(void) {

	DEBUG_PASS("timer1_driver_init()");
	
	timer1_driver_stop();
}

void timer1_driver_deinit(void) {

}

void timer1_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration) {
	
	timer1_driver_stop();

	TCCRA_backup = 0;
	TCCRB_backup = 0;
	TCCRC_backup = 0;
	OCRA_backup = 0;
	OCRB_backup = 0;
	TIMSK_backup = 0;

	if (p_configuration->mode == TIMER_MODE_TIMER) {

		DEBUG_PASS("timer1_driver_configure() - TIMER_MODE_TIMER");

		p_configuration->frequency = TIMER_FREQUENCY_NONE;
		TCCRA_backup = 0;
		TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;

		switch (p_configuration->time_interval) {

			default: 
				DEBUG_TRACE_byte(p_configuration->time_interval, "timer1_driver_configure() - INTERVAL UNSUPPORTED !!!");
				/* no break */

			case TIMER_TIME_INTERVAL_250ms :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_250ms");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO_BY_1024;
				OCRA_backup = 1800;
				interval_time_us = 250000;
				break;

			case TIMER_TIME_INTERVAL_1ms :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_1ms");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO_BY_64;
				OCRA_backup = 115;
				interval_time_us = 1000;
				break;

			case TIMER_TIME_INTERVAL_600us :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_600us");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				//TCCRB_backup |= TIMER1_CLOCK_SOURCE_NO_PRESCALER;
				OCRA_backup = 4450;
				interval_time_us = 600;
				break;

			case TIMER_TIME_INTERVAL_560us :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_560us");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				//TCCRB_backup |= TIMER1_CLOCK_SOURCE_NO_PRESCALER;
				OCRA_backup = 4160;
				interval_time_us = 560;
				break;

			case TIMER_TIME_INTERVAL_527us :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_527us");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				//TCCRB_backup |= TIMER1_CLOCK_SOURCE_NO_PRESCALER;
				OCRA_backup = 3868;
				interval_time_us = 527;
				break;

			case TIMER_TIME_INTERVAL_80us :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_80us");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				//TCCRB_backup |= TIMER1_CLOCK_SOURCE_NO_PRESCALER;
				//OCRA_backup = 460;
				OCRA_backup = 569;
				interval_time_us = 80;
				break;

			case TIMER_TIME_INTERVAL_40us :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_40us");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				//TCCRB_backup |= TIMER1_CLOCK_SOURCE_NO_PRESCALER;
				//OCRA_backup = 460;
				OCRA_backup = 285;
				interval_time_us = 40;
				break;

			case TIMER_TIME_INTERVAL_10us :
				DEBUG_PASS("timer1_driver_configure() - TIMER_TIME_INTERVAL_10us");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				//TCCRB_backup |= TIMER1_CLOCK_SOURCE_NO_PRESCALER;
				OCRA_backup = 71;
				interval_time_us = 10;
				break;
		}

		TCCRB_backup |= TIMER1_TCCRnB_WGM2;
		TIMSK_backup |= TIMER1_TIMSKn_IRQ_ENABLED | TIMER1_TIMSKn_IE_COMPARE_MATCH_A;

	} else if (p_configuration->mode == TIMER_MODE_FREQUENCY) {

		DEBUG_PASS("timer1_driver_configure() - TIMER_MODE_FREQUENCY");

		TCCRA_backup = TIMER1_TCCRnA_COMnA0 | TIMER1_TCCRnA_WGM1 | TIMER1_TCCRnA_WGM0;
		TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO | TIMER1_TCCRnB_WGM2;

		switch (p_configuration->frequency) {

			default: 
				DEBUG_TRACE_byte(p_configuration->frequency, "timer1_driver_configure() - FREQUENCY UNSUPPORTED !!!");
				break;

			case TIMER_FREQUENCY_36kHz:
				DEBUG_PASS("timer1_driver_configure() - TIMER_FREQUENCY_36kHz");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				OCRA_backup = 102;
				break;

			case TIMER_FREQUENCY_38kHz :
				DEBUG_PASS("timer1_driver_configure() - TIMER_FREQUENCY_38kHz");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				OCRA_backup = 97;
				break;

			case TIMER_FREQUENCY_40kHz :
				DEBUG_PASS("timer1_driver_configure() - TIMER_FREQUENCY_40kHz");
				OCRA_backup = 91;
				break;

			case TIMER_FREQUENCY_42kHz :
				DEBUG_PASS("timer1_driver_configure() - TIMER_FREQUENCY_42kHz");
				TCCRB_backup |= TIMER1_CLOCK_SOURCE_CLK_IO;
				OCRA_backup = 88;
				break;
		}

	}

	if (p_configuration->irq_callback != 0) {
		DEBUG_PASS("timer0_driver_configure() - IRQ callback set");
		p_irq_callback = p_configuration->irq_callback;
	} else {
		p_irq_callback = 0;
	}
}

void timer1_driver_start(u32 time_us) {

	DEBUG_TRACE_long(time_us, "timer1_driver_start()");

	run_time_us = time_us;

	TCNT1L = 0;
	TCNT1H = 0;

	TIMSK1 = TIMSK_backup;
	
	OCR1AH  = (u8)(OCRA_backup >> 8);
	OCR1AL  = (u8)OCRA_backup;

	OCR1BH  = (u8)(OCRB_backup >> 8);
	OCR1BL  = (u8)OCRB_backup;

	TCCR1A = TCCRA_backup;
	TCCR1B = TCCRB_backup;
	TCCR1C = TCCRC_backup;
}

void timer1_driver_stop(void) {

	DEBUG_PASS("timer1_driver_stop()");
	TIMER1_STOP_TIMER();
}

ISR(TIMER1_COMPA_vect) {

	if (run_time_us == TIME_CONFIGURATION_RUN_FOREVER) {

		if (p_irq_callback != 0) {
			p_irq_callback();
		}

		return;
	}

	if (run_time_us > interval_time_us) {
		run_time_us -= (interval_time_us + TIMER1_CLK_OP_TIME);
	} else {
		run_time_us = 0;
	}

	if (run_time_us == 0 && p_irq_callback != 0) {
		
		if (p_irq_callback != 0) {
			p_irq_callback();
		}
	}
}

ISR(TIMER1_COMPB_vect) {

}

ISR (TIMER1_OVF_vect) {

}
