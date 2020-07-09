/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_task/copro_routing_mcu_task.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

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

#include "command_management/protocol_interface.h"
#include "command_management/command_handler_interface.h"

#include "command_handler/rpi_cmd_handler_routing.h"

#include "copro/copro_interface.h"

// --------------------------------------------------------------------------------

#define COPRO_ROUTING_TASK_RUN_INTERVAL_MS		0
#define COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS		500
#define COPRO_ROUTING_TASK_PROCESS_TIMEOUT_MS		250
#define COPRO_ROUTING_TASK_TRANSMIT_TIMEOUT_MS		50
#define COPRO_ROUTING_TASK_POLLING_INTERVAL_MS		70

#define COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE	64

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	COPRO_ROUTING_TASK_STATE_IDLE,
	COPRO_ROUTING_TASK_STATE_PREPARE,
	COPRO_ROUTING_TASK_STATE_TRANSMIT,
	COPRO_ROUTING_TASK_STATE_PROCESS,
	COPRO_ROUTING_TASK_STATE_RESPONSE,
	COPRO_ROUTING_TASK_STATE_FINISH,
	COPRO_ROUTING_TASK_TIMEOUT
} COPRO_ROUTING_TASK_STATE;

//-----------------------------------------------------------------------------

/*
 *
 */
typedef void (*COPRO_TABLE_SIGNAL_CALLBACK)		(void* p_arg);

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
static COPRO_TABLE_SIGNAL_CALLBACK p_response_callback = 0;

/*!
 *
 */
