/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system_interface.h"
#include "local_event_task.h"
#include "time_management.h"

#include "specific.h"
#include "local_context.h"
#include "io_controller.h"
//#include "local_rtc_driver.h"

//-----------------------------------------------------------------------------

#define EVENT_RISE_TIME_MS	50
#define EVENT_TIMEOUT_MS	100
#define EVENT_QEUE_MAX_SIZE	10

//-----------------------------------------------------------------------------

typedef struct EVENT_QEUE_ELEMENT {
	SYSTEM_EVENT event_id;
	u32 timestamp;
} EVENT_QEUE_ELEMENT_TYPE;

/*!
 *
 */
typedef enum {
	EVENT_STATE_SLEEP = 0x00,
	EVENT_STATE_ACTIVATE,
	EVENT_STATE_RESET_QEUE,
	EVENT_STATE_WAIT_FOR_TIMEOUT,
	EVENT_STATE_FINISH,
} EVENT_HANDLER_STATE;

//-----------------------------------------------------------------------------

/*!
 *
 */
static EVENT_HANDLER_STATE actual_task_state = EVENT_STATE_SLEEP;

/*!
 *
 */
static EVENT_QEUE_ELEMENT_TYPE _event_qeue[EVENT_QEUE_MAX_SIZE];

/*!
 *
 */
static u8 _event_counter = 0;

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U32(operation_timer)

//-----------------------------------------------------------------------------

void local_event_mcu_task_init(void) {

	//EVENT_GPIO_init();
	//EVENT_GPIO_drive_low();

	//u8 i = 0;
	//for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {
	//	_event_qeue[i].event_id = SYS_EVT_NO_EVENT;
	//	_event_qeue[i].timestamp = 0;
	//}

	//actual_task_state = EVENT_STATE_SLEEP;
}

MCU_TASK_INTERFACE_TASK_STATE local_event_mcu_task_get_state(void) {

	if (_event_counter != 0 || actual_task_state != EVENT_STATE_SLEEP) {

		PASS(); // local_event_mcu_task_is_runable() --- 
		TRACE_byte(_event_counter); //
		TRACE_byte(actual_task_state); //

		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

void local_event_mcu_task_run(void) {

	u8 i = 0;

	switch (actual_task_state) {

		case EVENT_STATE_SLEEP :

			if (_event_counter == 0) {
				break;
			}

			PASS(); // local_event_mcu_task_run() - Event occured !!! ---
			actual_task_state = EVENT_STATE_ACTIVATE;
			// no break;

		case EVENT_STATE_ACTIVATE :

			#ifdef HAS_GPIO_EVENT_OUTPUT
			EVENT_OUTPUT_drive_high();
			#endif

			operation_timer_start();
			actual_task_state = EVENT_STATE_RESET_QEUE;

			// no break;

		case EVENT_STATE_RESET_QEUE :

			for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {

				if (_event_qeue[i].event_id == SYS_EVT_NO_EVENT) {
					continue;
				}

				if (i_system.time.isup_u32(_event_qeue[i].timestamp, EVENT_RISE_TIME_MS) == 0) {
					TRACE_byte(_event_qeue[i].event_id); // local_event_mcu_task_run() - Event still active !!! ---
					continue;
				}

				TRACE_byte(_event_qeue[i].event_id); // local_event_mcu_task_run() - Event removed !!! ---

				_event_qeue[i].event_id = SYS_EVT_NO_EVENT;
				_event_counter--;
			}

			actual_task_state = EVENT_STATE_WAIT_FOR_TIMEOUT;
			// no break;

		case EVENT_STATE_WAIT_FOR_TIMEOUT :

			if (operation_timer_is_up(EVENT_TIMEOUT_MS) == 0) {
				actual_task_state = EVENT_STATE_RESET_QEUE;
				break;
			}

			if (_event_counter != 0) {
				TRACE_byte(_event_counter); // local_event_mcu_task_run() - Event still available
				actual_task_state = EVENT_STATE_ACTIVATE;
				break;
			}

			actual_task_state = EVENT_STATE_FINISH;
			// no break;

		default:
		case EVENT_STATE_FINISH :

			PASS(); // local_event_mcu_task_run() - Event time is over

			#ifdef HAS_GPIO_EVENT_OUTPUT
			EVENT_OUTPUT_drive_low();
			#endif

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
