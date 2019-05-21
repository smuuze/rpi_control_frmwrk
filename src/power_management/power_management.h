#ifndef _POWER_MANAGEMENT_H_
#define _POWER_MANAGEMENT_H_

// --------------------------------------------------------------------------------------------------------------

#define POWER_MGMN_BUILD_UNIT(name)										\
														\
	static POWER_MANAGEMENT_UNIT_TYPE _##name##_power_mgmnt_unit = 0;					\
														\
	u8 name##_is_on(void) {											\
		return power_mgmnt_is_on(&_##name##_power_mgmnt_unit);						\
	}													\
														\
	u8 name##_request(void) {										\
		return power_mgmnt_request(&_##name##_power_mgmnt_unit);					\
	}													\
														\
	void name##_release(void) {										\
		power_mgmnt_release(&_##name##_power_mgmnt_unit);						\
	}
	

#define POWER_MGMN_INCLUDE_UNIT(name)										\
														\
	u8 name##_request(void);										\
	void name##_release(void);


// --------------------------------------------------------------------------------------------------------------

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
typedef struct {

	u8 power_up_time_ms;
	u8 request_counter;
	u32 switch_on_timestamp;
	POWER_MANAGEMENT_STATUS_TYPE status;
	
} POWER_MANAGEMENT_UNIT_TYPE;

// --------------------------------------------------------------------------------------------------------------

/*!
 *
 * @return
 */
u8 power_mgmnt_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

/*!
 *
 */
void power_mgmnt_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

/*!
 *
 */
u8 power_mgmnt_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit);

#endif // _TIME_MANAGEMENT_H_
