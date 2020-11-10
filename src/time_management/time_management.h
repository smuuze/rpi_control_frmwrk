#ifndef _TIME_MANAGEMENT_H_
#define _TIME_MANAGEMENT_H_


#define TIME_MGMN_BUILD_STATIC_TIMER_U8(name)									\
														\
	static u8 _##name##_time_reference = 0;									\
	static u8 _##name##_timer_status = 0;									\
														\
	static inline void name##_start(void) {									\
		_##name##_time_reference = time_mgmnt_gettime_u8();						\
		_##name##_timer_status = 1;									\
	}													\
														\
	static inline void name##_stop(void) {									\
		_##name##_timer_status = 0;									\
	}													\
														\
	static inline u8 name##_is_up(u8 time_interval) {							\
		return time_mgmnt_istimeup_u8(_##name##_time_reference, time_interval);				\
	}													\
														\
	static inline u8 name##_start_time(void) {								\
		return _##name##_time_reference;								\
	}													\
														\
	static inline u8 name##_is_active(void) {								\
		return _##name##_timer_status != 0 ? 1 : 0;							\
	}													\
														\
	static inline u8 name##_elapsed(void) {									\
		return time_mgmnt_elapsed_u8(_##name##_time_reference);						\
	}

#define TIME_MGMN_BUILD_STATIC_TIMER_U16(name)									\
														\
	static u16 _##name##_time_reference = 0;								\
	static u8 _##name##_timer_status = 0;									\
														\
	static inline void name##_start(void) {									\
		_##name##_time_reference = time_mgmnt_gettime_u16();						\
		_##name##_timer_status = 1;									\
	}													\
														\
	static inline void name##_stop(void) {									\
		_##name##_timer_status = 0;									\
	}													\
														\
	static inline u8 name##_is_up(u16 time_interval) {							\
		return time_mgmnt_istimeup_u16(_##name##_time_reference, time_interval);			\
	}													\
														\
	static inline u16 name##_start_time(void) {								\
		return _##name##_time_reference;								\
	}													\
														\
	static inline u8 name##_is_active(void) {								\
		return _##name##_timer_status != 0 ? 1 : 0;							\
	}													\
														\
	static inline u16 name##_elapsed(void) {								\
		return time_mgmnt_elapsed_u16(_##name##_time_reference);					\
	}

#define TIME_MGMN_BUILD_STATIC_TIMER_U32(name)									\
														\
	static u32 _##name##_time_reference = 0;								\
	static u8 _##name##_timer_status = 0;									\
														\
	static inline void name##_start(void) {									\
		_##name##_time_reference = time_mgmnt_gettime_u32();						\
		_##name##_timer_status = 1;									\
	}													\
														\
	static inline void name##_stop(void) {									\
		_##name##_timer_status = 0;									\
	}													\
														\
	static inline u8 name##_is_up(u32 time_interval) {							\
		return time_mgmnt_istimeup_u32(_##name##_time_reference, time_interval);			\
	}													\
														\
	static inline u32 name##_start_time(void) {								\
		return _##name##_time_reference;								\
	}													\
														\
	static inline u8 name##_is_active(void) {								\
		return _##name##_timer_status != 0 ? 1 : 0;							\
	}													\
														\
	static inline u32 name##_elapsed(void) {								\
		return time_mgmnt_elapsed_u32(_##name##_time_reference);					\
	}

#define TIME_MGMNT_MAX_TIME_U8							0xFF
#define TIME_MGMNT_MAX_TIME_U16							0xFFFF
#define TIME_MGMNT_MAX_TIME_U32							0xFFFFFFFF


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
u8 time_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval);

/*!
 *
 * @param time_reference
 * @return
 */
u8 time_mgmnt_elapsed_u8(u8 time_reference);

/*!
 *
 * @param time_reference
 * @return
 */
u16 time_mgmnt_elapsed_u16(u16 time_reference);

/*!
 *
 * @param time_reference
 * @return
 */
u32 time_mgmnt_elapsed_u32(u32 time_reference);

#endif // _TIME_MANAGEMENT_H_
