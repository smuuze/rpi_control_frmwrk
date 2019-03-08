/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "system_interface.h"
#include "local_event_task.h"

#include "specific.h"
#include "local_context.h"
#include "io_controller.h"
#include "local_rtc_driver.h"

//---------- Implementation of Traces -----------------------------------------

//#define TRACES
//#include <traces.h>

#define TRACER_OFF
#include "tracer.h"

#define EVENT_TIMEOUT_MS	200
#define EVENT_QEUE_MAX_SIZE	10

typedef struct EVENT_QEUE_ELEMENT {
	SYSTEM_EVENT event_id;
	u32 timestamp;
} EVENT_QEUE_ELEMENT_TYPE;

/*!
 *
 */
typedef enum {
	EVENT_STATE_SLEEP,
	EVENT_STATE_ACTIVATE,
	EVENT_STATE_RESET_QEUE,
	EVENT_STATE_IDLE,
	EVENT_STATE_FINISH,
} EVENT_HANDLER_STATE;

static EVENT_HANDLER_STATE actual_task_state = EVENT_STATE_SLEEP;

static EVENT_QEUE_ELEMENT_TYPE _event_qeue[EVENT_QEUE_MAX_SIZE];

static u8 _event_counter = 0;

IO_CONTROLLER_BUILD_INOUT(EVENT_GPIO, EVENT_OUTPUT)

void local_event_mcu_task_init(void) {

	EVENT_GPIO_init();

	u8 i = 0;
	for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {
		_event_qeue[i].event_id = SYS_EVT_NO_EVENT;
		_event_qeue[i].timestamp = 0;
	}

	actual_task_state = EVENT_STATE_SLEEP;
}

MCU_TASK_INTERFACE_TASK_STATE local_event_mcu_task_get_state(void) {

	if (_event_counter != 0 || actual_task_state != EVENT_STATE_SLEEP) {

		PASS(); // local_event_mcu_task_is_runable() ---
		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

void local_event_mcu_task_run(void) {

	PASS(); // local_event_mcu_task_run() ---

	static u16 operation_timeout = 0;
	u8 i = 0;

	switch (actual_task_state) {

		case EVENT_STATE_SLEEP :

			if (_event_counter == 0) {
				break;
			}

			actual_task_state = EVENT_STATE_ACTIVATE;
			// no break;

		case EVENT_STATE_ACTIVATE :

			EVENT_GPIO_drive_high();

			operation_timeout = i_system.time.now_u16();
			actual_task_state = EVENT_STATE_ACTIVATE;

			// no break;

		case EVENT_STATE_RESET_QEUE :

			for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {

				if (_event_qeue[i].event_id == SYS_EVT_NO_EVENT) {
					continue;
				}

				if (i_system.time.isup_u32(_event_qeue[i].timestamp, EVENT_TIMEOUT_MS) != 0) {
					_event_qeue[i].event_id = SYS_EVT_NO_EVENT;
					_event_counter--;
					continue;
				}

				_event_qeue[i].event_id = SYS_EVT_NO_EVENT;
				_event_counter--;
			}

			actual_task_state = EVENT_STATE_ACTIVATE;
			// no break;

		case EVENT_STATE_IDLE :

			if (i_system.time.isup_u16(operation_timeout, EVENT_TIMEOUT_MS) == 0) {
				break;
			}

			EVENT_GPIO_no_drive();
			actual_task_state = EVENT_STATE_FINISH;
			// no break;

		default:
		case EVENT_STATE_FINISH :

			actual_task_state = EVENT_STATE_SLEEP;
			break;
	}
}

void local_event_mcu_task_background_run(void) {

}

void local_event_mcu_task_sleep(void) {

}

void local_event_mcu_task_wakeup(void) {

}

void local_event_mcu_task_finish(void) {

}

void local_event_mcu_task_terminate(void) {

}

void local_event_add(SYSTEM_EVENT event) {

	if (_event_counter == EVENT_QEUE_MAX_SIZE) {
		PASS(); // local_event_add() - Event qeue is full
	}

	TRACE_byte((u8)event); // local_event_add()

	u8 i = 0;
	for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {
		if (_event_qeue[i].event_id == SYS_EVT_NO_EVENT) {
			_event_qeue[i].event_id = event;
			_event_qeue[i].timestamp = i_system.time.now_u32();
			_event_counter++;
			return;
		}
	}
}

SYSTEM_EVENT local_event_get_next(void) {
	return SYS_EVT_NO_EVENT;
}
