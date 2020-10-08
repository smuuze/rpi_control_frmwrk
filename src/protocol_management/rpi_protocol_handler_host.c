 /*
  * \@file	protocol_management/rpi_protocl_handler_host.c
  * \author	sebastian lesse
  */

#define TRACER_ON

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

//-----------------------------------------------------------------------------

/*!
 *
 */
static void rpi_protocol_handler_host_SIGNAL_RECEIVED_SLOT(void* p_argument);

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_CMD_RECEIVED)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SIGNAL_CMD_RECEIVED, SLOT_CMD_RECEIVED, rpi_protocol_handler_host_SIGNAL_RECEIVED_SLOT)

//-----------------------------------------------------------------------------

/*!
 *
 */
static RPI_PROTOCOL_HANDLER_STATE actual_state = RPI_STATE_SLEEP;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE actual_task_state = MCU_TASK_UNINITIALIZED;

//-----------------------------------------------------------------------------

static void rpi_protocol_handler_host_SIGNAL_RECEIVED_SLOT(void* p_argument) {
	DEBUG_PASS("rpi_protocol_handler_host_SIGNAL_RECEIVED_SLOT()");
}

//-----------------------------------------------------------------------------

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

	(void) p_driver;

	DEBUG_PASS("rpi_protocol_init()");
}

void rpi_protocol_task_init(void) {

	DEBUG_PASS("rpi_protocol_task_init()");

	SIGNAL_CMD_RECEIVED_init();
	SLOT_CMD_RECEIVED_connect();

	actual_task_state = MCU_TASK_SLEEPING;

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

	if ((actual_task_state == MCU_TASK_SLEEPING) && RPI_OP_TIMER_is_up(3000)) {

		DEBUG_PASS("rpi_protocol_task_get_state() - will enter Task-State RUNNING");
		actual_task_state = MCU_TASK_RUNNING;
		RPI_OP_TIMER_start();
	}

	return actual_task_state;
}

void rpi_protocol_task_run(void) {

	DEBUG_PASS("rpi_protocol_task_run()");
	actual_task_state = MCU_TASK_SLEEPING;
	SIGNAL_CMD_RECEIVED_send(NULL);
}

u8 rpi_protocol_handler_get_actual_state(void) {
	return (u8) actual_state;
}