/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_task/copro_routing_mcu_task.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "common/local_module_status.h"

#include "mcu_task_management/mcu_task_interface.h"

#include "time_management/time_management.h"
#include "driver/timer/timer_interface.h"

#include "copro/copro_interface.h"

// --------------------------------------------------------------------------------

#define COPRO_ROUTING_TASK_RUN_INTERVAL_MS		0
#define COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS		500
#define COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE	64

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	COPRO_ROUTING_TASK_STATE_IDLE,
	COPRO_ROUTING_TASK_STATE_PREPARE,
	COPRO_ROUTING_TASK_STATE_PROCESS,
	COPRO_ROUTING_TASK_STATE_FINISH,
	COPRO_ROUTING_TASK_TIMEOUT
} COPRO_ROUTING_TASK_STATE;

// --------------------------------------------------------------------------------

#ifdef COPRO1_AVAILABLE

static PROTOCOL_INTERFACE* p_copro1_protocol = 0;

static void copro1_slot_CMD_RECEIVED(void* p_arg) {

	if (p_copro1_protocol != 0) {
		DEBUG_PASS("copro1_slot_CMD_RECEIVED() - COPRO1 is busy");
		return;
	}

	DEBUG_PASS("copro1_slot_CMD_RECEIVED()");
	p_copro1_protocol = (PROTOCOL_INTERFACE*) p_arg;
}

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(COPRO1_ROUTING_COMMAND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(COPRO1_ROUTING_RESPONSE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(COPRO1_ROUTING_COMMAND_SIGNAL, COPRO1_ROUTING_COMMAND_SLOT, copro1_slot_CMD_RECEIVED)

COPRO_INTERFACE_INCLUDE(COPRO1)

#endif

// --------------------------------------------------------------------------------

/*!
 *
 */
static PROTOCOL_INTERFACE* p_scheduled_protocol = 0;

/*!
 *
 */
static COPRO_INTERFACE_OBJECT* p_copro_obj = 0;

/*!
 *
 */
static COPRO_ROUTING_TASK_STATE task_state;

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(COPRO_OP_TIMER)

// --------------------------------------------------------------------------------

void copro_routing_task_init(void) {

	DEBUG_PASS("copro_routing_task_init()");

	#ifdef COPRO1_AVAILABLE
	{
		COPRO1_COMMAND_ROUTING_SIGNAL_init();
		COPRO1_COMMAND_ROUTING_SLOT_connect();
	}
	#endif

	task_state = COPRO_ROUTING_TASK_STATE_IDLE;
}

u16 copro_routing_task_get_schedule_interval(void) {
	return COPRO_ROUTING_TASK_RUN_INTERVAL_MS;
}

MCU_TASK_INTERFACE_TASK_STATE copro_routing_task_get_state(void) {

	#ifdef COPRO1_AVAILABLE
	if (p_copro1_protocol != 0) {
		return MCU_TASK_RUNNING;
	}
	#endif

	if (task_state != COPRO_ROUTING_TASK_STATE_IDLE) {
		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

void copro_routing_task_run(void) {

	DEBUG_PASS("copro_routing_task_run()");

	u8 num_bytes = 0;
	u8 t_data_buffer[COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE];

	switch (task_state) {

		default: // no break;
		case COPRO_ROUTING_TASK_STATE_IDLE :

			#ifdef COPRO1_AVAILABLE
			if (p_copro1_protocol != 0) {
				DEBUG_PASS("copro_routing_task_run() - COPRO_1 selected");
				p_scheduled_protocol = p_copro1_protocol;
				p_copro_obj = (COPRO_INTERFACE_OBJECT*) &COPRO1;
				p_copro1_protocol = 0;
			}
			#endif

			if (p_scheduled_protocol == 0) {
				DEBUG_PASS("copro_routing_task_run() - No copro selected -> what's wrong ?!?");
				break;
			}

			task_state = COPRO_ROUTING_TASK_STATE_PROCESS;
			COPRO_OP_TIMER_start();

			// no break;

		case COPRO_ROUTING_TASK_STATE_PREPARE :

			if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS)) {
				DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PREPARE -> COPRO_ROUTING_TASK_TIMEOUT");
				task_state = COPRO_ROUTING_TASK_TIMEOUT;
				break;
			}

			if (p_copro_obj->mutex_req() == 0)  {
				break;
			}
			
			num_bytes = p_scheduled_protocol->cmd_buffer->bytes_available();
			p_scheduled_protocol->cmd_buffer->start_read();
			p_scheduled_protocol->cmd_buffer->get_N_bytes(num_bytes, t_data_buffer);
			p_scheduled_protocol->cmd_buffer->stop_read();

			p_copro_obj->clear_tx_buffer();
			p_copro_obj->set_N_bytes(num_bytes, t_data_buffer);
			p_copro_obj->start_tx();

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PREPARE -> COPRO_ROUTING_TASK_STATE_PROCESS");
			task_state = COPRO_ROUTING_TASK_STATE_PROCESS;
			// no break;

		case COPRO_ROUTING_TASK_STATE_PROCESS :

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PROCESS -> COPRO_ROUTING_TASK_STATE_FINISH");
			task_state = COPRO_ROUTING_TASK_STATE_FINISH;

			break;

		case COPRO_ROUTING_TASK_TIMEOUT :

			p_protocol->set_finished(CMD_ERR_TIMEOUT);

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_TIMEOUT -> COPRO_ROUTING_TASK_STATE_FINISH");
			task_state = COPRO_ROUTING_TASK_STATE_FINISH;

			break;

		case COPRO_ROUTING_TASK_STATE_FINISH :

			p_scheduled_protocol = 0;

			p_copro_obj->mutex_rel();
			p_copro_obj = 0;

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_FINISH -> COPRO_ROUTING_TASK_STATE_IDLE");
			task_state = COPRO_ROUTING_TASK_STATE_IDLE;

			break;
	}
}

void copro_routing_task_background_run(void) {

}

void copro_routing_task_sleep(void) {

}

void copro_routing_task_wakeup(void) {

}

void copro_routing_task_finish(void) {

}

void copro_routingtask_terminate(void) {

}
