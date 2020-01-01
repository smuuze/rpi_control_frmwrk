/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_ON

//-----------------------------------------------------------------------------

#include "config.h"
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"

#include "common/local_context.h"
#include "common/local_msg_buffer.h"
#include "common/local_module_status.h"
#include "common/local_mutex.h"

#include "io_management/io_controller.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "driver/driver_specific_spi.h"

#include "protocol_management/rpi_protocol_handler.h"
#include "command_handler/rpi_command_handler.h"
#include "command_management/protocol_interface.h"
#include "time_management/time_management.h"

//-----------------------------------------------------------------------------

#ifndef HAS_GPIO_READY_INOUT
#error DEFINE_PIN___READY_INOUT___IN_YOUR_BOARD_FILE
#endif

//-----------------------------------------------------------------------------

#define RPI_STATUS_COMMAND_PENDING				0
#define RPI_STATUS_ANSWER_PENDING				1

#define RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE			16

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
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_RELEASE_TIMEOUT_MS	15

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS		250

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	RPI_STATE_SLEEP,                  //!< RPI_STATE_IDLE
	RPI_PREPARE_FOR_REQUEST,       //!< RPI_PREPARE_FOR_REQUEST
	RPI_STATE_WAIT_FOR_REQUEST,//!< RPI_STATE_WAIT_FOR_REQUEST
	RPI_STATE_ACTIVATE_DRIVER,   //!< RPI_STATE_GIVE_RESPONSE
	RPI_STATE_START_DATA_EXCHANGE, //!< RPI_STATE_START_DATA_EXCHANGE
	RPI_STATE_DATA_EXCHANGE, //!< RPI_STATE_RECEIVE_COMMAND
	RPI_STATE_FINISH_DATA_EXCHANGE,//!< RPI_STATE_FINISH_DATA_EXCHANGE
	RPI_STATE_PROCESS_COMMAND, //!< RPI_STATE_PROCESS_COMMAND
	RPI_STATE_FINISH,                //!< RPI_STATE_FINISH
	RPI_STATE_WAIT_FOR_RELEASE,//!< RPI_STATE_WAIT_FOR_RELEASE
	RPI_STATE_CANCEL                 //!< RPI_STATE_CANCEL
} RPI_PROTOCOL_HANDLER_STATE;

/*!
 *
 */
typedef enum {
	RPI_CMD_RECEIVER_IDLE,              //!< RPI_CMD_RECEIVER_IDLE
	RPI_CMD_RECEIVER_WAIT_FOR_COMPLETION,//!< RPI_CMD_RECEIVER_WAIT_FOR_COMPLETION
	RPI_CMD_RECEIVER_COMPLETE
} RPI_CMD_RECEIVER_STATE;

//-----------------------------------------------------------------------------


BUILD_LOCAL_MSG_BUFFER( , RPI_COMMAND_BUFFER, 32)
BUILD_LOCAL_MSG_BUFFER( , RPI_ANSWER_BUFFER,  32)

TIME_MGMN_BUILD_STATIC_TIMER_U16(operation_timer)

BUILD_MODULE_STATUS_FAST_VOLATILE(rpi_status, 2)


/*!
 *
 */
static void _set_finished_debus(u8 err_code);

/*!
 *
 * @param err_code
 */
static void _set_finished_spi(u8 err_code);

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
static RPI_PROTOCOL_HANDLER_STATE actual_state = RPI_STATE_SLEEP;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE actual_task_state = MCU_TASK_UNINITIALIZED;

/*!
 *
 */
