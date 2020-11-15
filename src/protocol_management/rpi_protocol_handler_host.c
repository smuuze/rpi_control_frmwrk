 /*
  * \@file	protocol_management/rpi_protocl_handler_host.c
  * \author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"
#include "common/common_types.h"

//#include "common/local_context.h"
#include "common/local_msg_buffer.h"
#include "common/local_module_status.h"
#include "common/local_mutex.h"

#include "io_management/io_controller.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "driver/driver_specific_spi.h"

//#include "command_handler/rpi_command_handler.h"
#include "command_management/protocol_interface.h"
#include "time_management/time_management.h"
#include "common/signal_slot_interface.h"

#include "protocol_management/rpi_protocol_handler.h"

//-----------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS
#define RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS		5
#endif

#ifndef RPI_HOST_COMMAND_BUFFER_SIZE
#define RPI_HOST_COMMAND_BUFFER_SIZE				128
#endif

#ifndef RPI_HOST_RESPONSE_BUFFER_SIZE
#define RPI_HOST_RESPONSE_BUFFER_SIZE				128
#endif

#ifndef RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS
#define RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS		60
#endif

#ifndef RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS
#define RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS		500
#endif

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	RPI_HOST_STATE_SLEEP,
	RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND,
	RPI_HOST_STATE_CLIENT_REQUEST_FOR_COMMAND,
	RPI_HOST_STATE_WAIT_FOR_CLIENT,
	RPI_HOST_STATE_SEND_COMMAND,
	RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE,
	RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE,
	RPI_HOST_STATE_RECEIVE_RESPONSE,
	RPI_HOST_STATE_FINISH
} RPI_HOST_STATE_TYPE;

//-----------------------------------------------------------------------------

/*!
 *
 */
static void rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument);

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)

//-----------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, RPI_HOST_COMMAND_RECEIVED_SLOT, rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK)

//-----------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , RPI_HOST_COMMAND_BUFFER,  RPI_HOST_COMMAND_BUFFER_SIZE)
BUILD_LOCAL_MSG_BUFFER( , RPI_HOST_RESPONSE_BUFFER, RPI_HOST_RESPONSE_BUFFER_SIZE)

//-----------------------------------------------------------------------------

/*!
 *
 */
static RPI_HOST_STATE_TYPE rpi_host_state = RPI_HOST_STATE_SLEEP;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE actual_task_state = MCU_TASK_UNINITIALIZED;

/*!
 *
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;

/*!
 *
 */
static CFG_DRIVER_SPI _com_driver_cfg_spi = {
	RPI_PROTOCOL_HANDLER_DRIVER_CFG
};

/*!
 *
 */
static TRX_DRIVER_CONFIGURATION driver_cfg;

//-----------------------------------------------------------------------------

static void rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

	const COMMON_GENERIC_BUFFER_TYPE* p_buffer = (const COMMON_GENERIC_BUFFER_TYPE*) p_argument;
	DEBUG_TRACE_N(p_buffer->length, p_buffer->data, "rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK()");

	RPI_HOST_COMMAND_BUFFER_start_write();
	RPI_HOST_COMMAND_BUFFER_add_N_bytes(p_buffer->length, p_buffer->data);
	RPI_HOST_COMMAND_BUFFER_stop_write();
}

//-----------------------------------------------------------------------------

/*!
 *
 */
static void rpi_host_send_command(void) {

	//DEBUG_PASS("rpi_host_send_command()");

	u8 num_bytes = (u8)RPI_HOST_COMMAND_BUFFER_bytes_available();
	u8 p_buffer[RPI_HOST_COMMAND_BUFFER_SIZE];
	
	RPI_HOST_COMMAND_BUFFER_start_read();
	RPI_HOST_COMMAND_BUFFER_get_N_bytes(num_bytes, p_buffer);
	RPI_HOST_COMMAND_BUFFER_stop_read();

	p_com_driver->clear_rx_buffer();
	p_com_driver->set_N_bytes(num_bytes, p_buffer);
	
	p_com_driver->start_tx();
}

/*!
 *
 */
static u8 rpi_host_receive_response(void) {

	p_com_driver->start_rx(1);

	if (p_com_driver->bytes_available() == 0) {
		DEBUG_PASS("rpi_host_receive_response() - No data received");
		return 0;
	}

	COMMON_GENERIC_BUFFER_TYPE buffer;
	buffer.length = 0;

	p_com_driver->get_N_bytes(1, (u8*)&buffer.length);

	if (buffer.length == 0) {
		DEBUG_PASS("rpi_host_receive_response() - Received Length is zero");
		return 0;
	}

	if (buffer.length > COMMON_TYPES_GENERIC_BUFFER_SIZE) {
		DEBUG_TRACE_word(buffer.length, "rpi_host_receive_response() - Length exxeds buffer-limit");
		buffer.length = COMMON_TYPES_GENERIC_BUFFER_SIZE;
	}

	p_com_driver->start_rx(buffer.length);
	p_com_driver->get_N_bytes(buffer.length, buffer.data);
			
	RPI_HOST_RESPONSE_RECEIVED_SIGNAL_send((void*)&buffer);

	return 1;
}

