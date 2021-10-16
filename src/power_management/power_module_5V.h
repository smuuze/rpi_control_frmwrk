/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	power_module_5V.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _POWER_MODULE_5V_H_
#define _POWER_MODULE_5V_H_

//---------------------------------------------------------------------------------

#include "power_management/power_management_interface.h"

//---------------------------------------------------------------------------------

#ifdef HAS_POWER_MANAGEMENT_MODULE_5V
POWER_MGMN_INCLUDE_UNIT(POWER_UNIT_5V)
#endif

//---------------------------------------------------------------------------------

#endif // _POWER_MODULE_5V_H_
