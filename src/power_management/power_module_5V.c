/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	power_module_5V.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

//---------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//---------------------------------------------------------------------------------

#include "tracer.h"

//---------------------------------------------------------------------------------

#include "power_management/power_management_interface.h"

//---------------------------------------------------------------------------------

#ifndef POWER_UNIT_5V_POWER_UP_TIME_MS
#define POWER_UNIT_5V_POWER_UP_TIME_MS		20
#endif

#ifndef POWER_UNIT_5V_POWER_DOWN_TIME_MS
#define POWER_UNIT_5V_POWER_DOWN_TIME_MS	2000
#endif

#ifndef POWER_UNIT_5V_POWER_ON_CALLBACK
#define POWER_UNIT_5V_POWER_ON_CALLBACK		EXT_POWER_01_drive_high
#endif

#ifndef POWER_UNIT_5V_POWER_OFF_CALLBACK
#define POWER_UNIT_5V_POWER_OFF_CALLBACK	EXT_POWER_01_drive_low
#endif

//---------------------------------------------------------------------------------

/**
 * @brief dummy function prototype to avoid compiler warnings
 * 
 */
extern void POWER_UNIT_5V_POWER_ON_CALLBACK(void);


/**
 * @brief dummy function prototype to avoid compiler warnings
 * 
 */
extern void POWER_UNIT_5V_POWER_OFF_CALLBACK(void);

//---------------------------------------------------------------------------------

POWER_MGMN_BUILD_UNIT(POWER_UNIT_5V, POWER_UNIT_5V_POWER_UP_TIME_MS, POWER_UNIT_5V_POWER_DOWN_TIME_MS, POWER_UNIT_5V_POWER_ON_CALLBACK, POWER_UNIT_5V_POWER_OFF_CALLBACK)
