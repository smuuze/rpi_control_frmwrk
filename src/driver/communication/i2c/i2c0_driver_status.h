/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	driver/communication/i2c/i2c0_driver_status.h
 * \author	sebastian lesse
 * \brief
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _I2C0_DRIVER_STATUS_H_
#define _I2C0_DRIVER_STATUS_H_

// --------------------------------------------------------------------------------

#include "local_module_status.h"

// --------------------------------------------------------------------------------

#define I2C_STATUS_DEFAULT_LENGTH				3

// these are byte-index

#define I2C_STATUS_RX_ACTIVE					1
#define I2C_STATUS_TX_ACTIVE					2
#define I2C_STATUS_IS_MASTER					3

// --------------------------------------------------------------------------------

INCLUDE_MODULE_STATUS_FAST_VOLATILE(I2C0_STATUS)

// --------------------------------------------------------------------------------

#endif // _I2C0_DRIVER_STATUS_H_