static u8 driver_mutex_id = MUTEX_INVALID_ID;

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
static PROTOCOL_INTERFACE rpi_protocol_debus_interface = {

	0, // commnand-length
	0, // command-code
	0, // answer-status
	0, // arrival-time
	&_set_finished_debus,
	&rpi_cmd_handler_command_buffer,
	&rpi_cmd_handler_answer_buffer
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
static void _set_finished_debus(u8 err_code) {

	#if defined HAS_DEBUS_INTERFACE && HAS_DEBUS_INTERFACE == 1
	{
		DEBUG_TRACE_byte(err_code, "rpi_protocol_set_finished_debus()");

		rpi_cmd_handler_set_unrequested();

		if (err_code != CMD_NO_ERR) {
			debus_error_message(ERR_INVALID_DATA);
			return;
		}

		debus_start_answer();
		debus_put_byte(RPI_ANSWER_BUFFER_bytes_available());
		debus_put_byte(rpi_protocol_debus_interface.command_code);
		debus_put_byte(err_code);

		RPI_ANSWER_BUFFER_start_read();

		while (RPI_ANSWER_BUFFER_bytes_available()) {
			debus_put_byte(RPI_ANSWER_BUFFER_get_byte());
		}

		RPI_ANSWER_BUFFER_stop_read();
		debus_stop_message();
	}
	#else
	(void) err_code;
	#endif
}

/*!
 *
 * @param err_code
 */
static void _set_finished_spi(u8 err_code) {

	DEBUG_TRACE_byte(err_code, "_set_finished_spi() - Error: ");

	rpi_protocol_spi_interface.answer_status = err_code;

	rpi_status_unset(RPI_STATUS_COMMAND_PENDING);
	rpi_status_set(RPI_STATUS_ANSWER_PENDING);
	rpi_cmd_handler_set_unrequested();
}

/*!
 *
 */
static RPI_CMD_RECEIVER_STATE _command_receiver(void) {

	//DEBUG_PASS("_com_driver_command_handler()");

	while (rpi_protocol_spi_interface.command_length == 0) {

		p_com_driver->wait_for_rx(1, 100); // blocking function
		p_com_driver->wait_for_tx(255, 100); // blocking function - sending everything that is left within the transmit-buffer

		u16 num_bytes_available = p_com_driver->bytes_available();
		if (num_bytes_available == 0) {
			DEBUG_PASS("_com_driver_command_handler() - Only one byte available -> no valid command yet");
			return RPI_CMD_RECEIVER_IDLE;
		}

		while (num_bytes_available-- != 0) {

			p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_length);
			if (rpi_protocol_spi_interface.command_length != 0) {
				break;
			}
		}

		if (rpi_protocol_spi_interface.command_length == 0xFF) {
			rpi_protocol_spi_interface.command_length = 0;
		}

		if (rpi_protocol_spi_interface.command_length == 0) {
			DEBUG_PASS("_com_driver_command_handler() - Command Length is 0");
			return RPI_CMD_RECEIVER_IDLE;
		}

		//TRACE_byte(rpi_protocol_spi_interface.command_length); // _com_driver_command_handler() - Command-Length
		RPI_COMMAND_BUFFER_clear_all();
	}

	p_com_driver->wait_for_rx(rpi_protocol_spi_interface.command_length, 100); // blocking function

	u8 bytes_available = p_com_driver->bytes_available();
	if (bytes_available < rpi_protocol_spi_interface.command_length) {
		DEBUG_PASS("_com_driver_command_handler() - Command not complete yet");
		DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "_com_driver_command_handler() - Command-Length");
		DEBUG_TRACE_byte(bytes_available, "_com_driver_command_handler() - Bytes available");
		return RPI_CMD_RECEIVER_WAIT_FOR_COMPLETION;
	}

	rpi_status_set(RPI_STATUS_COMMAND_PENDING);

	// Command Code
	p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_code);
	rpi_protocol_spi_interface.command_length -= 1;
	DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_code, "_com_driver_command_handler() - Command-Code");

	RPI_COMMAND_BUFFER_start_write();

	while (rpi_protocol_spi_interface.command_length != 0) {

		u8 t_buffer[RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE];
		u8 read_count = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;

		if (read_count > p_com_driver->bytes_available()) {
			read_count = p_com_driver->bytes_available();
		}

		if (read_count > rpi_protocol_spi_interface.command_length) {
			read_count = rpi_protocol_spi_interface.command_length;
		}

		read_count = p_com_driver->get_N_bytes(read_count, t_buffer);
		rpi_protocol_spi_interface.command_length -= read_count;

		RPI_COMMAND_BUFFER_add_N_bytes(read_count, t_buffer);

		DEBUG_TRACE_byte(read_count, "_com_driver_command_handler() - Bytes added");
		DEBUG_TRACE_N(read_count, t_buffer, "_com_driver_command_handler() - Command Data");
	}

	RPI_COMMAND_BUFFER_stop_write();

	p_com_driver->clear_buffer();

	DEBUG_PASS("_com_driver_command_handler() - Requesting Command-Handler");
	rpi_cmd_handler_set_request(&rpi_protocol_spi_interface);

	return RPI_CMD_RECEIVER_COMPLETE;
}

