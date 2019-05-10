/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "cfg_driver_interface.h"
#include "system_interface.h"

#include "driver_PC9679.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	PCA9679_TASK_STATE_IDLE,
	PCA9679_TASK_STATE_REQUEST_DRIVER,
	PCA9679_TASK_STATE_CANCEL,
	PCA9679_TASK_STATE_REQUEST_FINISH,
	PCA9679_TASK_STATE_READ_IO,
	PCA9679_TASK_STATE_WRITE_IO
} PCA9679_TASK_STATE;

//-----------------------------------------------------------------------------

/*!
 *
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;	// pointer to call functions from driver through the interface

/*!
 *
 */
static TRX_DRIVER_CONFIGURATION driver_cfg;

/*!
 *
 */
static PCA9679_INSTANCE_TYPE* p_first_instance = 0;

/*!
 *
 */
static PCA9679_INSTANCE_TYPE* p_last_instance = 0;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE task_state = MCU_TASK_TERMINATED;

/*!
 *
 */
static PCA9679_TASK_STATE operation_stage = PCA9679_TASK_STATE_IDLE;

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)
TIME_MGMN_BUILD_STATIC_TIMER_U16(operation_timer)

//-----------------------------------------------------------------------------

static u8 _update_mask(u8 act_mask, u8 pin, u8 value) {

	u8 new_mask = act_mask;

	if (value != 0) {
		new_mask |= (1 << pin);
	} else {
		new_mask &= ~(1 << pin);
	}

	return new_mask;
}

static u8 _bit_isset(u8 vector, u8 bit) {
	return (vector & (1 << bit)) != 0 ? 1 : 0;
}

//-----------------------------------------------------------------------------

void local_sht31_module_init(TRX_DRIVER_INTERFACE* p_driver) {

	PASS(); // local_sht31_module_init()

	driver_cfg.module.i2c.answer_general_call = 0;
	driver_cfg.module.i2c.bit_rate = DRIVER_I2C_BITRATE_1KHZ;
	driver_cfg.module.i2c.enable_ack = 0;
	driver_cfg.module.i2c.interrupt_enable = 1;
	driver_cfg.module.i2c.is_master = 1;
	driver_cfg.module.i2c.slave_addr = 0;

	p_com_driver = p_driver;
}

void pca_9679_register_module(PCA9679_INSTANCE_TYPE* p_instance) {

	if (p_first_instance == 0) {
		p_first_instance = p_instance;
	}

	p_last_instance->next = p_instance;
	p_last_instance = p_last_instance->next;
	p_last_instance->next = 0;
}


void pca9679_set_direction(u8 instance_address, u8 instance_pin_number, u8 new_direction) {

	PCA9679_INSTANCE_TYPE* p_act = p_first_instance;

	while (p_act != 0) {

		if (p_act->address != instance_address) {
			p_act = p_act->next;
			continue;
		}

		p_act->direction_mask  = _update_mask(p_act->direction_mask, instance_pin_number, new_direction);

		break;
	}
}


u8 pca9679_get_direction(u8 instance_address, u8 instance_pin_number) {

	PCA9679_INSTANCE_TYPE* p_act = p_first_instance;

	while (p_act != 0) {

		if (p_act->address != instance_address) {
			p_act = p_act->next;
			continue;
		}
	
		if (_bit_isset(p_act->direction_mask, instance_pin_number) != 0) {
			return PCA9679_DIRECTION_OUTPUT;
		} else {
			return CA9679_DIRECTION_INPUT;
		}
	}
	
	return PCA9679_DIRECTION_UNDEFINED;
}


void pca9679_set_state(u8 instance_address, u8 instance_pin_number, u8 new_state) {

	PCA9679_INSTANCE_TYPE* p_act = p_first_instance;

	while (p_act != 0) {

		if (p_act->address != instance_address) {
			p_act = p_act->next;
			continue;
		}

		p_act->level_mask  = _update_mask(p_act->level_mask, instance_pin_number, new_state);

		break;
	}
}


u8 pca9679_get_state(u8 instance_address, u8 instance_pin_number) {

	PCA9679_INSTANCE_TYPE* p_act = p_first_instance;

	while (p_act != 0) {

		if (p_act->address != instance_address) {
			p_act = p_act->next;
			continue;
		}
	
		if (_bit_isset(p_act->level_mask, instance_pin_number) != 0) {
			return PCA9679_STATE_HIGH;
		} else {
			return PCA9679_STATE_LOW;
		}
	}
	
	return PCA9679_STATE_UNKNOWN;
}

//-----------------------------------------------------------------------------

/*!
 *
 */
void pca9679_task_init(void) {
	
	operation_stage = PCA9679_TASK_STATE_IDLE;
}

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE pca9679_task_get_state(void) {

	if (task_state == MCU_TASK_SLEEPING) {

		if (task_timer_is_up(PCA9679_TASK_RUN_INTERVAL_MS) /* i_system.time.isup_u16(task_run_interval_reference_actual, SHT31_TASK_RUN_INTERVAL_MS) */ != 0) {
			task_state = MCU_TASK_RUNNING;
		}
	}

	return task_state;
}

/*!
 *
 */
void pca9679_task_run(void) {	

	#ifdef TRACES_ENABLED
	u16 actual_time = i_system.time.now_u16();
	TRACE_word(actual_time); // pca9679_task_run() -----------------
	#endif

	switch (operation_stage) {

		default:
			operation_stage = PCA9679_TASK_STATE_IDLE;
			// no break;

		case PCA9679_TASK_STATE_IDLE :
		
			if (task_state != MCU_TASK_RUNNING) {
				break;
			}
			
			operation_timer_start();
			// no break;
		
		case PCA9679_TASK_STATE_REQUEST_DRIVER :
		
			if (operation_timer_is_up(PCA9679_REQUEST_DRIVER_TIMEOUT_MS) == 0) {
				PASS(); // pca9679_task_run() - Waiting for driver request has timed out !!! ---
				operation_stage = PCA9679_TASK_STATE_CANCEL;
				break;
			}

			if ((com_driver_mutex_id = p_com_driver->mutex_req()) == 0) {
				PASS(); // pca9679_task_run() - Can't get Mutex of communication-driver
				break;
			}

			p_com_driver->configure(&driver_cfg);

			PASS(); // pca9679_task_run() - Ready to progress IO

			// no break;
			
		case PCA9679_TASK_STATE_READ_IO :
			
			// no break;
			
		case PCA9679_TASK_STATE_WRITE_IO :
		
			// no break;
			
		case PCA9679_TASK_STATE_CANCEL :
		
			operation_stage = PCA9679_TASK_STATE_REQUEST_FINISH;
			p_com_driver->shut_down();
			p_com_driver->mutex_rel(com_driver_mutex_id);
			// no break;
		
		case PCA9679_TASK_STATE_REQUEST_FINISH :
		
			task_timer_start();
			operation_stage = PCA9679_TASK_STATE_IDLE;
			task_state == MCU_TASK_SLEEPING;
			
			break;
	}
}

/*!
 *
 */
void pca9679_task_background_run(void) {

}

/*!
 *
 */
void pca9679_task_sleep(void) {

}

/*!
 *
 */
void pca9679_task_wakeup(void) {

}

/*!
 *
 */
void pca9679_task_finish(void) {

}

/*!
 *
 */
void pca9679_task_terminate(void) {

}