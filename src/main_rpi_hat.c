/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"
//#include RTOS_H
//#include <fakertos.h>

#include <ctype.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include "initialization.h"
#include "mcu_task_controller.h"

#ifdef HAS_APP_TASK_DEBUS
#include "local_debus_mcu_task.h"
#endif

//---------- Implementation of Traces -----------------------------------------

//#define TRACES
//#include <traces.h>

#define TRACER_OFF
#include "tracer.h"

#ifdef TRACES_ENABLED
#define MAIN_TRACES_ENABLED
#define INIT_TRACES_ENABLED
#endif

#ifdef MAIN_TRACES_ENABLED
#define MAIN_PASS()			PASS()
#define MAIN_TRACE_byte(byte)		TRACE_byte(byte)
#define MAIN_TRACE_word(word)		TRACE_word(word)
#define MAIN_TRACE_long(integer)	TRACE_long(integer)
#define MAIN_TRACE_N(num,byte)		TRACE_N(num,byte)
#else
#define MAIN_PASS()
#define MAIN_TRACE_byte(byte)
#define MAIN_TRACE_word(byte)
#define MAIN_TRACE_long(integer)
#define MAIN_TRACE_N(num,byte)
#endif

#ifdef INIT_TRACES_ENABLED
#define INIT_PASS()			PASS()
#define INIT_TRACE_byte(byte)		TRACE_byte(byte)
#define INIT_TRACE_word(word)		TRACE_word(word)
#define INIT_TRACE_long(integer)	TRACE_long(integer)
#define INIT_TRACE_N(num,byte)		TRACE_N(num,byte)
#else
#define INIT_PASS()
#define INIT_TRACE_byte(byte)
#define INIT_TRACE_word(byte)
#define INIT_TRACE_long(integer)
#define INIT_TRACE_N(num,byte)
#endif

#ifndef  RTOS_TIMER

config_FAKERTOS_TASKYIELD_FCT_PROTO

void task_yield(void) {
	config_FAKERTOS_TASKYIELD_FUNCTION
}

#endif

void main_init(void) {

	INIT_PASS(); // Initialization - First-Time Debus-Init

	//INIT_PORTS_FOR_STARTUP();

	#if defined HAS_APP_TASK_DEBUS && HAS_APP_TASK_DEBUS != 0
	{
		// Baudrate erstmal auf 9600 Baud setzen
		INIT_PASS(); // Initialization - First-Time Debus-Init
		debus_host_com_init();
	}
	#endif

	#ifdef  RTOS_TIMER
	{
		// Millisekunden timer initialieren
		INIT_PASS(); // Initialization - fakertos_timer_init
		fakertos_timer_init();
	}
	#endif  // #ifdef RTOS_TIMER

	cli();
	initialization();
	sei();

	wdt_enable(WDTO_8S);
	watchdog();

	INIT_PASS(); // Initialization done
}

int main( void ) {

	main_init();

	for (;;) {  // Endlosschleife

		mcu_task_controller_schedule();
		task_yield();
		watchdog();
	}
}