/*!
 *
 */
static void _com_driver_answer_handler(void) {

	DEBUG_PASS("_com_driver_answer_handler()");

	#ifdef TRACES_ENABLED
	rpi_protocol_spi_interface.arrival_time = time_mgmnt_gettime_u16- rpi_protocol_spi_interface.arrival_time;
	DEBUG_TRACE_word(rpi_protocol_spi_interface.arrival_time, "_com_driver_answer_handler() - Time past since command has arrived and processed");
	#endif

	u8 answer_header[] = {
		(RPI_ANSWER_BUFFER_bytes_available() + 2),
		(rpi_protocol_spi_interface.command_code),
		(rpi_protocol_spi_interface.answer_status)
	};
	DEBUG_TRACE_N(3, answer_header, "_com_driver_answer_handler() - Answer-Header (incl Length byte)");

	p_com_driver->set_N_bytes(3, answer_header);

	RPI_ANSWER_BUFFER_start_read();
	u8 bytes_left = RPI_ANSWER_BUFFER_bytes_available();

	while (bytes_left != 0) {

		if (RPI_ANSWER_BUFFER_bytes_free() == 0) {
			break;
		}

		u8 read_length = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;
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

		DEBUG_TRACE_N(read_length, t_buffer, "_com_driver_answer_handler() - Answer-Data");
	}

	RPI_ANSWER_BUFFER_stop_read();

	rpi_status_unset(RPI_STATUS_ANSWER_PENDING);

	rpi_protocol_spi_interface.arrival_time = 0;
}

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	READY_INOUT_drive_low();

	DEBUG_PASS("rpi_protocol_init() - START");

	rpi_status_clear_all();

	rpi_protocol_spi_interface.command_length = 0;
	rpi_protocol_spi_interface.arrival_time = 0;

	rpi_protocol_debus_interface.command_length = 0;
	rpi_protocol_debus_interface.arrival_time = 0;

	RPI_COMMAND_BUFFER_init();
	RPI_ANSWER_BUFFER_init();

	driver_cfg.module.spi = _com_driver_cfg_spi;

	p_com_driver = p_driver;

	actual_task_state = MCU_TASK_SLEEPING;
	actual_state = RPI_STATE_SLEEP;

	DEBUG_PASS("rpi_protocol_init() - FINISH");

	READY_INOUT_pull_up();
}

void rpi_protocol_task_init(void) {

	DEBUG_PASS("rpi_protocol_task_init()");
}

MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	u8 is_ready = READY_INOUT_is_low_level();

	if ((actual_task_state == MCU_TASK_SLEEPING) && is_ready) {
		DEBUG_PASS("rpi_protocol_task_get_state() - Start-Signal detected - will enter Task-State RUNNING");
		actual_task_state = MCU_TASK_RUNNING;
	}

	return actual_task_state;
}

