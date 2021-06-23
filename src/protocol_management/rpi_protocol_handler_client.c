 /*
  * \@file	protocol_management/rpi_protocl_handler.c
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

#ifndef HAS_GPIO_READY_INOUT
#error DEFINE_PIN___READY_INOUT___IN_YOUR_BOARD_FILE
#endif

//-----------------------------------------------------------------------------

#define RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE			32

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS		250

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_START_DATA_EXCHANGE_TIMEOUT_MS	10

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS	250

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_DRIVER_TIMEOUT_MS		250

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_RELEASE_TIMEOUT_MS	15

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS		500

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS		5

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	RPI_STATE_IDLE,
	RPI_STATE_PREPARE,
	RPI_STATE_TRANSMISSION,
	RPI_STATE_COMMAND_RECEIVED,
	RPI_ENTER_SLEEP,
} RPI_PROTOCOL_HANDLER_STATE;

/*!
 *
 */
typedef enum {
	RPI_TRX_STATE_BUSY,
	RPI_TRX_STATE_NO_DATA,
	RPI_TRX_STATE_COMPLETE,
	RPI_TRX_STATE_TIMEOUT
} RPI_TRX_STATE;

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
//static void _set_finished_debus(u8 err_code);

/*!
 *
 * @param err_code
 */
static void _set_finished_spi(u8 err_code);

//-----------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , RPI_COMMAND_BUFFER, 32)
BUILD_LOCAL_MSG_BUFFER( , RPI_ANSWER_BUFFER,  32)

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_TRX_TIMER)

//-----------------------------------------------------------------------------

#define RPI_STATUS_COMMAND_PENDING		(1 << 0)
#define RPI_STATUS_RESPONSE_PENDING		(1 << 1)

BUILD_MODULE_STATUS_U16(RPI_STATUS)

//-----------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_ENTER_SLEEP_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL)

//-----------------------------------------------------------------------------

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

/*!
 *
 */
static RPI_PROTOCOL_HANDLER_STATE actual_state = RPI_STATE_IDLE;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE actual_task_state = MCU_TASK_UNINITIALIZED;

/*!
 *
 */
u8 driver_mutex_id = 0;

/*!
 *
 */
static COMMAND_BUFFER_INTERFACE rpi_cmd_handler_command_buffer = {
	&RPI_COMMAND_BUFFER_start_read,
	&RPI_COMMAND_BUFFER_bytes_available, 	// CMD_BUFFER_INTERFACE_BYTES_AVAILABLE_CALLBACK	bytes_available;
	&RPI_COMMAND_BUFFER_get_byte, 		// CMD_BUFFER_INTERFACE_GET_BYTE_CALLBACK		get_byte;
	&RPI_COMMAND_BUFFER_get_word, 		// CMD_BUFFER_INTERFACE_GET_WORD_CALLBACK		get_word;
	&RPI_COMMAND_BUFFER_get_long, 		// CMD_BUFFER_INTERFACE_GET_LONG_CALLBACK		get_long;
	&RPI_COMMAND_BUFFER_get_N_bytes, 	// CMD_BUFFER_INTERFACE_GET_N_BYTES_CALLBACK		get_N_bytes;
	&RPI_COMMAND_BUFFER_stop_read
};

/*!
 *
 */
static ANSWER_BUFFER_INTERFACE rpi_cmd_handler_answer_buffer = {
	&RPI_ANSWER_BUFFER_start_write,
	&RPI_ANSWER_BUFFER_add_byte , 		// ANSWER_BUFFER_INTERFACE_ADD_BYTE_CALLBACK		add_byte;
	&RPI_ANSWER_BUFFER_add_word , 		// ANSWER_BUFFER_INTERFACE_ADD_WORD_CALLBACK		add_word;
	&RPI_ANSWER_BUFFER_add_long , 		// ANSWER_BUFFER_INTERFACE_ADD_LONG_CALLBACK		add_long;
	&RPI_ANSWER_BUFFER_add_N_bytes ,	// ANSWER_BUFFER_INTERFACE_ADD_N_BYTES_CALLBACK		add_N_bytes;
	&RPI_ANSWER_BUFFER_stop_write,
};

