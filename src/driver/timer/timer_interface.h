/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	timer_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _TIMER_INTERFACE_H_
#define _TIMER_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_configuration.h"

// --------------------------------------------------------------------------------

typedef void (*TIMER_INTERFACE_CONFIGURE_CALLBACK)	(TIMER_CONFIGURATION_TYPE* p_configuration);
typedef void (*TIMER_INTERFACE_START_CALLBACK)		(u32 duration);
typedef void (*TIMER_INTERFACE_STOP_CALLBACK)		(void);

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct {

	TIMER_INTERFACE_CONFIGURE_CALLBACK configure;
	TIMER_INTERFACE_START_CALLBACK start;

} TIMER_INTERFACE_TYPE;


#endif // _TIMER_INTERFACE_H_