void rpi_protocol_task_run(void) {

	#ifdef TRACES_ENABLED
	if (actual_task_state != RPI_STATE_SLEEP) {
		u32 actual_time = time_mgmnt_gettime_u32();
		DEBUG_TRACE_long(actual_time, "rpi_protocol_task_run() ------------------");
	}
	#endif

	// actual state of the command receiver
	RPI_CMD_RECEIVER_STATE cmd_receiver_state = RPI_CMD_RECEIVER_IDLE;

	switch (actual_state) {

		default : //break;
		case RPI_STATE_SLEEP : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_SLEEP");

			if (READY_INOUT_is_high_level()) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_SLEEP - No request - FALSE ALARM");
				actual_task_state = MCU_TASK_SLEEPING;
				break;
			}

			// no break;

		case RPI_PREPARE_FOR_REQUEST : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_PREPARE_FOR_REQUEST");

			DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_SLEEP - Request detected - Going to work");

			actual_state = RPI_STATE_WAIT_FOR_REQUEST;
			actual_task_state = MCU_TASK_RUNNING;
			driver_mutex_id = MUTEX_INVALID_ID;
			operation_timer_start(); // operation_timeout_ms = i_system.time.now_u16();

			// no break;

		case RPI_STATE_WAIT_FOR_REQUEST : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_WAIT_FOR_REQUEST");

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST - OPERATION TIMEOUT!!! ---");
				actual_state = RPI_STATE_SLEEP;
				actual_task_state = MCU_TASK_SLEEPING;
				break;
			}
			
			if (READY_INOUT_is_low_level()) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST - Ready Pin still low !!! ---");
				break;
			}

			if (driver_mutex_id == MUTEX_INVALID_ID) {

				driver_mutex_id = p_com_driver->mutex_req();
				if (driver_mutex_id == MUTEX_INVALID_ID) {
					DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST - Requesting MUTEX has FAILED !!! ---");
					break;
				}
			}

			actual_state = RPI_STATE_ACTIVATE_DRIVER;

			// no break;

		case RPI_STATE_ACTIVATE_DRIVER : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_ACTIVATE_DRIVER");

			p_com_driver->configure(&driver_cfg);

			if (rpi_status_is_set(RPI_STATUS_ANSWER_PENDING) != 0) {
				_com_driver_answer_handler();
				p_com_driver->start_tx();
			}

			p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

			actual_state = RPI_STATE_START_DATA_EXCHANGE;
			operation_timer_start();

			// no break;


		case RPI_STATE_START_DATA_EXCHANGE:

