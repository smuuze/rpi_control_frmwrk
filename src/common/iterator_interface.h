/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	iterator_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _INTERATOR_INTERFACE_H_
#define _INTERATOR_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct ITERATOR_INTERFACE {
	void* __element;
	u8 is_first;
	u8 is_last;
	u8 is_valid;
} ITERATOR_INTERFACE;

// --------------------------------------------------------------------------------

#endif // _INTERATOR_INTERFACE_H_
