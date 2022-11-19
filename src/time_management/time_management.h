/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    time_management.h
 * @author  Sebastian Lesse
 * @date    2022 / 11 / 13
 * @brief   Interface to create tiemrs that can be used to measure time-intervals
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_time_management_
#define _H_time_management_

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

/**
 * @brief Container to realize a timer-object
 * with a 8-bit resolution
 */
typedef struct {

    /**
     * @brief Timer-Status
     * Bit 0: active:1 / inactive:0
     */
    u8 status;

    /**
     * @brief Start-timestamp of this timer
     */
    u8 time_reference;
} TIME_MANAGEMENT_U8_TYPE;

/**
 * @brief Container to realize a timer-object
 * with a 16-bit resolution
 */
typedef struct {

    /**
     * @brief Timer-Status
     * Bit 0: active:1 / inactive:0
     */
    u8 status;

    /**
     * @brief Start-timestamp of this timer
     */
    u16 time_reference;
} TIME_MANAGEMENT_U16_TYPE;

/**
 * @brief Container to realize a timer-object
 * with a 32-bit resolution
 */
typedef struct {

    /**
     * @brief Timer-Status
     * Bit 0: active:1 / inactive:0
     */
    u8 status;

    /**
     * @brief Start-timestamp of this timer
     */
    u32 time_reference;
} TIME_MANAGEMENT_U32_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Timer-Status active/inactive
 * 
 */
#define TIME_MGMNT_STATUS_ACTIVE                    (1 << 0)

// --------------------------------------------------------------------------------

/**
 * @brief Builds a static timer for local use only with a resulution of 8-bit
 */
