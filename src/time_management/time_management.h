#ifndef _TIME_MANAGEMENT_H_
#define _TIME_MANAGEMENT_H_


#define TIME_MGMN_BUILD_TIMER_U8(name)												\
																					\
	static u8 _##name##_time_reference = 0;											\
																					\
	inline void name##_start(void) {												\
		_##name##_time_refrence = time_mgmnt_gettime_u8(void);						\
	}																				\
																					\
	inline void name##_stop(void) {													\
		_##name##_time_refrence = 0;												\
	}																				\
																					\
	inline u8 name##_is_up(u8 time_interval) {										\
		return time_mgmnt_istimeup_u8(_##name##_time_reference, time_interval);		\
	}																				\
																					\
	inline u8 name##_elapsed(void) {												\
		return time_mgmnt_gettime_u8(void) - _##name##_time_refrence;				\
	}

#define TIME_MGMN_BUILD_TIMER_U16(name)												\
																					\
	static u16 _##name##_time_reference = 0;										\
																					\
	inline void name##_start(void) {												\
		_##name##_time_refrence = time_mgmnt_gettime_u16(void);						\
	}																				\
																					\
	inline void name##_stop(void) {													\
		_##name##_time_refrence = 0;												\
	}																				\
																					\
	inline u8 name##_is_up(u16 time_interval) {										\
		return time_mgmnt_istimeup_u16(_##name##_time_reference, time_interval);	\
	}																				\
																					\
	inline u16 name##_elapsed(void) {												\
		return time_mgmnt_gettime_u16(void) - _##name##_time_refrence;				\
	}

#define TIME_MGMN_BUILD_TIMER_U32(name)												\
																					\
	static u32 _##name##_time_reference = 0;										\
																					\
	inline void name##_start(void) {												\
		_##name##_time_refrence = time_mgmnt_gettime_u32(void);						\
	}																				\
																					\
	inline void name##_stop(void) {													\
		_##name##_time_refrence = 0;												\
	}																				\
																					\
	inline u8 name##_is_up(u32 time_interval) {										\
		return time_mgmnt_istimeup_u16(_##name##_time_reference, time_interval);	\
	}																				\
																					\
	inline u32 name##_elapsed(void) {												\
		return time_mgmnt_gettime_u16(void) - _##name##_time_refrence;				\
	}

/*!
 *
 * @return
 */
u8 time_mgmnt_gettime_u8(void);

/*!
 *
 * @return
 */
u16 time_mgmnt_gettime_u16(void);

/*!
 *
 * @return
 */
u32 time_mgmnt_gettime_u32(void);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 time_mgmnt_istimeup_u8(u8 time_reference, u8 time_interval);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 ltime_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval);
#endif // _TIME_MANAGEMENT_H_
