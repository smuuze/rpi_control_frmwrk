/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "system_interface.h"
#include "mcu_task_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#define SLEEP_DRIVER_PRR_ALL_ON		(0)
#define SLEEP_DRIVER_PRR_TWI		(1 << PRTWI)
#define SLEEP_DRIVER_PRR_TIMER2		(1 << PRTIM2)
#define SLEEP_DRIVER_PRR_TIMER0		(1 << PRTIM0)
#define SLEEP_DRIVER_PRR_USART0		(1 << PRUSART0)
#define SLEEP_DRIVER_PRR_TIMER1		(1 << PRTIM1)
#define SLEEP_DRIVER_PRR_SPI		(1 << PRSPI)
#define SLEEP_DRIVER_PRR_USART1		(1 << PRUSART0)
#define SLEEP_DRIVER_PRR_ADC		(1 << PRADC)

#define SET_MODULE_POWER_SAFE(prr_module_mask)	PRR0 |= (prr_module_mask)

void mcu_idle_task_init(void) {
	PASS(); // mcu_idle_task_init()
}

u16 mcu_idle_task_get_schedule_interval(void) {
	return 0;
}

MCU_TASK_INTERFACE_TASK_STATE mcu_idle_task_is_runable(void) {
	return MCU_TASK_RUNNING;
}

void mcu_idle_task_run(void) {

	#ifndef DISABLE_SLEEP_ON_IDLE
	{
		CPU_PREPARE_SLEEP_MODE();
		CPU_ENTER_SLEEP_MODE();
		CPU_DEACTIVATE_SLEEP_MODE();
	}
	#endif
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
