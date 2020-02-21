/*! 
 * ----------------------------------------------------------------------------
 *
 * \file	timer_configuration.h
 * \brief
 * \author	sebastian lesse
 *
 * ----------------------------------------------------------------------------
 */

#ifndef _LOCAL_RTC_DRIVER_H_
#define _LOCAL_RTC_DRIVER_H_

//-----------------------------------------------------------------------------

typedef void (*TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK)	(void);

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	TIMER_FREQUENCY_NONE
	TIMER_FREQUENCY_36kHz,
	TIMER_FREQUENCY_38kHz,
	TIMER_FREQUENCY_42kHz
} TIMER_CONFIGURAITON_FREQUENCY;

/*!
 *
 */
typedef enum {
	TIMER_TIME_NONE,
	TIMER_TIME_4ms,
	TIMER_TIME_1690us,
	TIMER_TIME_560us,
} TIMER_CONFIGURATION_TIME;

/*!
 *
 */
typedef enum {
	TIMER_MIN_TIME_560us,
	TIMER_MIN_TIME_1ms,
} TIMER_CONFIGURATION_MINIMUM_TIME;

/*!
 *
 */
typedef enum {
	TIMER_MODE_TIMER,
	TIMER_MODE_FREQUENCY
} TIMER_CONFIGURATION_MODE;

/*!
 *
 */
typedef struct {
	TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK irq_callback;
	TIMER_CONFIGURAITON_FREQUENCY frequency;
	TIMER_CONFIGURATION_MINIMUM_TIME min_time;
	TIMER_CONFIGURATION_MODE mode;
} TIMER_CONFIGURATION_TYPE;

//-----------------------------------------------------------------------------

#endif // _LOCAL_RTC_DRIVER_H_