/*!
 *
 */
static PROTOCOL_INTERFACE rpi_protocol_spi_interface = {

	0, // commnand-length
	0, // command-code
	0, // answer-status
	0, // arrival-time
	&_set_finished_spi,
	&rpi_cmd_handler_command_buffer,
	&rpi_cmd_handler_answer_buffer
};

//-----------------------------------------------------------------------------

/*!
 *
 * @param err_code
 */
static void _set_finished_spi(u8 err_code) {

	if (RPI_STATUS_is_set(RPI_STATUS_COMMAND_PENDING) == 0) {
		return;
	}

	//DEBUG_TRACE_byte(err_code, "_set_finished_spi() - Error: ");

	rpi_protocol_spi_interface.answer_status = err_code;

	RPI_STATUS_unset(RPI_STATUS_COMMAND_PENDING);
	RPI_STATUS_set(RPI_STATUS_RESPONSE_PENDING);
	//rpi_cmd_handler_set_unrequested();
}

/*!
 *
 */
static RPI_TRX_STATE rpi_protocol_receive_command(void) {

	rpi_protocol_spi_interface.command_length = 0;

	RPI_TRX_STATE error_code = RPI_TRX_STATE_COMPLETE;
			
	RPI_TRX_TIMER_start();

	while (p_com_driver->bytes_available() != 0) {
	//while (rpi_protocol_spi_interface.command_length == 0) {
			
		//DEBUG_TRACE_byte(p_com_driver->bytes_available(), "rpi_protocol_receive_command() - Bytes-Available: ");

		// first byte gives the length of the command (how many bytes will follow)
		//p_com_driver->wait_for_rx(1, 50); // blocking function

		if (RPI_TRX_TIMER_is_up(5)) {
			DEBUG_PASS("rpi_protocol_receive_command() - Receiving command-length has FAILED (TIMEOUT) !!! ---");
			error_code = RPI_TRX_STATE_TIMEOUT;
			goto EXIT_rpi_protocol_receive_command;
		}

		p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_length);
		if (rpi_protocol_spi_interface.command_length == 0) {
			//DEBUG_PASS("rpi_protocol_receive_command() - Zero byte received");
			continue;
		}

		if (rpi_protocol_spi_interface.command_length == 0xFF) {
			//DEBUG_PASS("rpi_protocol_receive_command() - 0xFF byte received");
			rpi_protocol_spi_interface.command_length = 0;
			continue;
		}

		if (rpi_protocol_spi_interface.command_length != 0) {
			DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "rpi_protocol_receive_command() - Command-Length: ");
			break;
		}
	}

	if (rpi_protocol_spi_interface.command_length == 0) {
		//DEBUG_PASS("rpi_protocol_receive_command() - No command-data received");
		error_code = RPI_TRX_STATE_NO_DATA;
		goto EXIT_rpi_protocol_receive_command;
	}
			
	RPI_TRX_TIMER_start();

	while (p_com_driver->bytes_available() == 0) {

		if (RPI_TRX_TIMER_is_up(10)) {
			DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "rpi_protocol_receive_command() - Receiving command-data has FAILED (TIMEOUT) !!! ---");
			error_code = RPI_TRX_STATE_TIMEOUT;
			goto EXIT_rpi_protocol_receive_command;
		}
	}

	p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_code);

	DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length ,"rpi_protocol_receive_command() - Command-Length: ");
	DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_code ,"rpi_protocol_receive_command() - Command-Code: ");

	RPI_COMMAND_BUFFER_clear_all();
	RPI_COMMAND_BUFFER_start_write();
			
	RPI_TRX_TIMER_start();

	u16 bytes_remain = (u16)rpi_protocol_spi_interface.command_length - 1; // command already received

	while (bytes_remain) {

		if (RPI_TRX_TIMER_is_up(50)) {
			DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "rpi_protocol_receive_command() - Receiving command-data has FAILED (TIMEOUT) !!! ---");
			error_code = RPI_TRX_STATE_TIMEOUT;
			goto EXIT_rpi_protocol_receive_command;
		}

		if (p_com_driver->bytes_available() == 0) {
			continue;
		}

		u8 t_buffer[RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE];
		u16 read_count = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;

		if (read_count > p_com_driver->bytes_available()) {
			read_count = p_com_driver->bytes_available();
		}

		if (read_count > bytes_remain) {
			read_count = bytes_remain;
		}

		read_count = p_com_driver->get_N_bytes(read_count, t_buffer);
		bytes_remain -= read_count;

		RPI_COMMAND_BUFFER_add_N_bytes(read_count, t_buffer);

		DEBUG_TRACE_N(read_count, t_buffer ,"rpi_protocol_receive_command() - Command-Data: ");
	}

	RPI_COMMAND_BUFFER_stop_write();
	RPI_STATUS_set(RPI_STATUS_COMMAND_PENDING);

	EXIT_rpi_protocol_receive_command :
	{
		p_com_driver->mutex_rel(driver_mutex_id);

		return error_code;
	}
}