#define TIME_MGMN_BUILD_STATIC_TIMER_U8(name)                                           \
                                                                                        \
    static TIME_MANAGEMENT_U8_TYPE _##name##_time_mgmnt_context;                        \
                                                                                        \
    __UNUSED__ static inline void name##_start(void) {                                  \
        time_mgmnt_start_u8(&_##name##_time_mgmnt_context);                             \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline void name##_stop(void) {                                   \
        time_mgmnt_stop_u8(&_##name##_time_mgmnt_context);                              \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_is_up(u8 time_interval) {                        \
        return time_mgmnt_istimeup_u8(&_##name##_time_mgmnt_context, time_interval);    \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_start_time(void) {                               \
        return time_mgmnt_start_time_u8(&_##name##_time_mgmnt_context);                 \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_is_active(void) {                                \
        return time_mgmnt_is_active_u8(&_##name##_time_mgmnt_context);                  \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_elapsed(void) {                                  \
        return time_mgmnt_elapsed_u8(&_##name##_time_mgmnt_context);                    \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Builds a static timer for local use only with a resulution of 16-bit
 */
#define TIME_MGMN_BUILD_STATIC_TIMER_U16(name)                                          \
                                                                                        \
    static TIME_MANAGEMENT_U16_TYPE _##name##_time_mgmnt_context;                       \
                                                                                        \
    __UNUSED__ static inline void name##_start(void) {                                  \
        time_mgmnt_start_u16(&_##name##_time_mgmnt_context);                            \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline void name##_stop(void) {                                   \
        time_mgmnt_stop_u16(&_##name##_time_mgmnt_context);                             \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_is_up(u16 time_interval) {                       \
        return time_mgmnt_istimeup_u16(&_##name##_time_mgmnt_context, time_interval);   \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u16 name##_start_time(void) {                              \
        return time_mgmnt_start_time_u16(&_##name##_time_mgmnt_context);                \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_is_active(void) {                                \
        return time_mgmnt_is_active_u16(&_##name##_time_mgmnt_context);                 \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u16 name##_elapsed(void) {                                 \
        return time_mgmnt_elapsed_u16(&_##name##_time_mgmnt_context);                   \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Builds a static timer for local use only with a resulution of 16-bit
 */
#define TIME_MGMN_BUILD_STATIC_TIMER_U32(name)                                          \
                                                                                        \
    static TIME_MANAGEMENT_U32_TYPE _##name##_time_mgmnt_context;                       \
                                                                                        \
    __UNUSED__ static inline void name##_start(void) {                                  \
        time_mgmnt_start_u32(&_##name##_time_mgmnt_context);                            \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline void name##_stop(void) {                                   \
        time_mgmnt_stop_u32(&_##name##_time_mgmnt_context);                             \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_is_up(u32 time_interval) {                       \
        return time_mgmnt_istimeup_u32(&_##name##_time_mgmnt_context, time_interval);   \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u32 name##_start_time(void) {                              \
        return time_mgmnt_start_time_u32(&_##name##_time_mgmnt_context);                \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u8 name##_is_active(void) {                                \
        return time_mgmnt_is_active_u32(&_##name##_time_mgmnt_context);                 \
    }                                                                                   \
                                                                                        \
    __UNUSED__ static inline u32 name##_elapsed(void) {                                 \
        return time_mgmnt_elapsed_u32(&_##name##_time_mgmnt_context);                   \
    }

// --------------------------------------------------------------------------------

#define TIME_MGMNT_MAX_TIME_U8                  0xFF
#define TIME_MGMNT_MAX_TIME_U16                 0xFFFF
#define TIME_MGMNT_MAX_TIME_U32                 0xFFFFFFFF

// --------------------------------------------------------------------------------

/**
 * @brief get the actual system time in milliseconds with 
 * a resolution of 8 bit.
 * 
 * @return system time in milliseconds with 8 bit resolution.
 */
u8 time_mgmnt_gettime_u8(void);

/**
 * @brief get the actual system time in milliseconds with 
 * a resolution of 16 bit.
 * 
 * @return system time in milliseconds with 16 bit resolution.
 */
u16 time_mgmnt_gettime_u16(void);

/**
 * @brief get the actual system time in milliseconds with 
 * a resolution of 32 bit.
 * 
 * @return system time in milliseconds with 32 bit resolution.
 */
u32 time_mgmnt_gettime_u32(void);

// --------------------------------------------------------------------------------

/**
 * @brief Starts the timer of the given context.
 * IF the timer is already started it is restarted.
 * This function is used to start a 8-bit timer-object
 * 
 * @param p_context reference to the 8-bit timer-object to start
 */
void time_mgmnt_start_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief Starts the timer of the given context.
 * IF the timer is already started it is restarted.
 * This function is used to start a 16-bit timer-object
 * 
 * @param p_context reference to the 16-bit timer-object to start
 */
void time_mgmnt_start_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief Starts the timer of the given context.
 * IF the timer is already started it is restarted.
 * This function is used to start a 32-bit timer-object
 * 
 * @param p_context reference to the 32-bit timer-object to start
 */
void time_mgmnt_start_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief Stops the given timer-object.
 * If the timer is already stopped, nothing happens.
 * 
 * @param p_context reference to the 8-bit timer-objeect
 */
void time_mgmnt_stop_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief Stops the given timer-object.
 * If the timer is already stopped, nothing happens.
 * 
 * @param p_context reference to the 16-bit timer-objeect
 */
void time_mgmnt_stop_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief Stops the given timer-object.
 * If the timer is already stopped, nothing happens.
 * 
 * @param p_context reference to the 32-bit timer-objeect
 */
void time_mgmnt_stop_u32(TIME_MANAGEMENT_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief Checks if the timer-object is active or note.
 * A timer-object is active if it was started and is still running.
 * 
 * @param p_context reference to the 8-bit timer object
 * @return 1: the timer is active right now, 0 otherwise.
 */
u8 time_mgmnt_is_active_u8(TIME_MANAGEMENT_U8_TYPE* p_context);

/**
 * @brief Checks if the timer-object is active or note.
 * A timer-object is active if it was started and is still running.
 * 
 * @param p_context reference to the 16-bit timer object
 * @return 1: the timer is active right now, 0 otherwise.
 */
u8 time_mgmnt_is_active_u16(TIME_MANAGEMENT_U16_TYPE* p_context);

/**
 * @brief Checks if the timer-object is active or note.
 * A timer-object is active if it was started and is still running.
 * 
 * @param p_context reference to the 16-bit timer object
 * @return 1: the timer is active right now, 0 otherwise.
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

// --------------------------------------------------------------------------------

#endif // _H_time_management_

// --------------------------------------------------------------------------------
