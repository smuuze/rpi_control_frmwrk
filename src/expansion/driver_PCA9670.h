#ifndef _PCA9670_H_
#define _PCA9670_H_


#include "mcu_task_management/mcu_task_interface.h"


// ---- Driver Macro Definitions -------------------------------------------------------------

#ifndef PCA9670_TASK_RUN_INTERVAL_MS
#define PCA9670_TASK_RUN_INTERVAL_MS		160 	/* every 5 * 32 = 160 ms */
#endif

//-----------------------------------------------------------------------------

#define PCA9670_DIRECTION_INPUT				1
#define PCA9670_DIRECTION_OUTPUT			0
#define PCA9670_DIRECTION_UNDEFINED			0xFF

#define PCA9670_STATE_HIGH				1
#define PCA9670_STATE_LOW				0
#define PCA9670_STATE_UNKNOWN				0xFF

#define PCA9670_PIN_NUM_1				0
#define PCA9670_PIN_NUM_2				1
#define PCA9670_PIN_NUM_3				2
#define PCA9670_PIN_NUM_4				3
#define PCA9670_PIN_NUM_5				4
#define PCA9670_PIN_NUM_6				5
#define PCA9670_PIN_NUM_7				6
#define PCA9670_PIN_NUM_8				7

#define PCA9670_BUILD_INSTANCE(name, addr)							\
												\
	static PCA9670_INSTANCE_TYPE _##name##_pca9670_instance;				\
												\
	static void name##_init(void) {								\
		_##name##_pca9670_instance.address = addr;					\
		_##name##_pca9670_instance.direction_mask = 0;					\
		_##name##_pca9670_instance.level_mask = 0;					\
		_##name##_pca9670_instance.pin_values = 0;					\
		_##name##_pca9670_instance.next = 0;						\
		pca9670_register_module(&_##name##_pca9670_instance);				\
	}

#define PCA9670_BUILD_INPUT(name, addr, pin_num, pin_cfg)					\
												\
	void name##_init(void) {								\
		pca9670_set_direction(addr, pin_num, PCA9670_DIRECTION_INPUT);			\
		pca9670_set_state(addr, pin_num, PCA9670_STATE_HIGH);				\
	}											\
												\
	u8 name##_get_state(void) {								\
		return pca9670_get_state(addr, pin_num);					\
	}											

#define PCA9670_INCLUDE_INPUT(name)								\
	void name##_init(void);									\
	u8 name##_get_state(void);								

#define PC9670_BUILD_OUTPUT(name, addr, pin_num, pin_state_on, pin_state_off)			\
												\
	void name##_init(void) {								\
		pca9670_set_direction(addr, pin_num, PCA9670_DIRECTION_OUTPUT);			\
		pca9670_set_state(addr, pin_num, pin_state_off);				\
	}											\
												\
	void name##_set_on(void) {								\
		return pca9670_set_state(addr, pin_num, pin_state_on);				\
	}											\
												\
	void name##_set_off(void) {								\
		return pca9670_set_state(addr, pin_num, pin_state_off);				\
	}											\
												\
	u8 name##_get_state(void) {								\
		return pca9670_get_state(addr, pin_num);					\
	}	

#define PCA9670_INCLUDE_OUTPUT(name)								\
	void name##_init(void);									\
	u8 name##_set_on(void);									\
	u8 name##_set_off(void);								\
	u8 name##_get_state(void);								


// ---- Driver Type Definitions --------------------------------------------------------------

typedef struct PCA9670_INSTANCE {
	u8 address;
	u8 direction_mask;
	u8 level_mask;
	u8 pin_values;
	struct PCA9670_INSTANCE* next;
} PCA9670_INSTANCE_TYPE;


// ---- Driver Function Prototypes -----------------------------------------------------------


void pca9670_init(TRX_DRIVER_INTERFACE* p_driver);


void pca9670_register_module(PCA9670_INSTANCE_TYPE* p_instance);


void pca9670_set_direction(u8 instance_address, u8 instance_pin_number, u8 new_direction);


u8 pca9670_get_direction(u8 instance_address, u8 instance_pin_number);


void pca9670_set_state(u8 instance_address, u8 instance_pin_number, u8 new_state);


u8 pca9670_get_state(u8 instance_address, u8 instance_pin_number);


// ---- Task Interface Prototypes ------------------------------------------------------------


/*!
 *
 */
void pca9670_task_init(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE pca9670_task_get_state(void);

/*!
 *
 */
void pca9670_task_run(void);

/*!
 *
 */
void pca9670_task_background_run(void);

/*!
 *
 */
void pca9670_task_sleep(void);

/*!
 *
 */
void pca9670_task_wakeup(void);

/*!
 *
 */
void pca9670_task_finish(void);

/*!
 *
 */
void pca9670_task_terminate(void);


#endif /* _PCA9670_H_ */
