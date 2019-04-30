#ifndef _PCA9679_H_
#define _PCA9679_H_


#include "mcu_task_interface.h"


// ---- Driver Macro Definitions -------------------------------------------------------------

#define PCA9679_DIRECTION_INPUT				1
#define PCA9679_DIRECTION_OUTPUT			0

#define PCA9679_STATE_HIGH				1
#define PCA9679_STATE_LOW				0

#define PCA9679_BUILD_INSTANCE(name, addr)							\
												\
	static PCA9679_INSTANCE_TYPE _##name##_pca9679_instance = {				\
		.address = addr,								\
		.direction_mask = 0,								\
		.leval_mask = 0									\
	};											\
												\
	static void name##_init(void) {								\
		pca_9679_register_module(&_##name##_pca9679_instance);				\
	}

#define PCA9679_BUILD_INPUT(name, addr, pin_num)						\
												\
	void name##_init(void) {								\
		pca9679_set_direction(addr, pin_num, PCA9679_DIRECTION_INPUT);			\
		pca9679_set_level(addr, pin_num, PCA9679_LEVEL_HIGH);				\
	}											\
												\
	u8 name##_get_state(void) {								\
		return pca9679_get_level(addr, pin_num);					\
	}

// ---- Driver Type Definitions --------------------------------------------------------------


typedef struct {
	u8 address;
	u8 direction_mask;
	u8 level_mask;
} PCA9679_INSTANCE_TYPE;


typedef struct {
	u8 pin_number : 3;
	u8 direction : 1;
	u8 level : 1;
	u8 RESERVED : 3;
} PCA9679_PIN_CFG_TYPE


typedef struct {
	u8 address;
	PCA9679_PIN_CFG_TYPE pin_cfg;
} PCA9679_PIN_TYPE;


// ---- Driver Function Prototypes -----------------------------------------------------------


void pca_9679_register_module(PCA9679_INSTANCE_TYPE* p_instance);


void pca9679_init(PCA9679_INSTANCE_TYPE* p_instance);


void pca9679_set_direction(u8 instance_address, u8 instance_pin_number, u8 new_direction);


u8 pca9679_get_direction(u8 instance_address, u8 instance_pin_number);


void pca9679_set_state(u8 instance_address, u8 instance_pin_number, u8 new_state);


u8 pca9679_get_state(u8 instance_address, u8 instance_pin_number);


// ---- Task Interface Prototypes ------------------------------------------------------------


/*!
 *
 */
void pca9679_task_init(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE pca9679_task_get_state(void);

/*!
 *
 */
void pca9679_task_run(void);

/*!
 *
 */
void pca9679_task_background_run(void);

/*!
 *
 */
void pca9679_task_sleep(void);

/*!
 *
 */
void pca9679_task_wakeup(void);

/*!
 *
 */
void pca9679_task_finish(void);

/*!
 *
 */
void pca9679_task_terminate(void);


#endif /* _PCA9679_H_ */
