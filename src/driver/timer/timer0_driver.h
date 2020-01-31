/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	timer0_driver.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _LOCAL_RTC_DRIVER_H_
#define _LOCAL_RTC_DRIVER_H_

//-----------------------------------------------------------------------------

#include "driver/timer/timer_configuration.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
void timer0_driver_init(void);

/*!
 *
 */
void timer0_driver_deinit(void);

/*!
 *
 */
void timer0_driver_configure(TIMER_CONFIGURATION_TYPE p_config);

/*!
 *
 */
void timer0_driver_start(void);

/*!
 *
 */
void timer0_driver_stop(void);

//-----------------------------------------------------------------------------

#endif // _LOCAL_RTC_DRIVER_H_
