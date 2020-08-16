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

#define TIME_CONFIGURATION_RUN_FOREVER				0xFFFFFFFF

//-----------------------------------------------------------------------------

typedef void (*TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK)	(void);

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	TIMER_FREQUENCY_NONE,
	TIMER_FREQUENCY_36kHz,
	TIMER_FREQUENCY_37_9kHz,
	TIMER_FREQUENCY_38kHz,
	TIMER_FREQUENCY_42kHz
} TIMER_CONFIGURAITON_FREQUENCY;

/*!
 *
 */
typedef enum {
	TIMER_TIME_INTERVAL_10us,
	TIMER_TIME_INTERVAL_40us,
	TIMER_TIME_INTERVAL_80us,
	TIMER_TIME_INTERVAL_560us,
	TIMER_TIME_INTERVAL_1ms,
	TIMER_TIME_INTERVAL_250ms,
	TIMER_TIME_INTERVAL_500ms,
	TIMER_TIME_INTERVAL_1sec
} TIMER_CONFIGURATION_TIME_INTERVAL;

/*!
 *
 */
typedef enum {
	TIMNER_DUTY_CACLE_25_PERCENT,
	TIMNER_DUTY_CACLE_50_PERCENT,
	TIMNER_DUTY_CACLE_75_PERCENT
} TIMER_CONFIGURATION_DUTY_CYCLE;

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
	TIMER_CONFIGURATION_TIME_INTERVAL time_interval;
	TIMER_CONFIGURATION_DUTY_CYCLE duty_cycle;
	TIMER_CONFIGURATION_MODE mode;
} TIMER_CONFIGURATION_TYPE;

//-----------------------------------------------------------------------------

#endif // _LOCAL_RTC_DRIVER_H_
