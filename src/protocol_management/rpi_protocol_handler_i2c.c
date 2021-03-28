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
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_DRIVER_TIMEOUT_MS		250

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_RELEASE_TIMEOUT_MS	15

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS		100

/*!
 *
 */
#define RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS		5

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	RPI_STATE_SLEEP,		//!< RPI_STATE_IDLE
	RPI_PREPARE_FOR_REQUEST,       	//!< RPI_PREPARE_FOR_REQUEST
	RPI_STATE_WAIT_FOR_REQUEST_RX,	//!< RPI_STATE_WAIT_FOR_REQUEST_RX
	RPI_STATE_PREPARE_RX,   	//!< RPI_STATE_GIVE_RESPONSE
	RPI_STATE_RX, 				//!< RPI_STATE_RECEIVE_COMMAND
	RPI_STATE_PROCESS_COMMAND, 	//!< RPI_STATE_PROCESS_COMMAND
	RPI_STATE_WAIT_FOR_REQUEST_TX,	//!< RPI_STATE_WAIT_FOR_REQUEST_RX
	RPI_STATE_TX,
	RPI_STATE_FINISH,
	RPI_STATE_WAIT_FOR_RELEASE	//!< RPI_STATE_WAIT_FOR_RELEASE
} RPI_PROTOCOL_HANDLER_STATE;

/*!
 *
 */
typedef enum {
	RPI_TRX_STATE_BUSY,
	RPI_TRX_STATE_COMPLETE,
	RPI_TRX_STATE_TIMEOUT
} RPI_TRX_STATE;

//-----------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , RPI_COMMAND_BUFFER, 32)
BUILD_LOCAL_MSG_BUFFER( , RPI_ANSWER_BUFFER,  32)

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_TRX_TIMER)

BUILD_MODULE_STATUS_FAST_VOLATILE(RPI_STATUS, 2)

//-----------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_ENTER_SLEEP_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL)

//-----------------------------------------------------------------------------

/*!
 *
 * @param err_code
 */
static void _set_finished_i2c(u8 err_code);

/*!
 *
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;

/*!
 *
 */
