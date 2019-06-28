#ifndef _POWER_MANAGEMENT_H_
#define _POWER_MANAGEMENT_H_

// --------------------------------------------------------------------------------------------------------------

#define POWER_MGMN_BUILD_UNIT(name, _POWER_UP_TIME_MS, _POWER_ON_CALLBACK, _POWER_OFF_CALLBACK)			\
														\
	static POWER_MANAGEMENT_UNIT_TYPE _##name##_power_mgmnt_unit;						\
														\
	void name##_init(void) {										\
		power_mgmnt_init(										\
			&_##name##_power_mgmnt_unit,								\
			_POWER_UP_TIME_MS,									\
			_POWER_ON_CALLBACK,									\
			_POWER_OFF_CALLBACK									\
		);												\
	}													\
														\
	u8 name##_is_on(void) {											\
		return power_mgmnt_is_on(&_##name##_power_mgmnt_unit);						\
	}													\
														\
	void name##_request(void) {										\
		power_mgmnt_request(&_##name##_power_mgmnt_unit);						\
	}													\
														\
	void name##_release(void) {										\
		power_mgmnt_release(&_##name##_power_mgmnt_unit);						\
	}
	

#define POWER_MGMN_INCLUDE_UNIT(name)										\
														\
	void name##_init(void);											\
	u8 name##_request(void);										\
	void name##_release(void);										\
	u8 name##_is_on(void);


// --------------------------------------------------------------------------------------------------------------

/*!
 *
 */
typedef void (*POWER_MANAGEMENT_POWER_ON_CALLBACK)		(void);

/*!
 *
 */
typedef void (*POWER_MANAGEMENT_POWER_OFF_CALLBACK)		(void);

/*!
 *
 */
typedef struct {

	u8 is_on : 1;
	u8 is_ramp_up : 1;
	
} POWER_MANAGEMENT_STATUS_TYPE;

/*!
 *
 */
typedef struct POWER_MANAGEMENT_UNIT {

	u16 power_up_time_ms;
	u8 request_counter;
	u16 switch_on_timestamp;
	
	POWER_MANAGEMENT_POWER_ON_CALLBACK on;
	POWER_MANAGEMENT_POWER_OFF_CALLBACK off;
	
	POWER_MANAGEMENT_STATUS_TYPE status;
	
	struct POWER_MANAGEMENT_UNIT* _next;
	
} POWER_MANAGEMENT_UNIT_TYPE;

// --------------------------------------------------------------------------------------------------------------

/*!
 *
 */
void power_mgmnt_init(POWER_MANAGEMENT_UNIT_TYPE* p_unit, u16 power_up_time, POWER_MANAGEMENT_POWER_ON_CALLBACK p_callback_on, POWER_MANAGEMENT_POWER_OFF_CALLBACK p_callback_off);

/*!
 *
 * @return
 */
void power_mgmnt_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

/*!
 *
 */
void power_mgmnt_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

/*!
 *
 */
u8 power_mgmnt_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit);


// ---- Task Interface Prototypes ------------------------------------------------------------


/*!
 *
 */
void power_management_task_init(void);

/*!
 *
 */
//MCU_TASK_INTERFACE_TASK_STATE power_management_task_get_state(void);

/*!
 *
 */
void power_management_task_run(void);

/*!
 *
 */
void power_management_task_background_run(void);

/*!
 *
 */
void power_management_task_sleep(void);

/*!
 *
 */
void power_management_task_wakeup(void);

/*!
 *
 */
void power_management_task_finish(void);

/*!
 *
 */
void power_management_task_terminate(void);

#endif // _TIME_MANAGEMENT_H_
