/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	timer1_driver.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _TIMER1_DRIVER_H_
#define _TIMER1_DRIVER_H_

//-----------------------------------------------------------------------------

#include "driver/timer/timer_configuration.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
void timer1_driver_init(void);

/*!
 *
 */
void timer1_driver_deinit(void);

/*!
 *
 */
void timer1_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration);

/*!
 *
 */
void timer1_driver_start(TIMER_CONFIGURATION_TIME time_ms);

/*!
 *
 */
void timer1_driver_stop(void);

//-----------------------------------------------------------------------------

#endif // _TIMER1_DRIVER_H_
