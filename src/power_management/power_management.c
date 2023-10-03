/*! 
 * -----------------------------------------------------------------------------
 *
 * @file	power_management.c
 * @brief
 * @author	sebastian lesse
 *
 * -----------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "time_management/time_management.h"
#include "power_management/power_management_interface.h"
#include "mcu_task_management/mcu_task_interface.h"
#include "common/local_module_status.h"
#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

//-----------------------------------------------------------------------------

#ifndef POWER_MNGNMT_TASK_SCHEDULE_INTERVAL_MS
#define POWER_MNGNMT_TASK_SCHEDULE_INTERVAL_MS			30
#endif

//-----------------------------------------------------------------------------

#define POWER_MGNT_STATUS_ACTION_PENDING			( 1 << 0)

BUILD_MODULE_STATUS_U8(POWER_MGNT_STATUS)

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(POWER_MGMNT_SCHEDULE_TIMER)

//-----------------------------------------------------------------------------

/**
 * @brief Pointer to the first unit
 * 
 */
static POWER_MANAGEMENT_UNIT_TYPE* p_first_unit = 0;

/**
 * @brief always the last module of all known power-management unit
 * 
 */
static POWER_MANAGEMENT_UNIT_TYPE* p_last_unit = 0;

//-----------------------------------------------------------------------------

static void power_management_task_start(void);
static u16 power_management_task_get_schedule_interval(void);
static MCU_TASK_INTERFACE_TASK_STATE power_management_task_get_state(void);
static void power_management_task_execute(void);
static void power_management_task_terminate(void);

TASK_CREATE(
    POWER_MANAGEMENT_TASK,
    TASK_PRIORITY_VERY_MIDDLE,
    power_management_task_get_schedule_interval,
    power_management_task_start,
    power_management_task_execute,
    power_management_task_get_state,
    power_management_task_terminate
)

//-----------------------------------------------------------------------------

void power_mgmnt_init(void) {

	DEBUG_PASS("power_mgmnt_init() - START");

    POWER_MANAGEMENT_TASK_init();

	DEBUG_PASS("power_mgmnt_init() - DONE");
}

//-----------------------------------------------------------------------------

void power_mgmnt_unit_init(POWER_MANAGEMENT_UNIT_TYPE* p_unit, u16 power_up_time, u16 power_down_time, POWER_MANAGEMENT_POWER_ON_CALLBACK p_callback_on, POWER_MANAGEMENT_POWER_OFF_CALLBACK p_callback_off) {

	if (p_unit->status.is_initialized != 0) {
		DEBUG_PASS("power_mgmnt_unit_init() - Unit is already initialized");
		return;
	}

	DEBUG_PASS("power_mgmnt_unit_init() - START");

	if (p_first_unit == 0) {

		DEBUG_PASS("power_mgmnt_unit_init() - This is the first unit");

		p_first_unit = p_unit;
		p_last_unit = p_unit;

	} else {

		DEBUG_PASS("power_mgmnt_unit_init() - Registering another unit");

		p_last_unit->_next = p_unit;
		p_last_unit = p_last_unit->_next;
	}

	p_last_unit->_next = 0;

	p_unit->request_counter = 0;
	
	p_unit->power_up_time_ms = power_up_time;
	p_unit->power_down_time_ms = power_down_time;

	p_unit->request_counter = 0;
	p_unit->switch_timestamp = 0;

	p_unit->on = p_callback_on;
	p_unit->off = p_callback_off;

	p_unit->status.is_on = 0;
	p_unit->status.is_ramp_up = 0;
	p_unit->status.is_ramp_down = 0;

	p_unit->off();

	DEBUG_PASS("power_mgmnt_unit_init() - DONE");
}

