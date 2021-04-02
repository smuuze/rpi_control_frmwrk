 /*
  * \@file	protocol_management/rpi_protocl_handler_host.c
  * \author	sebastian lesse
  */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"
#include "common/common_types.h"

#include "common/local_msg_buffer.h"
#include "common/local_module_status.h"
#include "common/local_mutex.h"
#include "common/common_tools_string.h"

#include "io_management/io_controller.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "driver/driver_specific_spi.h"

#include "command_management/protocol_interface.h"
#include "time_management/time_management.h"
#include "common/signal_slot_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

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
#define RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS		64
#endif

#ifndef RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS
#define RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS		505
#endif

#ifndef RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS
#define RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS			50
#endif

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	RPI_HOST_WAIT_FOR_USER_CFG,
	RPI_HOST_STATE_SLEEP,
	RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND,
	RPI_HOST_STATE_REQUEST_CLIENT,
	RPI_HOST_STATE_WAIT_FOR_CLIENT,
	RPI_HOST_STATE_SEND_COMMAND,
	RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE,
	RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE,
	RPI_HOST_STATE_RECEIVE_RESPONSE,
	RPI_HOST_STATE_FINISH
} RPI_HOST_STATE_TYPE;

// --------------------------------------------------------------------------------

#define RPI_HOST_STATUS_COMMAND_PENDING			(1 << 0)
#define RPI_HOST_STATUS_USER_CONFIG_COMPLETE		(1 << 1)

BUILD_MODULE_STATUS_U8(RPI_HOST_STATUS)

//-----------------------------------------------------------------------------

/*!
 *
 */
static void rpi_protocol_task_init(void);

/*!
 *
 */
static u16 rpi_protocol_task_get_schedule_interval(void);

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void);

/*!
 *
 */
static void rpi_protocol_task_run(void);

//-----------------------------------------------------------------------------

/*!
 *
 */
static void rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void rpi_protocol_new_cfg_object_CALLBACK(const void* p_argument);

/*!
 *
 */
static void rpi_protocol_cfg_complete_CALLBACK(const void* p_argument);

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)

//-----------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_TIMEOUT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_OVERFLOW_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, RPI_HOST_COMMAND_RECEIVED_SLOT, rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL, RPI_HOST_NEW_CFG_OBJECT_SLOT, rpi_protocol_new_cfg_object_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_CFG_COMPLETE_SIGNAL, RPI_HOST_CFG_COMPLETE_SLOT, rpi_protocol_cfg_complete_CALLBACK)

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

	RPI_HOST_STATUS_set(RPI_HOST_STATUS_COMMAND_PENDING);
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
		//DEBUG_PASS("rpi_host_receive_response() - No data received");
		return 0;
	}

	COMMON_GENERIC_BUFFER_TYPE buffer;
	buffer.length = 0;

	p_com_driver->get_N_bytes(1, (u8*)&buffer.length);

	if (buffer.length == 0) {
		//DEBUG_PASS("rpi_host_receive_response() - Received Length is zero");
		return 0;
	}

	if (buffer.length > COMMON_TYPES_GENERIC_BUFFER_SIZE) {

		DEBUG_TRACE_word(buffer.length, "rpi_host_receive_response() - Length exeeds buffer-limit");

		buffer.length = COMMON_TYPES_GENERIC_BUFFER_SIZE;
		RPI_HOST_RESPONSE_OVERFLOW_SIGNAL_send(NULL);

		// we will accept as much as possible bytes
	}

	p_com_driver->start_rx(buffer.length);
	p_com_driver->get_N_bytes(buffer.length, buffer.data);
			
	RPI_HOST_RESPONSE_RECEIVED_SIGNAL_send((void*)&buffer);

	return 1;
}

//-----------------------------------------------------------------------------

static MCU_TASK_INTERFACE rpi_protocol_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&rpi_protocol_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&rpi_protocol_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&rpi_protocol_task_get_state, 			// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&rpi_protocol_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};

//-----------------------------------------------------------------------------

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	DEBUG_PASS("rpi_protocol_init()");

	RPI_HOST_STATUS_clear_all();

	REQUEST_CLIENT_pull_up();

	p_com_driver = p_driver;

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_COMMAND_RECEIVED_SIGNAL_init()");
	RPI_HOST_COMMAND_RECEIVED_SIGNAL_init();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init()");
	RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_TIMEOUT_SIGNAL_init()");
	RPI_HOST_RESPONSE_TIMEOUT_SIGNAL_init();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_OVERFLOW_SIGNAL_init()");
	RPI_HOST_RESPONSE_OVERFLOW_SIGNAL_init();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_COMMAND_RECEIVED_SLOT_connect()");
	RPI_HOST_COMMAND_RECEIVED_SLOT_connect();
	
	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_NEW_CFG_OBJECT_SLOT_connect()");
	RPI_HOST_NEW_CFG_OBJECT_SLOT_connect();

	DEBUG_PASS("rpi_protocol_init() - RPI_HOST_CFG_COMPLETE_SLOT_connect()");
	RPI_HOST_CFG_COMPLETE_SLOT_connect();

	mcu_task_controller_register_task(&rpi_protocol_task);
}

u8 rpi_protocol_handler_get_actual_state(void) {
	return (u8) rpi_host_state;
}

//-----------------------------------------------------------------------------

static void rpi_protocol_task_init(void) {

	DEBUG_PASS("rpi_protocol_task_init() - HOST - ");

	actual_task_state = MCU_TASK_SLEEPING;
	rpi_host_state = RPI_HOST_WAIT_FOR_USER_CFG;

	RPI_OP_TIMER_start();
}

