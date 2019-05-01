/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "driver_PC9679.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

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

//-----------------------------------------------------------------------------

static PCA9679_INSTANCE_TYPE* p_first_instance = 0;
static PCA9679_INSTANCE_TYPE* p_last_instance = 0;

//-----------------------------------------------------------------------------

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

}

//-----------------------------------------------------------------------------

/*!
 *
 */
void pca9679_task_init(void) {

}

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE pca9679_task_get_state(void) {
	return MCU_TASK_SLEEPING;
}

/*!
 *
 */
void pca9679_task_run(void) {

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