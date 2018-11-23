#ifndef _LOCAL_RTC_TIMERH_
#define _LOCAL_RTC_TIMERH_


/*! --- Function prototypes --- */
/*
 *
 */
void local_rtc_timer_init(void);

/*
 *
 */
void local_rtc_timer_power_off(void);

/*!
 *
 */
void local_rtc_timer_deinit(void);

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

#endif
