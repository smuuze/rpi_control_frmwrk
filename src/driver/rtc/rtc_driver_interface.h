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
 * @file    rtc_driver_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 11 / 13
 * @brief   Interface to get the actual system time in number of milliseconds.
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_rtc_driver_interface_
#define _H_rtc_driver_interface_

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the hw-peripheral
 * to use the rtc-timer
 * 
 */
void rtc_driver_init(void);

/**
 * @brief Deinitializes the hw-peripheral.
 * The rtc-timer cannot be used anymore
 * 
 */
void rtc_driver_deinit(void);

/**
 * @brief Shuts down the hw-peripheral
 * to save power. The rtc-timer cannot be used anymore
 * 
 */
void rtc_timer_power_off(void);

// --------------------------------------------------------------------------------

/**
 * @brief Actual timer-value as u8.
 * This value will overrun.
 * 
 * @return Actual timer-value in milliseconds as u8
 */
u8 rtc_timer_gettime_u8(void);

/**
 * @brief Actual timer-value as u16
 * This value will overrun.
 * 
 * @return Actual timer-value in milliseconds as u16 
 */
u16 rtc_timer_gettime_u16(void);

/**
 * @brief Actual timer-value as u32
 * This value will overrun.
 * 
 * @return Actual timer-value in milliseconds as u32 
 */
u32 rtc_timer_gettime_u32(void);

// --------------------------------------------------------------------------------

/**
 * @brief Checks if the given time-interval has passed.
 * This value can only be used for time-intervals less than 2^8
 * 
 * @param time_reference start-time of the actual interval
 * @param time_interval interval that will be passed
 * @return 1 if the interval has passed, otherwise 0
 */
u8 rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval);

/**
 * @brief Checks if the given time-interval has passed.
 * This value can only be used for time-intervals less than 2^16
 * 
 * @param time_reference start-time of the actual interval
 * @param time_interval interval that will be passed
 * @return 1 if the interval has passed, otherwise 0
 */
u8 rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval);

/**
 * @brief Checks if the given time-interval has passed.
 * This value can only be used for time-intervals less than 2^32
 * 
 * @param time_reference start-time of the actual interval
 * @param time_interval interval that will be passed
 * @return 1 if the interval has passed, otherwise 0
 */
u8 rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval);

// --------------------------------------------------------------------------------

/**
 * @brief Get the time taht has elapsed since the given time-reference.
 * The maximum elapsed time is always less than 2^8
 *
 * @param time_reference reference-time from which the elapsed time is calculated
 * @return the elapsed time since time_reference in milliseconds
 */
u8 rtc_timer_elapsed_u8(u8 time_reference);

/**
 * @brief Get the time taht has elapsed since the given time-reference.
 * The maximum elapsed time is always less than 2^16
 *
 * @param time_reference reference-time from which the elapsed time is calculated
 * @return the elapsed time since time_reference in milliseconds
 */
u16 rtc_timer_elapsed_u16(u16 time_reference);

/**
 * @brief Get the time taht has elapsed since the given time-reference.
 * The maximum elapsed time is always less than 2^32
 *
 * @param time_reference reference-time from which the elapsed time is calculated
 * @return the elapsed time since time_reference in milliseconds
 */
u32 rtc_timer_elapsed_u32(u32 time_reference);

// --------------------------------------------------------------------------------

/**
 * @brief Delays the program for the given amount of micor-seconds.
 * This function blocks until the delay time has passed.
 * Use with care and remind active watchdogs.
 * 
 * @param delay_us amount of micro-seconds to pass before this function returns.
 */
void rtc_timer_usleep(u32 delay_us) {

// --------------------------------------------------------------------------------

/**
 * @brief backwards compatibility
 * @see rtc_timer_power_off(void)
 */
#define local_rtc_timer_power_off rtc_timer_power_off

/**
 * @brief backwards compatibility
 * @see rtc_timer_gettime_u8(void)
 */
#define local_rtc_timer_gettime_u8 rtc_timer_gettime_u8

/**
 * @brief backwards compatibility
 * @see local_rtc_timer_gettime_u16(void)
 */
#define local_rtc_timer_gettime_u16 rtc_timer_gettime_u16

/**
 * @brief backwards compatibility
 * @see rtc_timer_gettime_u32(void)
 */
#define local_rtc_timer_gettime_u32 rtc_timer_gettime_u32

/**
 * @brief backwards compatibility
 * @see rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval)
 */
#define local_rtc_timer_istimeup_u8 rtc_timer_istimeup_u8

/**
 * @brief backwards compatibility
 * @see rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval)
 */
#define local_rtc_timer_istimeup_u16 rtc_timer_istimeup_u16

/**
 * @brief backwards compatibility
 * @see rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval)
 */
#define local_rtc_timer_istimeup_u32 rtc_timer_istimeup_u32

// --------------------------------------------------------------------------------

#endif // _H_rtc_driver_interface_

// --------------------------------------------------------------------------------
