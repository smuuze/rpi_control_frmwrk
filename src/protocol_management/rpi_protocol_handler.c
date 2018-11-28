/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "debusapi.H"

#include "utils/stdmacros.h"

#include "system_interface.h"
#include "local_context.h"
#include "io_output_controller.h"


#include "local_msg_buffer.h"
#include "local_module_status.h"

#include "rpi_protocol_handler.h"
#include "rpi_command_handler.h"
#include "protocol_interface.h"
#include "trx_driver_interface.h"
#include "cfg_driver_interface.h"
#include "driver_specific_spi.h"

#define TRACES
#include <traces.h>

BUILD_MODULE_STATUS_FAST_VOLATILE(rpi_status, 2)

#define RPI_STATUS_CMD_BUSY		0
#define RPI_STATUS_ANSW_READY		1

#define RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE		16


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
 */
static IO_OUTPUT_DESCRIPTOR io_event_output_pin = {
	0, //u8 id;
	IO_TYPE_SYSTEM,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_system_output_busy_set, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

/*!
 *
 * @param err_code
 */
static void _set_finished_debus(u8 err_code) {

	#if SIO_FOR_HOST != '-'
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
	#endif
}

/*!
 *
 * @param err_code
 */
static void _set_finished_spi(u8 err_code) {

	TRACE_byte(err_code); // _set_finished_spi()

	rpi_protocol_spi_interface.answer_status = err_code;

	rpi_status_unset(RPI_STATUS_CMD_BUSY);
	rpi_status_set(RPI_STATUS_ANSW_READY);
	rpi_cmd_handler_set_unrequested();
}

/*!
 *
 */
static void _com_driver_command_handler(void) {

	//PASS(); // _com_driver_command_handler()


	if (rpi_protocol_spi_interface.command_length == 0) {

		u16 num_bytes_available = p_com_driver->bytes_available();
		if (num_bytes_available < 2) {
			return;
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
			return;
		}

		TRACE_byte(rpi_protocol_spi_interface.command_length); // _com_driver_command_handler() - Command-Length

		rpi_protocol_spi_interface.arrival_time = i_system.time.now_u16();
		io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_ON, 0, 0);
		RPI_COMMAND_BUFFER_clear_all();
	}


	if (p_com_driver->bytes_available() < rpi_protocol_spi_interface.command_length) {
		PASS(); // _com_driver_command_handler() - Command not complete yet
		return;
	}

	rpi_status_set(RPI_STATUS_CMD_BUSY);

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
}

/*!
 *
 */
static void _com_driver_answer_handler(void) {

	PASS(); // _com_driver_answer_handler()

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

	PASS(); // _com_driver_answer_handler() - start RX / TX
	p_com_driver->start_tx();

	io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_OFF, 0, 0);
	rpi_status_unset(RPI_STATUS_ANSW_READY);

	#ifdef TRACES_ENABLED
	rpi_protocol_spi_interface.arrival_time = i_system.time.now_u16() - rpi_protocol_spi_interface.arrival_time;
	TRACE_word(rpi_protocol_spi_interface.arrival_time); // _com_driver_answer_handler() - Time past since command has arrived and processed
	#endif

	rpi_protocol_spi_interface.arrival_time = 0;
}

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	PASS(); // rpi_protocol_init()

	GET_SYSTEM(SYS_OUTPUT).system_busy_output_01 = io_output_controller_register_output(&io_event_output_pin);
	io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_ON, 0, 0);

	rpi_status_clear_all();

	rpi_protocol_spi_interface.command_length = 0;
	rpi_protocol_spi_interface.arrival_time = 0;

	rpi_protocol_debus_interface.command_length = 0;
	rpi_protocol_debus_interface.arrival_time = 0;

	RPI_COMMAND_BUFFER_init();
	RPI_ANSWER_BUFFER_init();

	driver_cfg.module.spi = _com_driver_cfg_spi;

	p_com_driver = p_driver;
	p_com_driver->configure(&driver_cfg);
	p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

	io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_OFF, 0, 0);
}

void rpi_protocol_task_init(void) {

	PASS(); // rpi_protocol_task_init()
}

MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

	if (p_com_driver == 0) {
		return MCU_TASK_SLEEPING;
	}

	if (p_com_driver->bytes_available() != 0 && rpi_status_is_set(RPI_STATUS_CMD_BUSY) == 0) {
		PASS(); // rpi_protocol_task_get_state() - RX Complete
		return MCU_TASK_RUNNING;
	}

	if (rpi_status_is_set(RPI_STATUS_ANSW_READY) && p_com_driver->is_ready_for_tx()) {
		PASS(); // rpi_protocol_task_is_runable() - TX Complete and new answer pending
		return MCU_TASK_RUNNING;
	}

	if (rpi_protocol_debus_interface.arrival_time != 0 || rpi_protocol_spi_interface.arrival_time != 0) {
		PASS(); // rpi_protocol_task_is_runable() - There is a command operation pending
		return MCU_TASK_RUNNING;
	}

	return 0;
}

void rpi_protocol_task_run(void) {

	#ifdef TRACES_ENABLED
	u32 actual_time = i_system.time.now_u32();
	TRACE_long(actual_time); // rpi_protocol_task_run() ------------------
	#endif

	// --- only for debugging ---EVENT_IRQ_ON(); // --- only for debugging ---

	if (rpi_protocol_debus_interface.arrival_time != 0 && i_system.time.isup_u16(rpi_protocol_debus_interface.arrival_time, RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS)) {

		PASS(); // rpi_protocol_task_run() - Timeout on receiving command over DEBUS
		rpi_status_unset(RPI_STATUS_CMD_BUSY);
		rpi_protocol_debus_interface.command_length = 0;
		rpi_protocol_debus_interface.arrival_time = 0;
	}

	if (rpi_protocol_spi_interface.arrival_time != 0 && i_system.time.isup_u16(rpi_protocol_spi_interface.arrival_time, RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS)) {

		PASS(); // rpi_protocol_task_run() - Timeout on receiving command over SPI
		rpi_status_unset(RPI_STATUS_CMD_BUSY);
		rpi_protocol_spi_interface.command_length = 0;
		rpi_protocol_spi_interface.arrival_time = 0;

		p_com_driver->clear_buffer();
		io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).system_busy_output_01, IO_OUTPUT_STATE_OFF, 0, 0);
	}

	if (rpi_status_is_set(RPI_STATUS_ANSW_READY) && p_com_driver->is_ready_for_tx()) {
		_com_driver_answer_handler();

	} else if (p_com_driver->bytes_available() != 0 && rpi_status_is_set(RPI_STATUS_CMD_BUSY) == 0) {
		_com_driver_command_handler();
	}

	// --- only for debugging --- EVENT_IRQ_OFF(); // --- only for debugging ---
}

void rpi_protocol_handler_debus_handler(void) {

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

	rpi_status_set(RPI_STATUS_CMD_BUSY);

	RPI_COMMAND_BUFFER_clear_all();
	RPI_ANSWER_BUFFER_clear_all();

	RPI_COMMAND_BUFFER_start_write();
	RPI_COMMAND_BUFFER_add_N_bytes(byte_count, p_buffer);
	RPI_COMMAND_BUFFER_stop_write();

	rpi_cmd_handler_set_request(&rpi_protocol_debus_interface);
}
