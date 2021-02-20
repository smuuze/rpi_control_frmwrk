/*! 
 * --------------------------------------------------------------------------------
 *
 * 	@file		time_management.h
 * 	@author		sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _TIME_MANAGEMENT_H_
#define _TIME_MANAGEMENT_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct {
	u8 status;
	u8 time_reference;
} TIME_MANAGEMENT_U8_TYPE;

/**
 * @brief 
 * 
 */
typedef struct {
	u8 status;
	u16 time_reference;
} TIME_MANAGEMENT_U16_TYPE;

/**
 * @brief 
 * 
 */
typedef struct {
	u8 status;
	u32 time_reference;
} TIME_MANAGEMENT_U32_TYPE;

// --------------------------------------------------------------------------------

#define TIME_MGMNT_STATUS_ACTIVE					(1 << 0)

// --------------------------------------------------------------------------------

#define TIME_MGMN_BUILD_STATIC_TIMER_U8(name)									\
														\
	static TIME_MANAGEMENT_U8_TYPE _##name##_time_mgmnt_context;						\
														\
	__UNUSED__ static inline void name##_start(void) {							\
		time_mgmnt_start_u8(&_##name##_time_mgmnt_context);						\
	}													\
														\
	__UNUSED__ static inline void name##_stop(void) {							\
		time_mgmnt_stop_u8(&_##name##_time_mgmnt_context);						\
	}													\
														\
	__UNUSED__ static inline u8 name##_is_up(u8 time_interval) {						\
		return time_mgmnt_istimeup_u8(&_##name##_time_mgmnt_context, time_interval);			\
	}													\
														\
	__UNUSED__ static inline u8 name##_start_time(void) {							\
		return time_mgmnt_start_time_u8(&_##name##_time_mgmnt_context);					\
	}													\
														\
	__UNUSED__ static inline u8 name##_is_active(void) {							\
		return time_mgmnt_is_active_u8(&_##name##_time_mgmnt_context);					\
	}													\
														\
	__UNUSED__ static inline u8 name##_elapsed(void) {							\
		return time_mgmnt_elapsed_u8(&_##name##_time_mgmnt_context);					\
	}

#define TIME_MGMN_BUILD_STATIC_TIMER_U16(name)									\
														\
	static TIME_MANAGEMENT_U16_TYPE _##name##_time_mgmnt_context;						\
														\
	__UNUSED__ static inline void name##_start(void) {							\
		time_mgmnt_start_u16(&_##name##_time_mgmnt_context);						\
	}													\
														\
	__UNUSED__ static inline void name##_stop(void) {							\
		time_mgmnt_stop_u16(&_##name##_time_mgmnt_context);						\
	}													\
														\
	__UNUSED__ static inline u8 name##_is_up(u16 time_interval) {						\
		return time_mgmnt_istimeup_u16(&_##name##_time_mgmnt_context, time_interval);			\
	}													\
														\
	__UNUSED__ static inline u16 name##_start_time(void) {							\
		return time_mgmnt_start_time_u16(&_##name##_time_mgmnt_context);				\
	}													\
														\
	__UNUSED__ static inline u8 name##_is_active(void) {							\
		return time_mgmnt_is_active_u16(&_##name##_time_mgmnt_context);					\
	}													\
														\
	__UNUSED__ static inline u16 name##_elapsed(void) {							\
		return time_mgmnt_elapsed_u16(&_##name##_time_mgmnt_context);					\
	}

#define TIME_MGMN_BUILD_STATIC_TIMER_U32(name)									\
														\
	static TIME_MANAGEMENT_U32_TYPE _##name##_time_mgmnt_context;						\
														\
	__UNUSED__ static inline void name##_start(void) {							\
		time_mgmnt_start_u32(&_##name##_time_mgmnt_context);						\
	}													\
														\
	__UNUSED__ static inline void name##_stop(void) {							\
		time_mgmnt_stop_u32(&_##name##_time_mgmnt_context);						\
	}													\
														\
	__UNUSED__ static inline u8 name##_is_up(u32 time_interval) {						\
		return time_mgmnt_istimeup_u32(&_##name##_time_mgmnt_context, time_interval);			\
	}													\
														\
	__UNUSED__ static inline u32 name##_start_time(void) {							\
		return time_mgmnt_start_time_u32(&_##name##_time_mgmnt_context);				\
	}													\
														\
	__UNUSED__ static inline u8 name##_is_active(void) {							\
		return time_mgmnt_is_active_u32(&_##name##_time_mgmnt_context);					\
	}													\
														\
	__UNUSED__ static inline u32 name##_elapsed(void) {							\
		return time_mgmnt_elapsed_u32(&_##name##_time_mgmnt_context);					\
	}


// --------------------------------------------------------------------------------

#define TIME_MGMNT_MAX_TIME_U8							0xFF
#define TIME_MGMNT_MAX_TIME_U16							0xFFFF
#define TIME_MGMNT_MAX_TIME_U32							0xFFFFFFFF

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @return u8 
 */
u8 time_mgmnt_gettime_u8(void);

/**
 * @brief 
 * 
 * @return u16 
 */
u16 time_mgmnt_gettime_u16(void);

/**
 * @brief 
 * 
 * @return u32 
 */
u32 time_mgmnt_gettime_u32(void);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_context 
 */
void time_mgmnt_start_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 */
void time_mgmnt_start_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 */
void time_mgmnt_start_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_context 
 */
void time_mgmnt_stop_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 */
void time_mgmnt_stop_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 */
void time_mgmnt_stop_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_context 
 * @return u8 
 */
u8 time_mgmnt_is_active_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 * @return u8 
 */
u8 time_mgmnt_is_active_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 * @return u8 
 */
u8 time_mgmnt_is_active_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_context 
 * @param time_interval 
 * @return u8 
 */
u8 time_mgmnt_istimeup_u8(TIME_MANAGEMENT_U8_TYPE* p_context, u8 time_interval);

/**
 * @brief 
 * 
 * @param p_context 
 * @param time_interval 
 * @return u8 
 */
u8 time_mgmnt_istimeup_u16(TIME_MANAGEMENT_U16_TYPE* p_context, u16 time_interval);

/**
 * @brief 
 * 
 * @param p_context 
 * @param time_interval 
 * @return u8 
 */
u8 time_mgmnt_istimeup_u32(TIME_MANAGEMENT_U32_TYPE* p_context, u32 time_interval);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param time_reference 
 * @param time_interval 
 * @return u8 
 */
u8 time_mgmnt_istimeup_raw_u8(u8 time_reference, u8 time_interval);

/**
 * @brief 
 * 
 * @param time_reference 
 * @param time_interval 
 * @return u8 
 */
u8 time_mgmnt_istimeup_raw_u16(u16 time_reference, u16 time_interval);

/**
 * @brief 
 * 
 * @param time_reference 
 * @param time_interval 
 * @return u8 
 */
u8 time_mgmnt_istimeup_raw_u32(u32 time_reference, u32 time_interval);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_context 
 * @return u8 
 */
u8 time_mgmnt_elapsed_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 * @return u16 
 */
u16 time_mgmnt_elapsed_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 * @return u32 
 */
u32 time_mgmnt_elapsed_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_context 
 * @return u8 
 */
u8 time_mgmnt_start_time_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 * @return u16 
 */
u16 time_mgmnt_start_time_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief 
 * 
 * @param p_context 
 * @return u32 
 */
u32 time_mgmnt_start_time_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

#endif // _TIME_MANAGEMENT_H_
