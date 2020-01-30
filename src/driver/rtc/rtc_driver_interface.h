#ifndef _LOCAL_RTC_DRIVER_H_
#define _LOCAL_RTC_DRIVER_H_


/*! --- Function prototypes --- */
/*
 *
 */
void rtc_driver_init(void);

/*
 *
 */
void local_rtc_timer_power_off(void);

/*!
 *
 */
void rtc_driver_deinit(void);

/*!
 *
 * @return
 */
u8 local_rtc_timer_gettime_u8(void);

/*!
 *
 * @return
 */
u16 local_rtc_timer_gettime_u16(void);

/*!
 *
 * @return
 */
u32 local_rtc_timer_gettime_u32(void);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval);

/*!
 *
 * @param time_reference
 * @param time_interval
 * @return
 */
u8 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval);

#endif // _LOCAL_RTC_DRIVER_H_
