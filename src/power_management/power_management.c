/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "time_management/time_management.h"
#include "power_management/power_management.h"
#include "mcu_task_management/mcu_task_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//-----------------------------------------------------------------------------


/*!
 *
 */
//static MCU_TASK_INTERFACE_TASK_STATE task_state = MCU_TASK_TERMINATED;

//static POWER_MANAGEMENT_UNIT_TYPE* p_first_unit = 0;
//static POWER_MANAGEMENT_UNIT_TYPE* p_last_unit = 0;

//-----------------------------------------------------------------------------


void power_mgmnt_init(POWER_MANAGEMENT_UNIT_TYPE* p_unit, u16 power_up_time, POWER_MANAGEMENT_POWER_ON_CALLBACK p_callback_on, POWER_MANAGEMENT_POWER_OFF_CALLBACK p_callback_off) {
/*
	if (p_first_unit == 0) {
		p_first_unit = p_unit;
	}

	p_last_unit->next = p_unit;
	p_last_unit = p_last_unit->next;
	p_last_unit->next = 0;
*/

	DEBUG_PASS("power_mgmnt_init()");

	p_unit->request_counter = 0;
	
	p_unit->power_up_time_ms = power_up_time;
	p_unit->request_counter = 0;
	p_unit->switch_on_timestamp = 0;
	p_unit->on = p_callback_on;
	p_unit->off = p_callback_off;
	p_unit->status.is_on = 0;
	p_unit->status.is_ramp_up = 0;
}

void power_mgmnt_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {

	DEBUG_PASS("power_mgmnt_request()");
	
	if (p_unit->request_counter == 0) {
		DEBUG_PASS("power_mgmnt_request() - Going to switch on unit\n");
		
		p_unit->on();
		p_unit->status.is_ramp_up = 1;
		p_unit->switch_on_timestamp = time_mgmnt_gettime_u16();
		
		DEBUG_TRACE_word(p_unit->switch_on_timestamp, "power_mgmnt_request() - Timestamp: \n");
		DEBUG_TRACE_word(p_unit->power_up_time_ms, "power_mgmnt_request() - Timeout: \n");
	}
	
	p_unit->request_counter += 1;
}

/*!
 *
 */
void power_mgmnt_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {	

	DEBUG_PASS("power_mgmnt_release()");
	
	if (p_unit->request_counter == 0) {

		DEBUG_PASS("power_mgmnt_release() - Unit was not requested");
		return;		
	} 
	
	if (p_unit->request_counter == 1) {

		DEBUG_PASS("power_mgmnt_release() - The is the last release");
		
		p_unit->off();
		p_unit->status.is_on = 0;
	}
	
	p_unit->request_counter -= 1;
}

/*!
 *
 */
u8 power_mgmnt_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {

	if (p_unit->status.is_ramp_up == 1) {
	
		if (time_mgmnt_istimeup_u16(p_unit->switch_on_timestamp, p_unit->power_up_time_ms) == 0) {
			return 0;
		}
			
		p_unit->status.is_on = 1;
		p_unit->status.is_ramp_up = 0;
	}

	return (p_unit->status.is_on != 0) ? 1 : 0;
}


// ---- Task Interface Implementation ------------------------------------------------------------

/*!
 *
 */
void power_management_task_init(void) {
/*
	operation_stage = PCA9670_TASK_STATE_IDLE;
*/
}

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE power_management_task_get_state(void) {

/*
	if (task_state == MCU_TASK_SLEEPING) {

		if (task_timer_is_up(PCA9670_TASK_RUN_INTERVAL_MS) != 0) {
			task_state = MCU_TASK_RUNNING;
		}
	}

	return task_state;
*/
	return MCU_TASK_SLEEPING;
}

/*!
 *
 */
void power_management_task_run(void) {
/*	
	POWER_MANAGEMENT_UNIT_TYPE* p_act_unit = p_last_unit;
	
	while (p_act_unit != 0) {
	
		if (p_act_unit->status.is_ramp_up != 0) {
		
			if () {
			
			}
		}
	
	}
	*/
}

/*!
 *
 */
void power_management_task_background_run(void) {

}

/*!
 *
 */
void power_management_task_sleep(void) {

}

/*!
 *
 */
void power_management_task_wakeup(void) {

}

/*!
 *
 */
void power_management_task_finish(void) {

}

/*!
 *
 */
void power_management_task_terminate(void) {

}

