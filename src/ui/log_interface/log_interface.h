#ifndef LOG_INTERFACE_H_
#define LOG_INTERFACE_H_

/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ui/log_interface/log_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/*
 *
 */
void log_interface_init(void);

/*
 *
 */
void log_message(const char* new_message);

/*
 *
 */
void log_message_string(const char* message, const char* p_string);

// --------------------------------------------------------------------------------

#endif // LOG_INTERFACE_H_
