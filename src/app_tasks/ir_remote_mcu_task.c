/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_remote_mcu_task.c
 * \brief
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

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "driver/timer/timer0_driver.h"
#include "driver/timer/timer1_driver.h"

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#define IR_REMOTE_TASK_RUN_INTERVAL_MS			100
#define IR_REMOTE_TASK_CHANGE_FREQ_INTERVAL_MS		2000

// --------------------------------------------------------------------------------

#define IR_REMOTE_TASK_STATUS_TX_ACTIVE			(1 << 0)
#define IR_REMOTE_TASK_STATUS_CMD_PENDING		(1 << 1)
#define IR_REMOTE_TASK_STATUS_SAMSUNG_CMD_RECEIVED	(1 << 2)
#define IR_REMOTE_TASK_STATUS_JVC_CMD_RECEIVED		(1 << 3)

// --------------------------------------------------------------------------------

BUILD_MODULE_STATUS_U8(IR_REMOTE_TASK_STATUS)

// --------------------------------------------------------------------------------

static TIMER_INTERFACE_TYPE timer_carrier = {
	.init = &timer0_driver_init,
	.configure = &timer0_driver_configure,
	.start = &timer0_driver_start,
	.stop = &timer0_driver_stop
};

static TIMER_INTERFACE_TYPE timer_modulator = {
	.init = &timer1_driver_init,
	.configure = &timer1_driver_configure,
	.start = &timer1_driver_start,
	.stop = &timer1_driver_stop
};

// --------------------------------------------------------------------------------

#ifdef HAS_IR_PROTOCOL_SAMSUNG

#include "3rdparty/ir_protocol/ir_protocol_samsung.h"

static SAMSUNG_IR_PROTOCOL_COMMAND_TYPE samsung_ir_command;

static void ir_remote_task_slot_SAMSUNG_IR_CMD_RECEIVED(void* p_arg) {

	if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_SAMSUNG_CMD_RECEIVED)) {
		return;
	}

	DEBUG_PASS("ir_remote_task_slot_SAMSUNG_IR_CMD_RECEIVED()");

	SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command = (SAMSUNG_IR_PROTOCOL_COMMAND_TYPE*) p_arg;
	samsung_ir_command.address = p_command->address;
	samsung_ir_command.control = p_command->control;

	IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_SAMSUNG_CMD_RECEIVED | IR_REMOTE_TASK_STATUS_CMD_PENDING);
}

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SAMSUNG_IR_CMD_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SAMSUNG_IR_CMD_RECEIVED_SIGNAL, SAMSUNG_IR_CMD_RECEIVED_SLOT, ir_remote_task_slot_SAMSUNG_IR_CMD_RECEIVED)

#endif

// --------------------------------------------------------------------------------

#ifdef HAS_IR_PROTOCOL_JVC

#include "3rdparty/ir_protocol/ir_protocol_jvc.h"

static JVC_IR_PROTOCOL_COMMAND_TYPE jvc_ir_command;

static void ir_remote_task_slot_JVC_IR_CMD_RECEIVED(void* p_arg) {

	if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_JVC_CMD_RECEIVED)) {
		return;
	}

	DEBUG_PASS("ir_remote_task_slot_JVC_IR_CMD_RECEIVED()");

	JVC_IR_PROTOCOL_COMMAND_TYPE* p_command = (JVC_IR_PROTOCOL_COMMAND_TYPE*) p_arg;
	jvc_ir_command.address = p_command->address;
	jvc_ir_command.control = p_command->control;

	IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_JVC_CMD_RECEIVED | IR_REMOTE_TASK_STATUS_CMD_PENDING);
}

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(JVC_IR_CMD_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(JVC_IR_CMD_RECEIVED_SIGNAL, JVC_IR_CMD_RECEIVED_SLOT, ir_remote_task_slot_JVC_IR_CMD_RECEIVED)

#endif

// --------------------------------------------------------------------------------

void ir_remote_task_init(void) {

	DEBUG_PASS("ir_remote_task_init()");

	IR_REMOTE_TASK_STATUS_clear_all();

	IR_CARRIER_IN_no_pull();
	IR_CARRIER_OUT_drive_low();
	IR_MOD_OUT_drive_low();

	timer_carrier.init();
	timer_modulator.init();
	
	#ifdef HAS_IR_PROTOCOL_SAMSUNG
	{
		DEBUG_PASS("ir_remote_task_init() - Samsung");

		SAMSUNG_IR_CMD_RECEIVED_SIGNAL_init();
		SAMSUNG_IR_CMD_RECEIVED_SLOT_connect();
	
		ir_protocol_samsung_set_timer(&timer_carrier, &timer_modulator);
	}
	#endif
	
	#ifdef HAS_IR_PROTOCOL_JVC
	{
		DEBUG_PASS("ir_remote_task_init() - JVC");

		JVC_IR_CMD_RECEIVED_SIGNAL_init();
		JVC_IR_CMD_RECEIVED_SLOT_connect();
	
		ir_protocol_jvc_set_timer(&timer_carrier, &timer_modulator);
	}
	#endif
}

u16 ir_remote_task_get_schedule_interval(void) {

	if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE | IR_REMOTE_TASK_STATUS_CMD_PENDING)) {
		return 0;
	} else {
		return IR_REMOTE_TASK_RUN_INTERVAL_MS;
	}
}

MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void) {

	if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE | IR_REMOTE_TASK_STATUS_CMD_PENDING)) {
		return MCU_TASK_RUNNING;
	}
	
	return MCU_TASK_SLEEPING;
}

void ir_remote_task_run(void) {

	u8 is_active = 0;

	#ifdef HAS_IR_PROTOCOL_SAMSUNG
	if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_SAMSUNG_CMD_RECEIVED)) {

		if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE) == 0) {

			DEBUG_PASS("ir_remote_task_run() - Start Samsung IR-Command");

			IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE);
			IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_CMD_PENDING);

			ir_protocol_samsung_transmit(&samsung_ir_command);
			is_active = 1;

		} else  if (ir_protocol_samsung_is_busy()) {
			is_active = 1;

		} else {

			DEBUG_PASS("ir_remote_task_run() - Samsung IR-Command finished");
			IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_SAMSUNG_CMD_RECEIVED);
		}
	}
	#endif

	#ifdef HAS_IR_PROTOCOL_JVC
	if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_JVC_CMD_RECEIVED)) {

		if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE) == 0) {

			DEBUG_PASS("ir_remote_task_run() - Start Jvc IR-Command");

			IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE);
			IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_CMD_PENDING);

			ir_protocol_jvc_transmit(&jvc_ir_command);
			is_active = 1;

		} else  if (ir_protocol_jvc_is_busy()) {
			is_active = 1;

		} else {

			DEBUG_PASS("ir_remote_task_run() - Jvc IR-Command finished");
			IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_JVC_CMD_RECEIVED);
		}
	}
	#endif

	if (is_active == 0) {

		IR_CARRIER_IN_no_pull();
		IR_CARRIER_OUT_drive_low();
		IR_MOD_OUT_drive_low();

		DEBUG_PASS("ir_remote_task_run() - All operations finished");
		IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_TX_ACTIVE);
	}
}

void ir_remote_task_background_run(void) {

}

void ir_remote_task_sleep(void) {

}

void ir_remote_task_wakeup(void) {

}

void ir_remote_task_finish(void) {

}

void ir_remote_task_terminate(void) {

}