static COPRO_ROUTING_TASK_STATE task_state;

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(COPRO_OP_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(COPRO_WAIT_TIMER)

// --------------------------------------------------------------------------------

void copro_routing_task_init(void) {

	DEBUG_PASS("copro_routing_task_init()");

	#ifdef COPRO1_AVAILABLE
	{
		COPRO1_ROUTING_COMMAND_SIGNAL_init();
		COPRO1_ROUTING_RESPONSE_SIGNAL_init();
		COPRO1_ROUTING_COMMAND_SLOT_connect();
	}
	#endif
	
	EVENT_OUTPUT_drive_low();

	task_state = COPRO_ROUTING_TASK_STATE_IDLE;
	p_response_callback = 0;
	p_scheduled_protocol = 0;
	p_copro_obj = 0;

	DEBUG_PASS("copro_routing_task_init() - rpi_cmd_handler_routing_init()");
	rpi_cmd_handler_routing_init();
}

u16 copro_routing_task_get_schedule_interval(void) {
	return COPRO_ROUTING_TASK_RUN_INTERVAL_MS;
}

MCU_TASK_INTERFACE_TASK_STATE copro_routing_task_get_state(void) {

	if (task_state != COPRO_ROUTING_TASK_STATE_IDLE) {
		return MCU_TASK_RUNNING;
	}

	if (p_scheduled_protocol != 0) {
		return MCU_TASK_RUNNING;
	}

	#ifdef COPRO1_AVAILABLE
	if (p_copro1_protocol != 0) {

		DEBUG_PASS("copro_routing_task_get_state() - COPRO1 selected");

		p_scheduled_protocol = p_copro1_protocol;
		p_copro_obj = (COPRO_INTERFACE_OBJECT*) &COPRO1;
		p_response_callback = &COPRO1_ROUTING_RESPONSE_SIGNAL_send;
		p_copro1_protocol = 0;

		return MCU_TASK_RUNNING;
	}
	#endif

	return MCU_TASK_SLEEPING;
}

void copro_routing_task_run(void) {

	//DEBUG_PASS("copro_routing_task_run()");

	u8 num_bytes = 0;
	u8 t_data_buffer[COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE];

	switch (task_state) {

		default: // no break;
		case COPRO_ROUTING_TASK_STATE_IDLE :

			if (p_scheduled_protocol == 0) {
				DEBUG_PASS("copro_routing_task_run() - No copro selected -> what's wrong ?!?");
				break;
			}

			EVENT_OUTPUT_drive_low();
				
			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_IDLE -> COPRO_ROUTING_TASK_STATE_PREPARE");
			task_state = COPRO_ROUTING_TASK_STATE_PROCESS;
			COPRO_OP_TIMER_start();

			// no break;

		case COPRO_ROUTING_TASK_STATE_PREPARE :

			if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS)) {
				DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PREPARE -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
				task_state = COPRO_ROUTING_TASK_TIMEOUT;
				break;
			}

			if (p_copro_obj->mutex_req() == 0)  {
				DEBUG_PASS("copro_routing_task_run() - p_copro_obj->mutex_req() has FAILED !!! ---");
				break;
			}
			
			num_bytes = p_scheduled_protocol->cmd_buffer->bytes_available();
			p_scheduled_protocol->cmd_buffer->start_read();
			p_scheduled_protocol->cmd_buffer->get_N_bytes(num_bytes, t_data_buffer);
			p_scheduled_protocol->cmd_buffer->stop_read();

			DEBUG_TRACE_N(num_bytes, t_data_buffer, "copro_routing_task_run() - Routing Command");

			p_copro_obj->power_on();
			p_copro_obj->clear_tx_buffer();
			p_copro_obj->set_N_bytes(num_bytes, t_data_buffer);
			p_copro_obj->start_tx();

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PREPARE -> COPRO_ROUTING_TASK_STATE_TRANSMIT");
			task_state = COPRO_ROUTING_TASK_STATE_TRANSMIT;

			COPRO_OP_TIMER_start();
			// no break;

		case COPRO_ROUTING_TASK_STATE_TRANSMIT :

			if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_TRANSMIT_TIMEOUT_MS)) {
				DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_TRANSMIT -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
				task_state = COPRO_ROUTING_TASK_TIMEOUT;
				break;
			}

			if (p_copro_obj->is_ready_for_tx() == 0) {
				break;
			}

			p_copro_obj->stop_tx();

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_TRANSMIT -> COPRO_ROUTING_TASK_STATE_PROCESS");
			task_state = COPRO_ROUTING_TASK_STATE_PROCESS;

			COPRO_OP_TIMER_start();
			COPRO_WAIT_TIMER_start();
			// no break;


		case COPRO_ROUTING_TASK_STATE_PROCESS :

			if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_PROCESS_TIMEOUT_MS)) {
				DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PROCESS -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
				task_state = COPRO_ROUTING_TASK_TIMEOUT;
				break;
			}

			if (COPRO_WAIT_TIMER_is_up(COPRO_ROUTING_TASK_POLLING_INTERVAL_MS) == 0) {
				break;
			}

			DEBUG_TRACE_word(COPRO_WAIT_TIMER_elapsed(), "copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PROCESS - Polling COPRO for response");

			COPRO_WAIT_TIMER_start();

			EVENT_OUTPUT_drive_high();

			p_copro_obj->clear_rx_buffer();
			p_copro_obj->start_rx(1); // get length of answer
			p_copro_obj->wait_for_rx(1, 5);
			p_copro_obj->stop_rx();

			EVENT_OUTPUT_drive_low();

			if (p_copro_obj->bytes_available() == 0) {
				break;
			}

			num_bytes = p_copro_obj->get_N_bytes(1, t_data_buffer);

			if (t_data_buffer[0] == 0) {
				break;
			}

			DEBUG_TRACE_byte(t_data_buffer[0], "copro_routing_task_run() - Number of bytes to read from Copro");

			p_copro_obj->start_rx(t_data_buffer[0]); // get length of answer
			p_copro_obj->wait_for_rx(t_data_buffer[0], 50);
			p_copro_obj->stop_rx();

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_PROCESS -> COPRO_ROUTING_TASK_STATE_RESPONSE");
			task_state = COPRO_ROUTING_TASK_STATE_RESPONSE;

			break;

		case COPRO_ROUTING_TASK_STATE_RESPONSE :

			num_bytes = p_copro_obj->get_N_bytes(COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE, t_data_buffer + 1);

			if (num_bytes == 0) {
				DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_RESPONSE -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
				task_state = COPRO_ROUTING_TASK_TIMEOUT;
				COPRO_WAIT_TIMER_start();
				break;
			}

			p_scheduled_protocol->answ_buffer->start_write();
			p_scheduled_protocol->answ_buffer->add_N_bytes(t_data_buffer[0], t_data_buffer + 1);
			p_scheduled_protocol->answ_buffer->stop_write();

			DEBUG_TRACE_N(t_data_buffer[0] + 1, t_data_buffer, "copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_RESPONSE - Data received:");

			if (p_response_callback != 0) {
				DEBUG_PASS("copro_routing_task_run() - p_response_callback()");
				p_response_callback((void*) p_scheduled_protocol);
			}

			DEBUG_PASS("copro_routing_task_run() - COPRO_ROUTING_TASK_STATE_RESPONSE -> COPRO_ROUTING_TASK_STATE_FINISH");
			task_state = COPRO_ROUTING_TASK_STATE_FINISH;

			break;

		case COPRO_ROUTING_TASK_TIMEOUT :

			p_scheduled_protocol->set_finished(CMD_ERR_TIMEOUT);

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