/*!
 *
 */
static RPI_TRX_STATE rpi_protocol_transmit_answer(void) {

	//DEBUG_PASS("rpi_protocol_transmit_answer()");

	#ifdef TRACES_ENABLED
	rpi_protocol_spi_interface.arrival_time = time_mgmnt_gettime_u16- rpi_protocol_spi_interface.arrival_time;
	DEBUG_TRACE_word(rpi_protocol_spi_interface.arrival_time, "rpi_protocol_transmit_answer() - Time past since command has arrived and processed");
	#endif

	u8 answer_header[] = {
		(RPI_ANSWER_BUFFER_bytes_available() + 2),
		(rpi_protocol_spi_interface.command_code),
		(rpi_protocol_spi_interface.answer_status)
	};

	p_com_driver->clear_tx_buffer();
	p_com_driver->set_N_bytes(3, answer_header);

	u16 bytes_to_send = 3;

	RPI_ANSWER_BUFFER_start_read();
	u16 bytes_left = RPI_ANSWER_BUFFER_bytes_available();

	while (bytes_left != 0) {

		if (RPI_ANSWER_BUFFER_bytes_free() == 0) {
			break;
		}

		u16 read_length = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;
		u8 t_buffer[RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE];

		if (read_length > bytes_left) {
			read_length = bytes_left;
		}

		if (RPI_ANSWER_BUFFER_bytes_free() < read_length) {
			read_length = RPI_ANSWER_BUFFER_bytes_free();
		}

		RPI_ANSWER_BUFFER_get_N_bytes(read_length, t_buffer);
		p_com_driver->set_N_bytes(read_length, t_buffer);

		bytes_left -= read_length;
		bytes_to_send += read_length;
	}

	RPI_ANSWER_BUFFER_stop_read();
	RPI_STATUS_unset(RPI_STATUS_RESPONSE_PENDING);

	RPI_TRX_STATE error_code = RPI_TRX_STATE_COMPLETE;

	//DEBUG_PASS("rpi_protocol_transmit_answer() - answer is on it`s way");

	p_com_driver->start_tx();

	return error_code;
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

	READY_INOUT_drive_low();

	DEBUG_PASS("rpi_protocol_init() - CLIENT - ");

	RPI_STATUS_clear_all();

	rpi_protocol_spi_interface.command_length = 0;
	rpi_protocol_spi_interface.arrival_time = 0;

	RPI_COMMAND_BUFFER_init();
	RPI_ANSWER_BUFFER_init();

	RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL_init();
	RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL_init();
	RPI_PROTOCOL_ENTER_SLEEP_SIGNAL_init();
	RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL_init();

	driver_cfg.module.spi = _com_driver_cfg_spi;

	p_com_driver = p_driver;

	driver_mutex_id = p_com_driver->mutex_req();
	if (driver_mutex_id == MUTEX_INVALID_ID) {
		DEBUG_PASS("rpi_protocol_init() - Requesting MUTEX has FAILED !!! ---");
	}

	p_com_driver->configure(&driver_cfg);

	actual_task_state = MCU_TASK_SLEEPING;
	actual_state = RPI_STATE_IDLE;

	DEBUG_PASS("rpi_protocol_init() - FINISH");

	READY_INOUT_pull_up();

	mcu_task_controller_register_task(&rpi_protocol_task);
}

