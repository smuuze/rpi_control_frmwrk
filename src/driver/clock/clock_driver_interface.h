/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	src/driver/clock/clock_driver_interface.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _CLOCK_DRIVER_INTERFACE_H_
#define _CLOCK_DRIVER_INTERFACE_H_

// --------------------------------------------------------------------------------

#define CLK_DRIVER_CLK_PRESCALER_1		0x00
#define CLK_DRIVER_CLK_PRESCALER_2		0x01
#define CLK_DRIVER_CLK_PRESCALER_4		0x02
#define CLK_DRIVER_CLK_PRESCALER_8		0x03
#define CLK_DRIVER_CLK_PRESCALER_16		0x04
#define CLK_DRIVER_CLK_PRESCALER_32		0x05
#define CLK_DRIVER_CLK_PRESCALER_64		0x06
#define CLK_DRIVER_CLK_PRESCALER_128		0x07
#define CLK_DRIVER_CLK_PRESCALER_256		0x08

// --------------------------------------------------------------------------------

/*
 *
 */ 
void clock_driver_init(void);

// --------------------------------------------------------------------------------

#endif // _CLOCK_DRIVER_INTERFACE_H_