static u16 rpi_protocol_task_get_schedule_interval(void) {

	if (actual_task_state != MCU_TASK_SLEEPING) {
		return 0;
	} else {
		return RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS;
	}
}

static MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	if (rpi_host_state != RPI_HOST_STATE_SLEEP) {
		return MCU_TASK_RUNNING;
	}

	if (RPI_HOST_STATUS_is_set(RPI_HOST_STATUS_COMMAND_PENDING)) {
		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

static void rpi_protocol_task_run(void) {

	//DEBUG_PASS("rpi_protocol_task_run()");

	u32 client_poll_interval_ms = RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS;

	switch (rpi_host_state) {

		default:
			
			rpi_host_state = RPI_HOST_WAIT_FOR_USER_CFG;

		case RPI_HOST_WAIT_FOR_USER_CFG :

			if (RPI_HOST_STATUS_is_set(RPI_HOST_STATUS_USER_CONFIG_COMPLETE)) {

				DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_WAIT_FOR_USER_CFG >> RPI_HOST_STATE_SLEEP");

				driver_cfg.module.spi = _com_driver_cfg_spi;
				p_com_driver->configure(&driver_cfg);
				rpi_host_state = RPI_HOST_STATE_SLEEP;
			}

			break;

		case RPI_HOST_STATE_SLEEP:

			if (RPI_HOST_STATUS_is_set(RPI_HOST_STATUS_COMMAND_PENDING) == 0) {
				actual_task_state = MCU_TASK_SLEEPING;
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_SLEEP >> RPI_HOST_STATE_REQUEST_CLIENT");

			RPI_HOST_STATUS_unset(RPI_HOST_STATUS_COMMAND_PENDING);
			rpi_host_state = RPI_HOST_STATE_REQUEST_CLIENT;

			// no break;

		case RPI_HOST_STATE_REQUEST_CLIENT:

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND >> RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND");

			REQUEST_CLIENT_drive_low();

			rpi_host_state = RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND;
			RPI_OP_TIMER_start();

			// no break;	

		case RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS) == 0) {
				//DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND - waiting for client to wake up");
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_REQUEST_CLIENT >> RPI_HOST_STATE_SEND_COMMAND");

			rpi_host_state = RPI_HOST_STATE_SEND_COMMAND;
			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_SEND_COMMAND:

			rpi_host_send_command();

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_SEND_COMMAND >> RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE");

			client_poll_interval_ms = RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS;
			rpi_host_state = RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE;
			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS)) {
				DEBUG_PASS("rpi_protocol_task_run() - TIMEOUT!!! - RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE >> RPI_HOST_STATE_FINISH");
				RPI_HOST_RESPONSE_TIMEOUT_SIGNAL_send(NULL);
				rpi_host_state = RPI_HOST_STATE_FINISH;
				break;
			}

			if (RPI_OP_TIMER_is_up(client_poll_interval_ms) == 0) {
				break;
			}
			
			client_poll_interval_ms += RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS;

			if (rpi_host_receive_response() == 0) {
				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE >> RPI_HOST_STATE_FINISH - Time elapsed: ");

			rpi_host_state = RPI_HOST_STATE_FINISH;

			RPI_OP_TIMER_start();

			// no break;

		case RPI_HOST_STATE_FINISH:

			DEBUG_PASS("rpi_protocol_task_run() - RPI_HOST_STATE_FINISH >> RPI_HOST_STATE_SLEEP");
			rpi_host_state = RPI_HOST_STATE_SLEEP;
			REQUEST_CLIENT_no_pull();
			break;
	}
}

static void rpi_protocol_new_cfg_object_CALLBACK(const void* p_argument) {

	if (p_argument == NULL) {
		DEBUG_PASS("rpi_protocol_new_cfg_object_CALLBACK() - NULL_POINTER_EXCEPTION");
		return;
	}

	CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_object = (CFG_FILE_PARSER_CFG_OBJECT_TYPE*) p_argument;

	if (common_tools_string_compare(RPI_CONTROL_PREFIX_CFG_STRING, p_cfg_object->key)) {
		DEBUG_TRACE_STR(p_cfg_object->key, "rpi_protocol_new_cfg_object_CALLBACK() - Unknown cfg_object - prefix");
		return;
	}

	if (common_tools_string_compare(RPI_PROTOCOL_SPEED_CFG_STRING, p_cfg_object->key)) {
		
		DEBUG_TRACE_STR(p_cfg_object->value, "rpi_protocol_new_cfg_object_CALLBACK() - RPI_PROTOCOL_SPEED_CFG_STRING :");
		_com_driver_cfg_spi.speed = common_tools_string_to_u32(p_cfg_object->value);
		return;
	}

	if (common_tools_string_compare(RPI_PROTOCOL_DEVICE_CFG_STRING, p_cfg_object->key)) {
		
		DEBUG_TRACE_STR(p_cfg_object->value, "rpi_protocol_new_cfg_object_CALLBACK() - RPI_PROTOCOL_DEVICE_CFG_STRING :");
		common_tools_string_copy_string(_com_driver_cfg_spi.device, p_cfg_object->value, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
		return;
	}

	DEBUG_TRACE_STR(p_cfg_object->key, "rpi_protocol_new_cfg_object_CALLBACK() - Unknown cfg_object");
}

static void rpi_protocol_cfg_complete_CALLBACK(const void* p_argument) {

	(void) p_argument;

	DEBUG_PASS("rpi_protocol_cfg_complete_CALLBACK()");
	RPI_HOST_STATUS_set(RPI_HOST_STATUS_USER_CONFIG_COMPLETE);
}

