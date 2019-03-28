/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "system_interface.h"
#include "local_context.h"
#include "io_controller.h"

#include "local_msg_buffer.h"
#include "local_module_status.h"
#include "local_mutex.h"

#include "rpi_protocol_handler.h"
#include "rpi_command_handler.h"
#include "protocol_interface.h"
#include "trx_driver_interface.h"
#include "cfg_driver_interface.h"
#include "driver_specific_spi.h"
#include "time_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

BUILD_MODULE_STATUS_FAST_VOLATILE(rpi_status, 2)

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
#define RPI_PROTOCOL_HANDLER_START_DATA_EXCHANGE_TIMEOUT_MS	5

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
#define RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS		3000

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


/*!
 *
 */
static void _set_finished_debus(u8 err_code);

/*!
 *
 * @param err_code
 */
static void _set_finished_spi(u8 err_code);

BUILD_LOCAL_MSG_BUFFER( , RPI_COMMAND_BUFFER, 32)
BUILD_LOCAL_MSG_BUFFER( , RPI_ANSWER_BUFFER,  32)

IO_CONTROLLER_BUILD_INOUT(IS_READY, READY_INOUT)

TIME_MGMN_BUILD_STATIC_TIMER_U16(operation_timer)

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
MCU_TASK_INTERFACE_TASK_STATE actual_task_state = MCU_TASK_UNINITIALIZED;

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

/*!
 *
 * @param err_code
 */
static void _set_finished_debus(u8 err_code) {

	#if defined HAS_DEBUS_INTERFACE && HAS_DEBUS_INTERFACE == 1
	{
		TRACE_byte(err_code); // rpi_protocol_set_finished_debus()

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

	TRACE_byte(err_code); // _set_finished_spi()

	rpi_protocol_spi_interface.answer_status = err_code;

	rpi_status_unset(RPI_STATUS_COMMAND_PENDING);
	rpi_status_set(RPI_STATUS_ANSWER_PENDING);
	rpi_cmd_handler_set_unrequested();
}

/*!
 *
 */
static RPI_CMD_RECEIVER_STATE _command_receiver(void) {

	PASS(); // _com_driver_command_handler()

	while (rpi_protocol_spi_interface.command_length == 0) {

		p_com_driver->wait_for_rx(1, 100); // blocking function
		p_com_driver->wait_for_tx(255, 100); // blocking function

		u16 num_bytes_available = p_com_driver->bytes_available();
		if (num_bytes_available == 0) {
			PASS(); // _com_driver_command_handler() - Only one byte available -> no valid command yet
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
			PASS(); // _com_driver_command_handler() - Command Length is 0
			return RPI_CMD_RECEIVER_IDLE;
		}

		TRACE_byte(rpi_protocol_spi_interface.command_length); // _com_driver_command_handler() - Command-Length
		RPI_COMMAND_BUFFER_clear_all();
	}

	p_com_driver->wait_for_rx(rpi_protocol_spi_interface.command_length, 100); // blocking function

	if (p_com_driver->bytes_available() < rpi_protocol_spi_interface.command_length) {
		PASS(); // _com_driver_command_handler() - Command not complete yet
		return RPI_CMD_RECEIVER_WAIT_FOR_COMPLETION;
	}

	rpi_status_set(RPI_STATUS_COMMAND_PENDING);

	// Command Code
	p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_code);
	rpi_protocol_spi_interface.command_length -= 1;
	TRACE_byte(rpi_protocol_spi_interface.command_code); // _com_driver_command_handler() - Command-Code

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

		TRACE_byte(read_count); // _com_driver_command_handler() - Bytes added
		TRACE_N(read_count, t_buffer); // _com_driver_command_handler() - Command Data
	}

	RPI_COMMAND_BUFFER_stop_write();

	p_com_driver->clear_buffer();

	PASS(); // _com_driver_command_handler() - Requesting Command-Handler
	rpi_cmd_handler_set_request(&rpi_protocol_spi_interface);

	return RPI_CMD_RECEIVER_COMPLETE;
}