//-----------------------------------------------------------------------------

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	DEBUG_PASS("rpi_protocol_init()");

	REQUEST_CLIENT_pull_up();

	p_com_driver = p_driver;

	driver_cfg.module.spi = _com_driver_cfg_spi;
	p_com_driver->configure(&driver_cfg);

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_COMMAND_RECEIVED_SIGNAL_init()");
	RPI_HOST_COMMAND_RECEIVED_SIGNAL_init();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init()");
	RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_COMMAND_RECEIVED_SLOT_connect()");
	RPI_HOST_COMMAND_RECEIVED_SLOT_connect();
}

void rpi_protocol_task_init(void) {

	DEBUG_PASS("rpi_protocol_task_init()");

	actual_task_state = MCU_TASK_SLEEPING;
	rpi_host_state = RPI_HOST_STATE_SLEEP;

	RPI_OP_TIMER_start();
}

u16 rpi_protocol_task_get_schedule_interval(void) {

	if (actual_task_state != MCU_TASK_SLEEPING) {
		return 0;
	} else {
		return RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS;
	}
}

MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	if (rpi_host_state != RPI_HOST_STATE_SLEEP) {
		return MCU_TASK_RUNNING;
	}

	if (RPI_HOST_COMMAND_BUFFER_bytes_available()) {
		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

void rpi_protocol_task_run(void) {

	//DEBUG_PASS("rpi_protocol_task_run()");

	switch (rpi_host_state) {

		default:
			
			rpi_host_state = RPI_HOST_STATE_SLEEP;

		case RPI_HOST_STATE_SLEEP:

			if (RPI_HOST_COMMAND_BUFFER_bytes_available() == 0) {
				actual_task_state = MCU_TASK_SLEEPING;
				break;
			}

			rpi_host_state = RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND;
			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_SLEEP >> RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND");

			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS)) {
				DEBUG_PASS("rpi_protocol_task_run() - TIMEOUT!!! - RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND >> RPI_HOST_STATE_FINISH");
				rpi_host_state = RPI_HOST_STATE_FINISH;
				break;
			}

			if (REQUEST_CLIENT_is_low_level()) {
				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND >> RPI_HOST_STATE_CLIENT_REQUEST_FOR_COMMAND - Time elapsed: ");

			rpi_host_state = RPI_HOST_STATE_CLIENT_REQUEST_FOR_COMMAND;

			REQUEST_CLIENT_drive_low();
			RPI_OP_TIMER_start();

			// no break;	

		case RPI_HOST_STATE_CLIENT_REQUEST_FOR_COMMAND:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS) == 0) {
				break;
			}
	
			REQUEST_CLIENT_pull_up();

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_REQUEST_FOR_COMMAND >> RPI_HOST_STATE_SEND_COMMAND");

			rpi_host_state = RPI_HOST_STATE_SEND_COMMAND;
			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_SEND_COMMAND:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS)) {
				DEBUG_PASS("rpi_protocol_task_run() - TIMEOUT!!! - RPI_HOST_STATE_SEND_COMMAND >> RPI_HOST_STATE_FINISH");
				rpi_host_state = RPI_HOST_STATE_FINISH;
				break;
			}

			if (REQUEST_CLIENT_is_high_level()) {
				break;
			}

			rpi_host_send_command();

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_SEND_COMMAND >> RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE");

			rpi_host_state = RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE;
			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS)) {
				DEBUG_PASS("rpi_protocol_task_run() - TIMEOUT!!! - RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE >> RPI_HOST_STATE_FINISH");
				rpi_host_state = RPI_HOST_STATE_FINISH;
				break;
			}

			if (REQUEST_CLIENT_is_low_level()) {
				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE >> RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE - Time elapsed: ");

			rpi_host_state = RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE;

			REQUEST_CLIENT_drive_low();
			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS) == 0) {
				break;
			}
	
			REQUEST_CLIENT_pull_up();

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE >> RPI_HOST_STATE_RECEIVE_RESPONSE");

			rpi_host_state = RPI_HOST_STATE_RECEIVE_RESPONSE;
			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_RECEIVE_RESPONSE:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS)) {
				DEBUG_PASS("rpi_protocol_task_run() - TIMEOUT!!! - RPI_HOST_STATE_RECEIVE_RESPONSE >> RPI_HOST_STATE_FINISH");
				rpi_host_state = RPI_HOST_STATE_FINISH;
				break;
			}

			if (REQUEST_CLIENT_is_high_level()) {
				break;
			}

			if (rpi_host_receive_response() == 0) {
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_RECEIVE_RESPONSE >> RPI_HOST_STATE_FINISH");

			rpi_host_state = RPI_HOST_STATE_FINISH;

		case RPI_HOST_STATE_FINISH:

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_FINISH >> RPI_HOST_STATE_SLEEP");
			rpi_host_state = RPI_HOST_STATE_SLEEP;
			break;
	}
}

u8 rpi_protocol_handler_get_actual_state(void) {
	return (u8) rpi_host_state;
}