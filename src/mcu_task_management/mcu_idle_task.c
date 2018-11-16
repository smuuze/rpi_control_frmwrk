/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include <avr/sleep.h>
#include <avr/wdt.h>

#include "system_interface.h"

#ifndef config_SLEEP_MODE
#define config_SLEEP_MODE	SLEEP_MODE_IDLE
#endif

#define noTRACES
#include <traces.H>

void mcu_idle_task_init(void) {
	PASS(); // mcu_idle_task_init()
}

u8 mcu_idle_task_is_runable(void) {
	return 1;
}

void mcu_idle_task_run(void) {

	set_sleep_mode(config_SLEEP_MODE);

	cli();
	sleep_enable();
	sleep_bod_disable();
	sei();
	sleep_cpu();

	sleep_disable();
}

void mcu_idle_task_background_run(void) {

}

void mcu_idle_task_sleep(void) {

}

void mcu_idle_task_wakeup(void) {

}

void mcu_idle_task_finish(void) {

}

void mcu_idle_task_terminate(void) {

}