/*!
 *
 */
static void _com_driver_answer_handler(void) {

	PASS(); // _com_driver_answer_handler()

	#ifdef TRACES_ENABLED
	rpi_protocol_spi_interface.arrival_time = i_system.time.now_u16() - rpi_protocol_spi_interface.arrival_time;
	TRACE_word(rpi_protocol_spi_interface.arrival_time); // _com_driver_answer_handler() - Time past since command has arrived and processed
	#endif

	u8 answer_header[] = {
		(RPI_ANSWER_BUFFER_bytes_available() + 2),
		(rpi_protocol_spi_interface.command_code),
		(rpi_protocol_spi_interface.answer_status)
	};
	TRACE_N(3, answer_header); // Answer-Header (incl Length byte)

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

		TRACE_N(read_length, t_buffer); // Answer-Data
	}

	RPI_ANSWER_BUFFER_stop_read();

//	PASS(); // _com_driver_answer_handler() - start RX / TX
//	p_com_driver->start_tx();

	io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_OFF, 0, 0);
	rpi_status_unset(RPI_STATUS_ANSWER_PENDING);

	rpi_protocol_spi_interface.arrival_time = 0;
}

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	PASS(); // rpi_protocol_init()

	IS_READY_init();

	//config_IS_READY_IDLE; // OUTPUT_ON
	//IS_READY_drive_high();

	//GET_SYSTEM(SYS_OUTPUT).system_busy_output_01 = io_output_controller_register_output(&io_event_output_pin);
	//io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_ON, 0, 0);

	rpi_status_clear_all();

	rpi_protocol_spi_interface.command_length = 0;
	rpi_protocol_spi_interface.arrival_time = 0;

	rpi_protocol_debus_interface.command_length = 0;
	rpi_protocol_debus_interface.arrival_time = 0;

	RPI_COMMAND_BUFFER_init();
	RPI_ANSWER_BUFFER_init();

	driver_cfg.module.spi = _com_driver_cfg_spi;

	p_com_driver = p_driver;

//	while ((driver_mutex_id = p_com_driver->mutex_req()) == MUTEX_INVALID_ID);
//	p_com_driver->configure(&driver_cfg);
//	p_com_driver->shut_down();
//	p_com_driver->mutex_rel(driver_mutex_id);

	actual_task_state = MCU_TASK_SLEEPING;
	actual_state = RPI_STATE_SLEEP;

	PASS(); // rpi_protocol_init() - Finish

	//IS_READY_drive_low();
	IS_READY_no_drive();

	//io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_OFF, 0, 0);
}

void rpi_protocol_task_init(void) {

	PASS(); // rpi_protocol_task_init()
}

MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	if (IS_READY_is_high_level()) {
		actual_task_state = MCU_TASK_RUNNING;
	}

	return actual_task_state;
}