void power_mgmnt_unit_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {
	
	if (p_unit->request_counter == 0) {
		
		if (p_unit->status.is_ramp_down == 0) {
			
			DEBUG_PASS("power_mgmnt_unit_request() - Going to switch on unit\n");

			p_unit->on();
			p_unit->status.is_ramp_up = 1;
			p_unit->switch_timestamp = time_mgmnt_gettime_u16();
		
			DEBUG_TRACE_word(p_unit->switch_timestamp, "power_mgmnt_unit_request() - Timestamp: \n");
			DEBUG_TRACE_word(p_unit->power_up_time_ms, "power_mgmnt_unit_request() - Timeout: \n");
		}

		POWER_MGNT_STATUS_set(POWER_MGNT_STATUS_ACTION_PENDING);

	} else {
		//DEBUG_PASS("power_mgmnt_unit_request() - Unit was requested before");
	}
	
	if (p_unit->request_counter < POWER_MANAGEMENT_MAX_REQUEST_COUNT_VALUE) {
		p_unit->request_counter += 1;
	} else {
		DEBUG_PASS("power_mgmnt_unit_request() - Maximum Reques-Count reached");
	}
}

void power_mgmnt_unit_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {

	//DEBUG_PASS("power_mgmnt_unit_release()");
	
	if (p_unit->request_counter == 0) {

		DEBUG_PASS("power_mgmnt_unit_release() - Unit was not requested");
		return;		
	} 
	
	if (p_unit->request_counter == 1) {

		DEBUG_PASS("power_mgmnt_unit_release() - The is the last release");
		
		p_unit->status.is_ramp_down = 1;

		POWER_MGNT_STATUS_set(POWER_MGNT_STATUS_ACTION_PENDING);
	}
	
	p_unit->request_counter -= 1;
}

u8 power_mgmnt_unit_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {

	if (p_unit->status.is_on || p_unit->status.is_ramp_down) {
		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------

static void power_management_task_start(void) {

	DEBUG_PASS("power_management_task_start()");
	POWER_MGMNT_SCHEDULE_TIMER_start();
}

static u16 power_management_task_get_schedule_interval(void) {

	if (POWER_MGNT_STATUS_is_set(POWER_MGNT_STATUS_ACTION_PENDING)) {
		return 0;

	} else {
		return POWER_MNGNMT_TASK_SCHEDULE_INTERVAL_MS;
	}
}

static MCU_TASK_INTERFACE_TASK_STATE power_management_task_get_state(void) {

	if (POWER_MGNT_STATUS_is_set(POWER_MGNT_STATUS_ACTION_PENDING)) {
		DEBUG_PASS("power_management_task_get_state() - Task is active");
		POWER_MGMNT_SCHEDULE_TIMER_start();
		return MCU_TASK_RUNNING;
	}

	if (POWER_MGMNT_SCHEDULE_TIMER_is_up(POWER_MNGNMT_TASK_SCHEDULE_INTERVAL_MS)) {
		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

static void power_management_task_execute(void) {

	POWER_MANAGEMENT_UNIT_TYPE* p_act_unit = p_first_unit;
	
	while (p_act_unit != 0) {

		if (p_act_unit->status.is_ramp_up) {

			if (time_mgmnt_istimeup_raw_u16(p_act_unit->switch_timestamp, p_act_unit->power_up_time_ms)) {

				DEBUG_PASS("power_management_task_execute() - unit changed status from RAMP-UP to IS-ON");
				p_act_unit->status.is_on = 1;
				p_act_unit->status.is_ramp_up = 0;

			}

		} else if (p_act_unit->status.is_on) {

			if (p_act_unit->status.is_ramp_down) {

				DEBUG_PASS("power_management_task_execute() - unit changed status from IS-ON to RAMP-DOWN");
				p_act_unit->status.is_on = 0;
				p_act_unit->switch_timestamp = time_mgmnt_gettime_u16();
			}

		} else if (p_act_unit->status.is_ramp_down) {

			if (p_act_unit->request_counter != 0) {

				DEBUG_PASS("power_management_task_execute() - unit changed status from RAMP-DOWN to IS_ON");
				p_act_unit->status.is_ramp_down = 0;
				p_act_unit->status.is_on = 1;

			} else if (time_mgmnt_istimeup_raw_u16(p_act_unit->switch_timestamp, p_act_unit->power_down_time_ms)) {

				DEBUG_PASS("power_management_task_execute() - unit changed status from RAMP-DOWN to OFF");
				p_act_unit->status.is_ramp_down = 0;
				p_act_unit->off();
			}
		} 

		p_act_unit = p_act_unit->_next;
	}

	POWER_MGNT_STATUS_unset(POWER_MGNT_STATUS_ACTION_PENDING);
}

static void power_management_task_terminate(void) {

}
