/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "time_management/time_management.h"
#include "power_management/power_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

//TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)
//TIME_MGMN_BUILD_STATIC_TIMER_U16(operation_timer)

/*!
 *
 */
//typedef enum {
//	POWER_MGMTN_TASK_STATE_IDLE,
//	PCA9670_TASK_STATE_REQUEST_DRIVER
//} PCA9670_TASK_STATE;

/*!
 *
 */
//static MCU_TASK_INTERFACE_TASK_STATE task_state = MCU_TASK_TERMINATED;

/*!
 *
 */
///static PCA9670_TASK_STATE operation_stage = PCA9670_TASK_STATE_IDLE;

/*!
 *
 */
//static POWER_MANAGEMENT_UNIT_TYPE* p_first_instance = 0;

/*!
 *
 */
//static POWER_MANAGEMENT_UNIT_TYPE* p_last_instance = 0;

/*!
 *
 */
//static POWER_MANAGEMENT_UNIT_TYPE* p_act_instance = 0;


//-----------------------------------------------------------------------------


void power_mgmnt_init(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {
/*
	if (p_first_instance == 0) {
		p_first_instance = p_unit;
	}

	p_last_instance->next = p_unit;
	p_last_instance = p_last_instance->next;
	p_last_instance->next = 0;
*/
}

u8 power_mgmnt_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {

	PASS(); // power_mgmnt_release()
	
	if (p_unit->request_counter == 0) {	
		PASS(); // power_mgmnt_release() - Going to switch on unit
		
		p_unit->on();
		p_unit->status.is_ramp_up = 1;
		p_unit->switch_on_timestamp = time_mgmnt_gettime_u16();
	}
	
	p_unit->request_counter += 1;
}

/*!
 *
 */
void power_mgmnt_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {
	
	PASS(); // power_mgmnt_release()
	
	if (p_unit->request_counter == 0) {
	
		PASS(); // power_mgmnt_release() - Unit was not requested
		return;
		
	} else if (p_unit->request_counter == 1) {
	
		PASS(); // power_mgmnt_release() - The is the last release
		
		p_unit->off();
		p_unit->status.is_on = 0;
	}
	
	p_unit->request_counter -= 1;
}

/*!
 *
 */
u8 power_mgmnt_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {

	if (p_unit->status.is_ramp_up == 1) {
	
		if (time_mgmnt_istimeup_u16(p_unit->switch_on_timestamp, p_unit->power_up_time_ms) == 0) {
			return 0;
		}
			
		p_unit->status.is_on = 1;
		p_unit->status.is_ramp_up = 0;
	}

	return (p_unit->status.is_on != 0) ? 1 : 0;
}


// ---- Task Interface Implementation ------------------------------------------------------------

/*!
 *
 */
void power_management_task_init(void) {
/*
	operation_stage = PCA9670_TASK_STATE_IDLE;
*/
}

/*!
 *
 */
//MCU_TASK_INTERFACE_TASK_STATE power_management_task_get_state(void) {

/*
	if (task_state == MCU_TASK_SLEEPING) {

		if (task_timer_is_up(PCA9670_TASK_RUN_INTERVAL_MS) != 0) {
			task_state = MCU_TASK_RUNNING;
		}
	}

	return task_state;
*/
//	return MCU_TASK_SLEEPING;
//}

/*!
 *
 */
void power_management_task_run(void) {

/*
	#ifdef TRACES_ENABLED
	u16 actual_time = i_system.time.now_u16();
	TRACE_word(actual_time); // pca9670_task_run() -----------------
	#endif
		
	u8 command_buffer[PCA9670_COMMAND_BUFFER_SIZE];

	switch (operation_stage) {

		default:
			operation_stage = PCA9670_TASK_STATE_IDLE;
			// no break;

		case PCA9670_TASK_STATE_IDLE :
		
			if (task_state != MCU_TASK_RUNNING) {
				break;
			}
			
			if (p_first_instance == 0) {
				PASS(); // pca9670_task_run() - No instance registered - FINISH !!! ---
				operation_stage = PCA9670_TASK_STATE_REQUEST_FINISH;
				break;
			}
			
			p_act_instance = p_first_instance;
			operation_stage = PCA9670_TASK_STATE_REQUEST_DRIVER;
			operation_timer_start();
			// no break;
		
		case PCA9670_TASK_STATE_REQUEST_DRIVER :
		
			if (operation_timer_is_up(PCA9670_REQUEST_DRIVER_TIMEOUT_MS) != 0) {
				PASS(); // pca9670_task_run() - Waiting for driver request has timed out !!! ---
				operation_stage = PCA9670_TASK_STATE_CANCEL;
				break;
			}

			if ((com_driver_mutex_id = p_com_driver->mutex_req()) == 0) {
				PASS(); // pca9670_task_run() - Can't get Mutex of communication-driver
				break;
			}

			p_com_driver->configure(&driver_cfg);
			operation_timer_start();
			operation_stage = PCA9670_TASK_STATE_WRITE_IO;
			// no break;
			
		case PCA9670_TASK_STATE_WRITE_IO :
		
			if (operation_timer_is_up(PCA9670_DRIVER_REQUEST_TIMEOUT_MS) != 0) {
				PASS(); // pca9670_task_run() - PCA9670_TASK_STATE_WRITE_IO - Waiting for driver request has timed out !!! ---
				operation_stage = PCA9670_TASK_STATE_CANCEL;
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // pca9670_task_run() - PCA9670_TASK_STATE_WRITE_IO - Waiting for communication-driver
				break;
			}
				
			TRACE_byte(); // pca9670_task_run() - PCA9670_TASK_STATE_WRITE_IO - Going to handle instance
			
			PCA9670_GEN_COMMAND_WRITE_IO(p_act_instance, command_buffer);

			p_com_driver->clear_buffer();
			p_com_driver->set_address(p_act_instance->address);
			p_com_driver->set_N_bytes(PCA9670_WRITE_IO_COMMAND_LENGTH, command_buffer);
			p_com_driver->start_tx();
			
			operation_timer_start();	
			operation_stage = PCA9670_TASK_STATE_READ_IO;	
			// no break;
			
		case PCA9670_TASK_STATE_READ_IO :
		
			if (operation_timer_is_up(PCA9670_OPERATION_TIMEOUT_MS) != 0) {
				PASS(); // pca9670_task_run() - PCA9670_TASK_STATE_READ_IO - Waiting for driver operation has timed out !!! ---
				operation_stage = PCA9670_TASK_STATE_CANCEL;
				p_com_driver->stop_tx();
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // pca9670_task_run() - PCA9670_TASK_STATE_READ_IO - Waiting for communication-driver
				break;
			}

			p_com_driver->set_address(p_act_instance->address);
			p_com_driver->start_rx(PCA9670_READ_IO_ANSWER_LENGTH);
			
			operation_timer_start();
			operation_stage = PCA9670_TASK_STATE_PROCESS_DATA;	
			// no break;
			
		case  PCA9670_TASK_STATE_PROCESS_DATA :
		
			if (operation_timer_is_up(PCA9670_OPERATION_TIMEOUT_MS) != 0) {
				PASS(); // pca9670_task_run() - PCA9670_TASK_STATE_READ_IO - Waiting for driver operation has timed out !!! ---
				operation_stage = PCA9670_TASK_STATE_CANCEL;
				p_com_driver->stop_tx();
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // pca9670_task_run() - PCA9670_TASK_STATE_READ_IO - Waiting for communication-driver
				break;
			}			
			
			p_com_driver->get_N_bytes(PCA9670_READ_IO_ANSWER_LENGTH, command_buffer);		
			// no break;
			
		case PCA9670_TAST_STATE_LOAD_NEXT_INSTANCE :
		
			if (p_act_instance->next != 0) {
				
				PASS(); // pca9670_task_run() - PCA9670_TAST_STATE_LOAD_NEXT_INSTANCE - Loading next instance
				
				p_act_instance = p_act_instance->next;
				operation_stage = PCA9670_TASK_STATE_WRITE_IO;				
				operation_timer_start();				
				break;
			}
				
			PASS(); // pca9670_task_run() - PCA9670_TAST_STATE_LOAD_NEXT_INSTANCE - No more instance available			
			operation_stage = PCA9670_TASK_STATE_CANCEL;				
			// no break;
			
		case PCA9670_TASK_STATE_CANCEL :
		
			operation_stage = PCA9670_TASK_STATE_REQUEST_FINISH;
			p_com_driver->shut_down();
			p_com_driver->mutex_rel(com_driver_mutex_id);
			// no break;
		
		case PCA9670_TASK_STATE_REQUEST_FINISH :
		
			task_timer_start();
			operation_stage = PCA9670_TASK_STATE_IDLE;
			task_state = MCU_TASK_SLEEPING;
			
			break;
	}
*/
}

/*!
 *
 */
void power_management_task_background_run(void) {

}

/*!
 *
 */
void power_management_task_sleep(void) {

}

/*!
 *
 */
void power_management_task_wakeup(void) {

}

/*!
 *
 */
void power_management_task_finish(void) {

}

/*!
 *
 */
void power_management_task_terminate(void) {

}

