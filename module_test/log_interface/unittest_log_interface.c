/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_log_interface.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "ui/file_interface/file_interface.h"
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "ui/log_interface/log_interface.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_CONFIGURE					1
#define TEST_CASE_ID_LOG_MESSAGE				2
#define TEST_CASE_ID_WRITE_LOG_FILE				3
#define TEST_CASE_ID_LOG_FILE_SIZE_EXCEEDED			4
#define TEST_CASE_ID_LOG_QEUE_OVERFLOW				5

// --------------------------------------------------------------------------------

// counter

u8 counter_FILE_SET_PATH = 0;
u8 counter_FILE_HAS_CHANGED = 0;
u8 counter_FILE_IS_READABLE = 0;
u8 counter_FILE_IS_EXISTING = 0;
u8 counter_FILE_OPEN = 0;
u8 counter_FILE_CLOSE = 0;
u8 counter_FILE_READ_NEXT_LINE = 0;
u8 counter_FILE_GET_SIZE = 0;
u8 counter_FILE_DELETE = 0;
u8 counter_FILE_RENAME = 0;
u8 counter_FILE_CREATE = 0;
u8 counter_FILE_APPEND_LINE = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {
	counter_FILE_SET_PATH = 0;
	counter_FILE_HAS_CHANGED = 0;
	counter_FILE_IS_READABLE = 0;
	counter_FILE_IS_EXISTING = 0;
	counter_FILE_OPEN = 0;
	counter_FILE_CLOSE = 0;
	counter_FILE_READ_NEXT_LINE = 0;
	counter_FILE_GET_SIZE = 0;
	counter_FILE_DELETE = 0;
	counter_FILE_RENAME = 0;
	counter_FILE_CREATE = 0;
	counter_FILE_APPEND_LINE = 0;
}

// --------------------------------------------------------------------------------

// stubs

void file_set_path(FILE_INTERFACE* p_file, const char* path) {

	DEBUG_TRACE_STR(path, "file_set_path()");
	counter_FILE_SET_PATH += 1;

	memset(p_file->path, '\0', FILE_PATH_MAX_STRING_LENGTH);
	memcpy(p_file->path, path, strlen(path));
}

u8 file_has_changed(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_has_changed()");
	counter_FILE_HAS_CHANGED += 1;
	return 0;
}

u8 file_is_readable(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_is_readable()");
	counter_FILE_IS_READABLE += 1;
	return 1;
}

u8 file_is_existing(FILE_INTERFACE* p_file) {

	DEBUG_TRACE_STR(p_file->path, "file_is_existing()");
	counter_FILE_IS_EXISTING += 1;

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_FILE_SIZE_EXCEEDED) {
		if (counter_FILE_IS_EXISTING < 4) {
			return 1;
		}
		return 0;
	}

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_QEUE_OVERFLOW) {
		return 1;
	}

	return 0;
}

u8 file_is_open(FILE_INTERFACE* p_file) {
	return counter_FILE_OPEN != 0;
}

u32 file_get_size(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_get_size()");
	counter_FILE_GET_SIZE += 1;

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_FILE_SIZE_EXCEEDED) {
		return LOG_INTERFACE_MAX_LOG_FILE_SIZE_KB + 10;
	}

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_QEUE_OVERFLOW) {
		return 512;
	}

	return 0;
}

u8 file_delete(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_delete()");
	counter_FILE_DELETE += 1;
	return 0;
}

u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path) {

	DEBUG_PASS("file_rename()");
	counter_FILE_RENAME += 1;
	return 0;
}

const char* file_get_path(FILE_INTERFACE* p_file) {
	return (const char*)p_file->path;
}

u8 file_create(FILE_INTERFACE* p_file) {

	DEBUG_TRACE_STR(p_file->path, "file_create()");
	counter_FILE_CREATE += 1;
	return 1;
}

u8 file_open(FILE_INTERFACE* p_file) {

	counter_FILE_OPEN += 1;
	DEBUG_TRACE_STR(p_file->path, "file_open()");

	return 1;
}

void file_close(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_close()");
	counter_FILE_CLOSE += 1;
}

i16 file_read_line(FILE* file_handle, char* line_to, u16 num_max_bytes) {

	DEBUG_PASS("file_read_line()");
	return 0;
}

i16 file_read_next_line(FILE_INTERFACE* p_file, char* line_to, u16 max_line_length) {

	DEBUG_TRACE_STR(p_file->path, "file_read_next_line()");
	return -1;
}

u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length) {
	return 0;
}

u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line) {
	counter_FILE_APPEND_LINE += 1;
	DEBUG_TRACE_STR(new_line, "file_append_line()");
	return 0;
}

// --------------------------------------------------------------------------------

// slots

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void UNITTEST_log_interface_init(void) {
	
	UT_START_TEST_CASE("Log_interface - Initialize")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		log_interface_init();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UNITTEST_TIMER_stop();

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_configure(void) {
	
	UT_START_TEST_CASE("Log_interface - Configure")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONFIGURE);

		unittest_reset_counter();

		CFG_FILE_PARSER_CFG_OBJECT_TYPE log_file_path_cfg_obj	 	= { .key = "LOG_FILE_PATH"	, .value = "log/" 		};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE unknown_cfg_obj 		= { .key = "UNKNOWN"		, .value = "I am unknown"	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE invalid_cfg_obj 		= { .key = "INVALID"		, .value = "I am invalid"	};

		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&log_file_path_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&unknown_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&invalid_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(NULL);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(50) == 0) {
			mcu_task_controller_schedule();
		}

		CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(50) == 0) {
			mcu_task_controller_schedule();
		}

		CFG_FILE_PARSER_CFG_OBJECT_TYPE invalid_log_file_path_cfg_obj	= { .key = "LOG_FILE_PATH"	, .value = "invalid_log_path" 	};
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&invalid_log_file_path_cfg_obj);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(50) == 0) {
			mcu_task_controller_schedule();
		}

		UNITTEST_TIMER_stop();

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_log_message(void) {
	
	UT_START_TEST_CASE("Log_interface - Log-Message")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_LOG_MESSAGE);

		unittest_reset_counter();

		log_message("First log message of the unittest");
		log_message("Second log message of the unittest");
		log_message("Third log message of the unittest");
		log_message("Fourth log message of the unittest");
		log_message("Fivth log message of the unittest");

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_write_log_file(void) {
	
	UT_START_TEST_CASE("Log_interface - Write log file")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_WRITE_LOG_FILE);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UNITTEST_TIMER_stop();

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 5);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_log_file_size_too_big(void) {
	
	UT_START_TEST_CASE("Log_interface - Log-File size too big")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_LOG_FILE_SIZE_EXCEEDED);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		log_message("First log message of the unittest");

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UNITTEST_TIMER_stop();

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 4);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 4);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 3);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_que_overflow(void) {
	
	UT_START_TEST_CASE("Log_interface- Qeue overflow")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_LOG_QEUE_OVERFLOW);

		unittest_reset_counter();

		log_message("First log message of the unittest");
		log_message("Second log message of the unittest");
		log_message("Third log message of the unittest");
		log_message("Fourth log message of the unittest");
		log_message("Fivth log message of the unittest");
		log_message("Sixth log message of the unittest");
		log_message("Seventh log message of the unittest");

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 7);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	//TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for log-interface v1.0")
	{
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
		CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
	
		UNITTEST_log_interface_init();
		UNITTEST_log_interface_configure();
		UNITTEST_log_interface_log_message();
		UNITTEST_log_interface_write_log_file();
		UNITTEST_log_interface_log_file_size_too_big();
		UNITTEST_log_interface_que_overflow();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