static CFG_DRIVER_I2C _com_driver_cfg_i2c = {
	.is_master = 0,
	.bit_rate = DRIVER_I2C_BITRATE_1KHZ,
	.enable_ack = 1,
	.interrupt_enable = 1,
	.answer_general_call = 1,
	.slave_addr = 0x11
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
static PROTOCOL_INTERFACE rpi_protocol_i2c_interface = {

	0, // commnand-length
	0, // command-code
	0, // answer-status
	0, // arrival-time
	&_set_finished_i2c,
	&rpi_cmd_handler_command_buffer,
	&rpi_cmd_handler_answer_buffer
};

//-----------------------------------------------------------------------------

static u8 driver_mutex_id = MUTEX_INVALID_ID;

//-----------------------------------------------------------------------------

/*!
 *
 * @param err_code
 */
static void _set_finished_i2c(u8 err_code) {

	if (RPI_STATUS_is_set(RPI_STATUS_COMMAND_PENDING) == 0) {
		return;
	}

	DEBUG_TRACE_byte(err_code, "_set_finished_i2c() - Error: ");

	rpi_protocol_i2c_interface.answer_status = err_code;

	RPI_STATUS_unset(RPI_STATUS_COMMAND_PENDING);
	RPI_STATUS_set(RPI_STATUS_ANSWER_PENDING);
}

/*!
 *
 */
static RPI_TRX_STATE rpi_protocol_receive_command(void) {

	// Das Macht die Kommunikation eher kaputt : p_com_driver->configure(&driver_cfg);
	//p_com_driver->clear_rx_buffer();
	//p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

	rpi_protocol_i2c_interface.command_length = 0;

	RPI_TRX_STATE error_code = RPI_TRX_STATE_COMPLETE;
			
	RPI_TRX_TIMER_start();

	//p_com_driver->configure(&driver_cfg);
	// - DO NOT CLEAR BUFFER, BECAUSE RX IS RUNNING IN BACKGROUND - p_com_driver->clear_rx_buffer();
	// - RX IS STARTED AT INIT - p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

	while (rpi_protocol_i2c_interface.command_length == 0) {

		if (RPI_TRX_TIMER_is_up(1000)) {
			DEBUG_PASS("rpi_protocol_receive_command() - Receiving command-length has FAILED (TIMEOUT) !!! ---");
			error_code = RPI_TRX_STATE_TIMEOUT;
			goto EXIT_rpi_protocol_receive_command_ON_ERROR;
		}

		// first byte gives the length of the command (how many bytes will follow)
		p_com_driver->wait_for_rx(1, 100); // blocking function

		if (p_com_driver->bytes_available() == 0) {
			DEBUG_PASS("rpi_protocol_receive_command() - No Data Bytes are available !!! ---");
			continue;
		}

		p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_i2c_interface.command_length);
		if (rpi_protocol_i2c_interface.command_length == 0) {
			DEBUG_PASS("rpi_protocol_receive_command() - Length is 0x00 !!! ---");
			continue;
		}

		if (rpi_protocol_i2c_interface.command_length == 0xFF) {
			DEBUG_PASS("rpi_protocol_receive_command() - Length is 0xFF !!! ---");
			rpi_protocol_i2c_interface.command_length = 0;
			continue;
		}
	}

	u8 bytes_available = 0;

	while (bytes_available < rpi_protocol_i2c_interface.command_length) {

		p_com_driver->wait_for_rx(rpi_protocol_i2c_interface.command_length, RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS); // blocking function
		bytes_available = p_com_driver->bytes_available();

		if (RPI_TRX_TIMER_is_up(1000)) {
			DEBUG_PASS("rpi_protocol_receive_command() - Receiving command-data has FAILED (TIMEOUT) !!! ---");
			error_code = RPI_TRX_STATE_TIMEOUT;
			goto EXIT_rpi_protocol_receive_command_ON_ERROR;
		}
	}

	p_com_driver->stop_rx();
	p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_i2c_interface.command_code);
	bytes_available -= 1;

	RPI_COMMAND_BUFFER_clear_all();
	RPI_COMMAND_BUFFER_start_write();

	while (bytes_available != 0) {

		u8 t_buffer[RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE];
		u8 read_count = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;

		if (read_count > p_com_driver->bytes_available()) {
			read_count = p_com_driver->bytes_available();
		}

		if (read_count > bytes_available) {
			read_count = bytes_available;
		}

		read_count = p_com_driver->get_N_bytes(read_count, t_buffer);
		bytes_available -= read_count;

		RPI_COMMAND_BUFFER_add_N_bytes(read_count, t_buffer);
	}

	RPI_COMMAND_BUFFER_stop_write();

	RPI_STATUS_set(RPI_STATUS_COMMAND_PENDING);

	//rpi_cmd_handler_set_request(&rpi_protocol_i2c_interface);
	RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL_send(&rpi_protocol_i2c_interface);
	//SIGNAL_CMD_RECEIVED_send(&rpi_protocol_i2c_interface);

	EXIT_rpi_protocol_receive_command_ON_SUCCESS :
	{
		return error_code;
	}

	EXIT_rpi_protocol_receive_command_ON_ERROR :
	{
		p_com_driver->stop_rx();
		return error_code;
	}
}

/*!
 *
 */
