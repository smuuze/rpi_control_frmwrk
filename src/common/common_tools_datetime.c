/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common/common_tools_datetime.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// --------------------------------------------------------------------------------

void common_tools_datetime_now(const char* format, char* buffer_to, u8 max_length) {
	DEBUG_PASS("common_tools_datetime_now()");

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	strftime(buffer_to, max_length, format, tm);
}

// --------------------------------------------------------------------------------
