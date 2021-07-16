/**
 * --------------------------------------------------------------------------------
 *
 * @file	power_management_interface.h
 * @author	sebastian lesse
 * 
 * @brief	USAGE:
 * 
 * 		- in your makefile add:			POWER_MANAGEMENT_CFG = 
 *							POWER_MANAGEMENT_CFG += POWER_5V_MODULE
 *
 *		- add header to your init-file:		#include "power_management/power_management_initialization.h"
 *
 *		- in your init-routine add :		#ifdef HAS_POWER_MANAGEMENT_MODULE
 *							{
 *								power_management_initialization();
 *							}
 *							#endif
 *
 *		- in your task-file include header:	#include "power_management/power_module_5V.h"
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _POWER_MANAGEMENT_H_
#define _POWER_MANAGEMENT_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#ifndef POWER_MANAGEMENT_MAX_REQUEST_COUNT_VALUE
#define POWER_MANAGEMENT_MAX_REQUEST_COUNT_VALUE				255
#endif

// --------------------------------------------------------------------------------

#define POWER_MGMN_BUILD_UNIT(name, _POWER_UP_TIME_MS, _POWER_DOWN_TIME_MS, _POWER_ON_CALLBACK, _POWER_OFF_CALLBACK)	\
															\
	static POWER_MANAGEMENT_UNIT_TYPE _##name##_power_mgmnt_unit;							\
															\
	void name##_init(void) {											\
		power_mgmnt_unit_init(											\
			&_##name##_power_mgmnt_unit,									\
			_POWER_UP_TIME_MS,										\
			_POWER_DOWN_TIME_MS,										\
			_POWER_ON_CALLBACK,										\
			_POWER_OFF_CALLBACK										\
		);													\
	}														\
															\
	u8 name##_is_on(void) {												\
		return power_mgmnt_unit_is_on(&_##name##_power_mgmnt_unit);						\
	}														\
															\
	void name##_request(void) {											\
		power_mgmnt_unit_request(&_##name##_power_mgmnt_unit);							\
	}														\
															\
	void name##_release(void) {											\
		power_mgmnt_unit_release(&_##name##_power_mgmnt_unit);							\
	}
	

#define POWER_MGMN_INCLUDE_UNIT(name)											\
															\
	void name##_init(void);												\
	u8 name##_request(void);											\
	void name##_release(void);											\
	u8 name##_is_on(void);

// --------------------------------------------------------------------------------

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
	u8 is_ramp_down : 1;
	u8 is_initialized : 1;

} POWER_MANAGEMENT_STATUS_TYPE;

/**
 * @brief Structure to hold a single unit and its context
 * This structure is needed to use a new created unit on runtime.
 * 
 */
typedef struct POWER_MANAGEMENT_UNIT {

	u16 power_up_time_ms;
	u16 power_down_time_ms;

	u8 request_counter;
	u16 switch_timestamp;
	
	POWER_MANAGEMENT_POWER_ON_CALLBACK on;
	POWER_MANAGEMENT_POWER_OFF_CALLBACK off;
	
	POWER_MANAGEMENT_STATUS_TYPE status;
	
	struct POWER_MANAGEMENT_UNIT* _next;
	
} POWER_MANAGEMENT_UNIT_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the power management module.
 * This function must be called before initialization
 * any power-management-unit.
 * 
 */
void power_management_initialization(void);

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the power-management task.
 * This function mus be called before adding the first unit
 * 
 */
void power_mgmnt_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief Initializes a single unit. The power-off callback is executed after the unit is initializes
 * The unit is added to the list of available unit so it can be handled by the power-management task
 * 
 * @param p_unit unit to initialize
 * @param power_up_time ramp-up time, time span that has been passed from the moment the unit as requested until it is set into powered-up stats
 * @param power_down_time 
 * @param p_callback_on function pointer to power on the new unit
 * @param p_callback_off function pointer to the power-off the new unit
 */
void power_mgmnt_unit_init(POWER_MANAGEMENT_UNIT_TYPE* p_unit, u16 power_up_time, u16 power_down_time, POWER_MANAGEMENT_POWER_ON_CALLBACK p_callback_on, POWER_MANAGEMENT_POWER_OFF_CALLBACK p_callback_off);

/**
 * @brief Resquests the given unit. To check if the unit is ready, use the is_power_on function.
 * If the unit was requested by an other object before and the unit is already powered-up, the ramp-up time isn't used for a second time.
 * The unit then is immediately ready to be used, indicated by power_mgmnt_unit_is_on().
 * 
 * @param p_unit the unit that is requested
 */
void power_mgmnt_unit_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

/**
 * @brief Release a unit. If the unit is still requested by one or more other objects, it will not be powered-off.
 * After the last object has released this unit it enters the power-down state.
 * 
 * @param p_unit the unit that is released
 */
void power_mgmnt_unit_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

/**
 * @brief Check if a unit is ready to use. A unit is ready for use if it was requested by at least one object and the ramp-up time has passed.
 * 
 * @param p_unit The unit of interesst
 * @return u8 1: the unit is powered-on and ready to use, otherwise 0
 */
u8 power_mgmnt_unit_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

// --------------------------------------------------------------------------------

#endif // _TIME_MANAGEMENT_H_