//-----------------------------------------------------------------------------

static void rpi_protocol_task_init(void) {

	DEBUG_PASS("rpi_protocol_task_init()");
}

static u16 rpi_protocol_task_get_schedule_interval(void) {
	if (actual_task_state != MCU_TASK_SLEEPING) {
		return 0;
	} else {
		return RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS;
	}
}

static MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	u8 is_ready = READY_INOUT_is_low_level();

	if ((actual_task_state == MCU_TASK_SLEEPING) && is_ready) {
		DEBUG_PASS("rpi_protocol_task_get_state() - Start-Signal detected - will enter Task-State RUNNING");
		actual_task_state = MCU_TASK_RUNNING;
	}

	return actual_task_state;
}

static void rpi_protocol_task_run(void) {

	RPI_TRX_STATE trx_state = RPI_TRX_STATE_BUSY;

	switch (actual_state) {

		default : 

			actual_state = RPI_STATE_IDLE;
			// no break;

		case RPI_STATE_IDLE :

			if (READY_INOUT_is_low_level() == 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_IDLE - No request - FALSE ALARM");
				actual_task_state = MCU_TASK_SLEEPING;
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_IDLE -> RPI_STATE_PREPARE");
			RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL_send(NULL);
			actual_state = RPI_STATE_PREPARE;
			// no break;

		case RPI_STATE_PREPARE :

			//DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_PREPARE -> RPI_STATE_PREPARE");

			p_com_driver->configure(&driver_cfg);
			p_com_driver->clear_rx_buffer();
			p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

			actual_state = RPI_STATE_TRANSMISSION;

			// no break;

		case RPI_STATE_TRANSMISSION :
			
			if (READY_INOUT_is_low_level() == 0) {

				DEBUG_PASS("rpi_protocol_task_run() - not requested anymore - RPI_STATE_TRANSMISSION -> RPI_ENTER_SLEEP");

				RPI_OP_TIMER_start();
				actual_state = RPI_ENTER_SLEEP;

				break;
			}

			if (RPI_STATUS_is_set(RPI_STATUS_RESPONSE_PENDING)) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_TRANSMISSION - Response available");
				RPI_STATUS_unset(RPI_STATUS_RESPONSE_PENDING);
				rpi_protocol_transmit_answer();
			}

			// block until command rx complete, device busy or timeout
			trx_state = rpi_protocol_receive_command();
			
			if (trx_state == RPI_TRX_STATE_TIMEOUT) {

				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_TRANSMISSION - invalid caommand received");
				RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL_send(NULL);
				break;

			} else if (trx_state == RPI_TRX_STATE_COMPLETE) {

			} else {
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - command received - RPI_STATE_TRANSMISSION -> RPI_STATE_COMMAND_RECEIVED");
			actual_state = RPI_STATE_COMMAND_RECEIVED;

			// no break;

		case RPI_STATE_COMMAND_RECEIVED :

			RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL_send((const void*)((const PROTOCOL_INTERFACE*)&rpi_protocol_spi_interface));
			p_com_driver->clear_rx_buffer();

			DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_COMMAND_RECEIVED -> RPI_STATE_TRANSMISSION");
			actual_state = RPI_STATE_TRANSMISSION;
			break;

		case RPI_ENTER_SLEEP:

			if (READY_INOUT_is_high_level() == 0) {

				DEBUG_PASS("rpi_protocol_task_run() - re-request - RPI_ENTER_SLEEP -> RPI_STATE_TRANSMISSION");

				actual_state = RPI_STATE_TRANSMISSION;
				break;
			}

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS) == 0) {
				//DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - still waiting for re-request");
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - request-timeout - RPI_STATE_FINISH -> RPI_STATE_IDLE");

			p_com_driver->stop_rx();
			p_com_driver->stop_tx();
			p_com_driver->shut_down();

			actual_state = RPI_STATE_IDLE;
			actual_task_state = MCU_TASK_SLEEPING;

			RPI_PROTOCOL_ENTER_SLEEP_SIGNAL_send(NULL);

			break;
	}
}