void rpi_protocol_task_run(void) {

	#ifdef TRACES_ENABLED
	if (actual_task_state != RPI_STATE_SLEEP) {
		u32 actual_time = i_system.time.now_u32();
		TRACE_long(actual_time); // rpi_protocol_task_run() ------------------
	}
	#endif

	// --- only for debugging ---EVENT_IRQ_ON(); // --- only for debugging ---

	// timeout for actual task operation
	//static u16 operation_timeout_ms;

	// actual state of the command receiver
	RPI_CMD_RECEIVER_STATE cmd_receiver_state = RPI_CMD_RECEIVER_IDLE;

	switch (actual_state) {

		default : //break;
		case RPI_STATE_SLEEP : PASS(); //

			//if (IS_READY_PIN() == 0) {
			if (IS_READY_is_low_level()) {
				PASS(); // rpi_protocol_task_run() - RPI_STATE_SLEEP - No request - FALSE ALARM
				actual_task_state = RPI_STATE_SLEEP;
				break;
			}

			// no break;

		case RPI_PREPARE_FOR_REQUEST :

			PASS(); // rpi_protocol_task_run() - RPI_STATE_SLEEP - Request detected - Going to work

			actual_state = RPI_STATE_WAIT_FOR_REQUEST;
			actual_task_state = MCU_TASK_RUNNING;
			driver_mutex_id = MUTEX_INVALID_ID;
			operation_timer_start(); // operation_timeout_ms = i_system.time.now_u16();

			// no break;

		case RPI_STATE_WAIT_FOR_REQUEST : PASS(); //

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS)/* i_system.time.isup_u16(operation_timeout_ms, RPI_PROTOCOL_HANDLER_WAIT_FOR_REQUEST_TIMEOUT_MS)*/ != 0) {
				PASS(); // rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST - OPERATION TIMEOUT!!! ---
				actual_state = RPI_STATE_SLEEP;
				break;
			}

			if (driver_mutex_id == MUTEX_INVALID_ID) {

				driver_mutex_id = p_com_driver->mutex_req();
				if (driver_mutex_id == MUTEX_INVALID_ID) {
					PASS(); // rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST - Requesting MUTEX has FAILED !!! ---
					break;
				}
			}

			//if (IS_READY_PIN() == 0) {
			if (IS_READY_is_low_level()) {
				PASS(); // rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_REQUEST - Ready Pin low !!! ---
			}

			actual_state = RPI_STATE_ACTIVATE_DRIVER;

			// no break;

		case RPI_STATE_ACTIVATE_DRIVER : PASS(); //

			p_com_driver->configure(&driver_cfg);

			if (rpi_status_is_set(RPI_STATUS_ANSWER_PENDING) != 0) {
				_com_driver_answer_handler();
				p_com_driver->start_tx();
			}

			p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

			actual_state = RPI_STATE_DATA_EXCHANGE;
			operation_timer_start(); // operation_timeout_ms = i_system.time.now_u16();

			//------------------config_IS_READY_ENABLE; // OUTPUT_OFF
			IS_READY_drive_low();

			// no break;


//		case RPI_STATE_START_DATA_EXCHANGE: PASS(); //
//
//			// Exchange length Byte
//			// - how many bytes do we have to receive
//			// how many byte we will send
//
////			if (i_system.time.isup_u16(operation_timeout_ms, RPI_PROTOCOL_HANDLER_START_DATA_EXCHANGE_TIMEOUT_MS) == 0) {
////				PASS(); // rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - Wait some Time to receive the first bits
////				break;
////			}
//
//			operation_timeout_ms = i_system.time.now_u16();
//			actual_state = RPI_STATE_DATA_EXCHANGE;
//			break; // leave task and let driver receive some bytes

		case RPI_STATE_DATA_EXCHANGE : PASS(); //

			/*
			 * New Command data is received within this state
			 * a complete answer is also transmitted to the host
			 *
			 * How get i get noticed that the answer is read and no command was given
			 */

