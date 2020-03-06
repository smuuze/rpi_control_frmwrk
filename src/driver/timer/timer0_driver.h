/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	timer0_driver.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _TIMER0_DRIVER_H_
#define _TIMER0_DRIVER_H_

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
void timer0_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration);

/*!
 *
 */
void timer0_driver_start(u32 time_us);

/*!
 *
 */
void timer0_driver_stop(void);

//-----------------------------------------------------------------------------

#endif // _TIMER0_DRIVER_H_