static RPI_TRX_STATE rpi_protocol_transmit_answer(void) {

	//DEBUG_PASS("rpi_protocol_prepare_answer()");

	if (RPI_STATUS_is_set(RPI_STATUS_ANSWER_PENDING) == 0) {
		goto rpi_protocol_transmit_answer_TRANSMIT_ANSWER;
	}

	#ifdef TRACES_ENABLED
	rpi_protocol_i2c_interface.arrival_time = time_mgmnt_gettime_u16- rpi_protocol_i2c_interface.arrival_time;
	DEBUG_TRACE_word(rpi_protocol_i2c_interface.arrival_time, "rpi_protocol_prepare_answer() - Time past since command has arrived and processed");
	#endif

	u8 answer_header[] = {
		(RPI_ANSWER_BUFFER_bytes_available() + 2),
		(rpi_protocol_i2c_interface.command_code),
		(rpi_protocol_i2c_interface.answer_status)
	};

	// Das Macht die Kommunikation eher kaputt : p_com_driver->configure(&driver_cfg);
	p_com_driver->clear_tx_buffer();
	p_com_driver->set_N_bytes(3, answer_header);

	u8 bytes_to_send = 3;

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
		bytes_to_send += read_length;
	}

	RPI_ANSWER_BUFFER_stop_read();
	RPI_STATUS_unset(RPI_STATUS_ANSWER_PENDING);

	p_com_driver->start_tx();

	rpi_protocol_transmit_answer_TRANSMIT_ANSWER:

	if (p_com_driver->is_ready_for_tx() == 0 ) {
		return RPI_TRX_STATE_BUSY;
	}

	return RPI_TRX_STATE_COMPLETE;
}

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	DEBUG_PASS("rpi_protocol_init() - I2C - START");

	RPI_STATUS_clear_all();

	rpi_protocol_i2c_interface.command_length = 0;
	rpi_protocol_i2c_interface.arrival_time = 0;

	RPI_COMMAND_BUFFER_init();
	RPI_ANSWER_BUFFER_init();

	RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL_init();
	RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL_init();
	RPI_PROTOCOL_ENTER_SLEEP_SIGNAL_init();
	RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL_init();
	//SIGNAL_CMD_RECEIVED_init();

	p_com_driver = p_driver;
	
	driver_mutex_id = p_com_driver->mutex_req();
	if (driver_mutex_id == MUTEX_INVALID_ID) {
		DEBUG_PASS("rpi_protocol_init() - Requesting MUTEX has FAILED !!! ---");
	}

	driver_cfg.module.i2c = _com_driver_cfg_i2c;

	p_com_driver->configure(&driver_cfg);
	p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

	actual_task_state = MCU_TASK_SLEEPING;
	actual_state = RPI_STATE_SLEEP;
	

	DEBUG_PASS("rpi_protocol_init() - I2C - FINISH");
}

void rpi_protocol_task_init(void) {

	DEBUG_PASS("rpi_protocol_task_init()");

	// only for debugging --- PROG_MISO_drive_low();
}

u16 rpi_protocol_task_get_schedule_interval(void) {
	if (actual_task_state != MCU_TASK_SLEEPING) {
		return 0;
	} else {
		return RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS;
	}
}

MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	if (p_com_driver == 0) {
		return MCU_TASK_SLEEPING;
	}

	u8 is_ready = p_com_driver->bytes_available(); // i2c_has_bytes_available()

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

	if (p_com_driver == 0) {
		return;
	}

	// actual state of the command receiver
	RPI_TRX_STATE trx_state = RPI_TRX_STATE_BUSY;

	switch (actual_state) {

		default : //break;
		case RPI_STATE_SLEEP : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_SLEEP");

			DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_SLEEP -> RPI_PREPARE_FOR_REQUEST");
		 	// only for debugging --- PROG_MISO_drive_low();
			// no break;

		case RPI_PREPARE_FOR_REQUEST : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_PREPARE_FOR_REQUEST");

			DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_PREPARE_FOR_REQUEST -> RPI_STATE_WAIT_FOR_REQUEST_RX");

			actual_state = RPI_STATE_WAIT_FOR_REQUEST_RX;
			actual_task_state = MCU_TASK_RUNNING;

			RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL_send(NULL);
			RPI_OP_TIMER_start(); // operation_timeout_ms = i_system.time.now_u16();

			// no break;

		case RPI_STATE_WAIT_FOR_REQUEST_RX : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_WAIT_FOR_REQUEST_RX");

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST_RX - OPERATION TIMEOUT!!! ---");
				DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_WAIT_FOR_REQUEST_RX -> RPI_STATE_FINISH");
				actual_state = RPI_STATE_FINISH;
				break;
			}

			DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_WAIT_FOR_REQUEST_RX -> RPI_STATE_RX");
			actual_state = RPI_STATE_RX;

			RPI_OP_TIMER_start();
			// no break;

		case RPI_STATE_RX :

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_DRIVER_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_RX - OPERATION TIMEOUT!!! ---");
				DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_RX -> RPI_STATE_FINISH");
				actual_state = RPI_STATE_FINISH;
				break;
			}

			// block until command rx complete, device busy or timeout
			trx_state = rpi_protocol_receive_command();

			if (trx_state == RPI_TRX_STATE_BUSY) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_RX - TRX-Driver is busy right now ..");
				break;
			}

			if (trx_state == RPI_TRX_STATE_TIMEOUT) {

				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_RX - Receiving command has FAILED (TIMEOUT) !!! --- ");
				DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_RX -> RPI_STATE_FINISH");
				actual_state = RPI_STATE_FINISH;

				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - change state - RPI_STATE_RX -> RPI_STATE_PROCESS_COMMAND");
			actual_state = RPI_STATE_PROCESS_COMMAND;

			RPI_OP_TIMER_start();
			break;

		case RPI_STATE_PROCESS_COMMAND : //DEBUG_PASS("rpi_protocol_task_run() - case RPI_STATE_PROCESS_COMMAND");

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - Command has TIMED OUT !!! ---");
				RPI_STATUS_unset(RPI_STATUS_ANSWER_PENDING);
				actual_state = RPI_STATE_FINISH;
				break;
			}

			if (RPI_STATUS_is_set(RPI_STATUS_ANSWER_PENDING) == 0) {
				//DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - Answer not finished yet");
				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - change state - RPI_STATE_PROCESS_COMMAND -> RPI_STATE_WAIT_FOR_REQUEST_TX");
			actual_state = RPI_STATE_WAIT_FOR_REQUEST_TX;

			RPI_OP_TIMER_start();
			// no break;

		case RPI_STATE_WAIT_FOR_REQUEST_TX:

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST_TX - OPERATION TIMEOUT!!! ---");
				DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_WAIT_FOR_REQUEST_TX -> RPI_STATE_FINISH");
				actual_state = RPI_STATE_FINISH;
				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - change state - RPI_STATE_WAIT_FOR_REQUEST_TX -> RPI_STATE_TX");
			actual_state = RPI_STATE_TX;

			RPI_OP_TIMER_start();
			// no break;

	 	case RPI_STATE_TX:

		 	// only for debugging --- PROG_MISO_drive_high();

			if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS) != 0) {
				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_TX - OPERATION TIMEOUT!!! ---");
				DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_TX -> RPI_STATE_FINISH");
				actual_state = RPI_STATE_FINISH;
				break;
			}

			// block until answer tx complete or timeout
			trx_state = rpi_protocol_transmit_answer();

			if (trx_state == RPI_TRX_STATE_BUSY) {
				//DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_TX - TRX-Driver is busy right now ..");
				break;
			}

			if (trx_state == RPI_TRX_STATE_TIMEOUT) {

				DEBUG_PASS("rpi_protocol_task_run() - RPI_STATE_TX - Transmitting answer has FAILED (TIMEOUT) !!! --- ");
				DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_TX -> RPI_STATE_FINISH");
				actual_state = RPI_STATE_FINISH;

				break;
			}

			DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_run() - change state - RPI_STATE_TX -> RPI_STATE_FINISH");
			actual_state = RPI_STATE_FINISH;

			break;

		case RPI_STATE_FINISH:

			p_com_driver->stop_tx();
			p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

		 	// only for debugging --- PROG_MISO_drive_low();

			DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_FINISH -> RPI_STATE_WAIT_FOR_RELEASE");
			actual_state = RPI_STATE_WAIT_FOR_RELEASE;

			break;

		case RPI_STATE_WAIT_FOR_RELEASE :

			DEBUG_PASS("rpi_protocol_task_run() - change state - RPI_STATE_WAIT_FOR_RELEASE -> RPI_STATE_SLEEP");

			RPI_PROTOCOL_ENTER_SLEEP_SIGNAL_send(NULL);

			actual_state = RPI_STATE_SLEEP;
			actual_task_state = MCU_TASK_SLEEPING;

			break;
	}
}

u8 rpi_protocol_handler_get_actual_state(void) {
	return (u8) actual_state;
}