//			if (i_system.time.isup_u16(operation_timeout_ms, RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS) != 0) {
//				PASS(); // rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - Receiving Command has Timed-Out !!! ---
//				actual_state = RPI_STATE_CANCEL;
//				break;
//			}

			PASS(); // rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - Get state of
			cmd_receiver_state = _command_receiver(); // this information has to be remember

			if (cmd_receiver_state == RPI_CMD_RECEIVER_IDLE) {

				PASS(); // rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - No Command data available

				if (p_com_driver->is_ready_for_tx() != 0) {

					PASS(); //  rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - No command received and no answer is pending
					actual_state = RPI_STATE_CANCEL; // RPI_STATE_FINISH_DATA_EXCHANGE;
				}

				break;
			}

			if (cmd_receiver_state == RPI_CMD_RECEIVER_WAIT_FOR_COMPLETION) {

				PASS(); // rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - Data Exchange still in Progress
				break;
			}

			PASS(); // rpi_protocol_task_run() - RPI_STATE_EXCHANGE_DATA - Command has been received

			rpi_protocol_spi_interface.arrival_time = i_system.time.now_u16();
			actual_state = RPI_STATE_FINISH_DATA_EXCHANGE;

			//io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_ON, 0, 0);

			break; // leave task so command handler can be executed

		case RPI_STATE_FINISH_DATA_EXCHANGE :

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS) /*i_system.time.isup_u16(operation_timeout_ms, RPI_PROTOCOL_HANDLER_DATA_EXCHANGE_TIMEOUT_MS)*/ != 0) {
				PASS(); // rpi_protocol_task_run() - RPI_STATE_FINISH_DATA_EXCHANGE - Finish Data-Exchange has Timed-Out !!! ---
				actual_state = RPI_STATE_CANCEL;
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); //  rpi_protocol_task_run() - RPI_STATE_FINISH_DATA_EXCHANGE - Still some answer bytes to read
				break;
			}

			operation_timer_start(); // operation_timeout_ms = i_system.time.now_u16();
			actual_state = RPI_STATE_PROCESS_COMMAND;
			// no break;

		case RPI_STATE_PROCESS_COMMAND : PASS(); //

			if (operation_timer_is_up(RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS) /*i_system.time.isup_u16(operation_timeout_ms, RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS) */ != 0) {
				PASS(); // rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - Command has TIMED OUT !!! ---
				rpi_status_unset(RPI_STATUS_ANSWER_PENDING);
				actual_state = RPI_STATE_CANCEL;
				break;
			}

			if (rpi_status_is_set(RPI_STATUS_COMMAND_PENDING) != 0) {

				if (rpi_status_is_set(RPI_STATUS_ANSWER_PENDING) == 0) {
					PASS(); // rpi_protocol_task_run() - RPI_STATE_PROCESS_COMMAND - Answer not finished yet
					break;
				}

				rpi_status_unset(RPI_STATUS_COMMAND_PENDING);
			}

			//_com_driver_answer_handler();
			actual_state = RPI_STATE_CANCEL;

			// no break;

		case RPI_STATE_CANCEL : PASS(); //

			rpi_protocol_spi_interface.command_length = 0;
			//rpi_protocol_spi_interface.arrival_time = 0;
			cmd_receiver_state = RPI_CMD_RECEIVER_IDLE;
			actual_state = RPI_STATE_FINISH;

			//------------------config_IS_READY_IDLE; // OUTPUT_ON
			IS_READY_drive_high();

			// no break; // Leave Task and let IO-Controller set Ouputs

		case RPI_STATE_FINISH : PASS(); //

			p_com_driver->stop_rx();
			p_com_driver->stop_tx();
			p_com_driver->mutex_rel(driver_mutex_id);
			p_com_driver->shut_down();

			actual_state = RPI_STATE_WAIT_FOR_RELEASE;

			//------------------config_IS_READY_DISABLE; // INPUT_HIGH_Z
			IS_READY_no_drive();

			// no break; // Leave Task and let IO-Controller set Ouputs

		case RPI_STATE_WAIT_FOR_RELEASE : PASS(); //

			//if (IS_READY_PIN() != 0) {
			if (IS_READY_is_high_level()) {
				PASS(); // rpi_protocol_task_run() - RPI_STATE_WAIT_FOR_RELEASE - Request Still pending
				actual_state = RPI_PREPARE_FOR_REQUEST;
				break;
			}

			actual_state = RPI_STATE_SLEEP;
			actual_task_state = MCU_TASK_SLEEPING;

			break;
	}

	// --- only for debugging --- EVENT_IRQ_OFF(); // --- only for debugging ---
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

		rpi_protocol_debus_interface.arrival_time = i_system.time.now_u16();
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
