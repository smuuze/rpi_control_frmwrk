/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	timer_configuration.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _LOCAL_RTC_DRIVER_H_
#define _LOCAL_RTC_DRIVER_H_

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	TIMER_FREQUENCY_38kHz
} TIMER_CONFIGURAITON_FREQUENCY;


/*!
 *
 */
typedef struct {
	TIMER_CONFIGURAITON_FREQUENCY frequency;
} TIMER_CONFIGURATION_TYPE;

//-----------------------------------------------------------------------------

#endif // _LOCAL_RTC_DRIVER_H_