//			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_START_DATA_EXCHANGE_TIMEOUT_MS) == 0) {
//				break;
//			}

			DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_START_DATA_EXCHANGE - Starting data exchange"); 

			actual_state = RPI_STATE_DATA_EXCHANGE;
			operation_timer_start();


		case RPI_STATE_DATA_EXCHANGE : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_DATA_EXCHANGE");

			/*
			 * New Command data is received within this state
			 * a complete answer is also transmitted to the host
			 *
			 * How get i get noticed that the answer is read and no command was given
			 */

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_DATA_EXCHANGE - Receiving Command has Timed-Out !!! ---");
				actual_state = RPI_STATE_CANCEL;
				break;
			}

			READY_INOUT_drive_low();

			cmd_receiver_state = _command_receiver(); // this information has to be remember
			DEBUG_TRACE_byte(cmd_receiver_state, "rpi_protocol_task_run() - RPI_STATE_DATA_EXCHANGE - State of command-receiver:");

			if (cmd_receiver_state == RPI_CMD_RECEIVER_IDLE) {

				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_DATA_EXCHANGE - No Command data available");

				if (p_com_driver->is_ready_for_tx() != 0) {

					DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_DATA_EXCHANGE - No command received and no answer is pending");
					actual_state = RPI_STATE_CANCEL;
				}

				break;
			}

			if (cmd_receiver_state == RPI_CMD_RECEIVER_WAIT_FOR_COMPLETION) {

				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_DATA_EXCHANGE - Data Exchange still in Progress");
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_DATA_EXCHANGE - Command has been received");

			rpi_protocol_spi_interface.arrival_time = time_mgmnt_gettime_u16();
			actual_state = RPI_STATE_FINISH_DATA_EXCHANGE;

			break; // pause task so command handler can be executed

		case RPI_STATE_FINISH_DATA_EXCHANGE : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_FINISH_DATA_EXCHANGE");

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_FINISH_DATA_EXCHANGE - Finish Data-Exchange has Timed-Out !!! ---");
				actual_state = RPI_STATE_CANCEL;
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_FINISH_DATA_EXCHANGE - Still some answer bytes to read");
				break;
			}

			operation_timer_start();
			actual_state = RPI_STATE_PROCESS_COMMAND;
			// no break;

		case RPI_STATE_PROCESS_COMMAND : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_PROCESS_COMMAND");

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - Command has TIMED OUT !!! ---");
				rpi_status_unset(RPI_STATUS_ANSWER_PENDING);
				actual_state = RPI_STATE_CANCEL;
				break;
			}

			if (rpi_status_is_set(RPI_STATUS_COMMAND_PENDING) != 0) {

				if (rpi_status_is_set(RPI_STATUS_ANSWER_PENDING) == 0) {
					DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - Answer not finished yet");
					break;
				}

				rpi_status_unset(RPI_STATUS_COMMAND_PENDING);
			}

			actual_state = RPI_STATE_CANCEL;

			// no break;

		case RPI_STATE_CANCEL : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_CANCEL");

			DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_CANCEL - Going to stop operation");

			rpi_protocol_spi_interface.command_length = 0;
			cmd_receiver_state = RPI_CMD_RECEIVER_IDLE;
			actual_state = RPI_STATE_FINISH;

			READY_INOUT_pull_up();

			// no break; 

		case RPI_STATE_FINISH : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_FINISH");

			p_com_driver->stop_rx();
			p_com_driver->stop_tx();
			p_com_driver->mutex_rel(driver_mutex_id);
			p_com_driver->shut_down();

			actual_state = RPI_STATE_WAIT_FOR_RELEASE;

			// no break;

		case RPI_STATE_WAIT_FOR_RELEASE : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_WAIT_FOR_RELEASE");

			if (READY_INOUT_is_low_level()) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_RELEASE - Request Still pending");
				actual_state = RPI_PREPARE_FOR_REQUEST;
				break;
			}

			actual_state = RPI_STATE_SLEEP;
			actual_task_state = MCU_TASK_SLEEPING;

			break;
	}
}

void rpi_protocol_handler_debus_handler(void) {

	#if defined HAS_DEBUS_INTERFACE && HAS_DEBUS_INTERFACE == 1
	{
		if (rpi_protocol_debus_interface.command_length == 0) {

			rpi_protocol_debus_interface.command_length = debus_get_byte();
			if (rpi_protocol_debus_interface.command_length != 0xFF) {
				rpi_protocol_debus_interface.command_length = 0;
			}
		}

		if (rpi_protocol_debus_interface.command_length == 0) {
			return;
		}

		if (rpi_protocol_debus_interface.command_length != debus_bytes_available()) {
			return;
		}

		rpi_protocol_debus_interface.arrival_time = time_mgmnt_gettime_u16;
		TRACE_byte(rpi_protocol_debus_interface.command_length); // rpi_protocol_handler_debus_handler() - Command-Length

		rpi_protocol_debus_interface.command_code = debus_get_byte();
		TRACE_byte(rpi_protocol_debus_interface.command_code); // rpi_protocol_handler_debus_handler() - Command-Code

		u8 byte_count = debus_bytes_available();
		const u8* p_buffer = debus_get_pointer(byte_count);

		if (byte_count > RPI_COMMAND_BUFFER_size()) {
			byte_count = RPI_COMMAND_BUFFER_size();
		}

		rpi_status_set(RPI_STATUS_COMMAND_PENDING);

		RPI_COMMAND_BUFFER_clear_all();
		RPI_ANSWER_BUFFER_clear_all();

		RPI_COMMAND_BUFFER_start_write();
		RPI_COMMAND_BUFFER_add_N_bytes(byte_count, p_buffer);
		RPI_COMMAND_BUFFER_stop_write();

		rpi_cmd_handler_set_request(&rpi_protocol_debus_interface);
	}
	#endif
}

u8 rpi_protocol_handler_get_actual_state(void) {
	return (u8) actual_state;
}