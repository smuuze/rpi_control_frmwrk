#ifndef _PCA9679_H_
#define _PCA9679_H_


#include "mcu_task_interface.h"


// ---- Driver Macro Definitions -------------------------------------------------------------

#ifndef PCA9679_TASK_RUN_INTERVAL_MS
#define PCA9679_TASK_RUN_INTERVAL_MS		160 	/* every 5 * 32 = 160 ms */
#define PCA9679_TASK_RUN_INTERVAL_MS

#ifndef PCA9679_REQUEST_DRIVER_TIMEOUT_MS	100

//-----------------------------------------------------------------------------

#define PCA9679_DIRECTION_INPUT				1
#define PCA9679_DIRECTION_OUTPUT			0
#define PCA9679_DIRECTION_UNDEFINED			0xFF

#define PCA9679_STATE_HIGH				1
#define PCA9679_STATE_LOW				0
#define PCA9679_STATE_UNKNOWN				0xFF

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
		return pca9679_get_state(addr, pin_num);					\
	}											

#define PCA9679_INCLUDE_INPUT(name)								\
	void name##_init(void);									\
	u8 name##_get_state(void);								

#define PC9679_BUILD_OUTPUT(name, addr, pin_num)						\
												\
	void name##_init(void) {								\
		pca9679_set_direction(addr, pin_num, PCA9679_DIRECTION_INPUT);			\
		pca9679_set_level(addr, pin_num, PCA9679_LEVEL_HIGH);				\
	}											\
												\
	void name##_set_on(void) {								\
		return pca9679_set_state(addr, pin_num, PCA9679_STATE_HIGH);			\
	}											\
												\
	void name##_set_off(void) {								\
		return pca9679_set_state(addr, pin_num, PCA9679_STATE_LOW);			\
	}											\
												\
	u8 name##_get_state(void) {								\
		return pca9679_get_state(addr, pin_num);					\
	}	

#define PCA9679_INCLUDE_OUTPUT(name)								\
	void name##_init(void);									\
	u8 name##_set_on(void);									\
	u8 name##_set_off(void);								\
	u8 name##_get_state(void);								


// ---- Driver Type Definitions --------------------------------------------------------------

typedef struct PCA9679_INSTANCE {
	u8 address;
	u8 direction_mask;
	u8 level_mask;
	struct PCA9679_INSTANCE* next;
} PCA9679_INSTANCE_TYPE;


// ---- Driver Function Prototypes -----------------------------------------------------------


void local_sht31_module_init(TRX_DRIVER_INTERFACE* p_driver);


void pca_9679_register_module(PCA9679_INSTANCE_TYPE* p_instance);


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
