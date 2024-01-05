/*! 
 * --------------------------------------------------------------------------------
 *
 * \file    unittest_mcu_top.c
 * \brief
 * \author    sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "signal_slot_interface.h"
#include "mcu_task_interface.h"

#include "initialization.h"

#include "ui/file_interface/file_interface.h"
#include "time_management/time_management.h"
#include "common_tools_string.h"
#include "modules/cfg_file_parser/cfg_file_parser.h"

#include "lcd_interface.h"

#include "mcu_top.h"

// --------------------------------------------------------------------------------

#define UNITTEST_OUTPUT_PREFIX              "UT - "

// --------------------------------------------------------------------------------

#define UNITTEST_CFG_FILE_PATH                "mcu_top_output.txt"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INIT                    1

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static u8 counter_FILE_OPEN = 0;
static u8 counter_FILE_CLOSE = 0;
static u8 counter_FILE_IS_READABLE = 0;
static u8 counter_FILE_SET_PATH = 0;
static u8 counter_FILE_GET_PATH = 0;
static u8 counter_FILE_HAS_CHANGED = 0;
static u8 counter_FILE_IS_OPEN = 0;
static u8 counter_FILE_IS_EXISTING = 0;
static u8 counter_FILE_CREATE = 0;
static u8 counter_FILE_APPEND_LINE = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {
     counter_FILE_OPEN = 0;
     counter_FILE_CLOSE = 0;
     counter_FILE_IS_READABLE = 0;
     counter_FILE_SET_PATH = 0;
     counter_FILE_GET_PATH = 0;
     counter_FILE_HAS_CHANGED = 0;
     counter_FILE_IS_OPEN = 0;
     counter_FILE_IS_EXISTING = 0;
     counter_FILE_CREATE = 0;
     counter_FILE_APPEND_LINE = 0;
}

// --------------------------------------------------------------------------------

static u16 test_case_counter = 0xFFFF;

// stubs

void file_set_path(FILE_INTERFACE* p_file, const char* path) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_set_path()");

    memcpy(p_file->path, path, strlen(path));
}

const char* file_get_path(FILE_INTERFACE* p_file) {
    return (const char*)p_file->path;
}

u8 file_has_changed(FILE_INTERFACE* p_file) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_has_changed()");
    return 0;
}

u8 file_is_open(FILE_INTERFACE* p_file) {
    return 0;
}

u8 file_is_readable(FILE_INTERFACE* p_file) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_is_readable()");
    return 1;
}

u8 file_is_existing(FILE_INTERFACE* p_file) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_is_existing() - EXISTING");
    return 1;
}

u32 file_get_size(FILE_INTERFACE* p_file) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_get_size()");
    return 0;
}

u8 file_delete(FILE_INTERFACE* p_file) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_delete()");
    return 0;
}

u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_rename()");
    return 0;
}

u8 file_create(FILE_INTERFACE* p_file) {

    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_create()");
    return 0;
}

u8 file_open(FILE_INTERFACE* p_file) {

    if (file_is_existing(p_file) == 0) {
        return 0;
    }

    if (file_is_readable(p_file) == 0) {
        return 0;
    }

    DEBUG_TRACE_byte(test_case_counter, UNITTEST_OUTPUT_PREFIX "file_open() - test_case_counter");

    return 1;
}

void file_close(FILE_INTERFACE* p_file) {
    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_close()");
}

i16 file_read_line(FILE* file_handle, char* line_to, u16 num_max_bytes) {
    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_read_line()");
    return 0;
}

i16 file_read_next_line(FILE_INTERFACE* p_file, char* line_to, u16 max_line_length) {
    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_read_next_line()");

    static u8 line_counter = 0;
    line_counter += 1;
}

u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length) {
    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_read_specific_line()");
    return 0;
}

u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line) {
    DEBUG_TRACE_STR(new_line, UNITTEST_OUTPUT_PREFIX "file_append_line()");
    return 0;
}

u16 file_read_bytes(FILE_INTERFACE* p_file, u16 offset, u8* p_buffer, u16 max_length) {
    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "file_read_bytes()");
    return 0;
}

// --------------------------------------------------------------------------------

void console_write_line(const char* p_string) {
    DEBUG_TRACE_STR(p_string, UNITTEST_OUTPUT_PREFIX "console_write_line()");
    // printf("%s\n", p_string);
}

void console_write(const char* p_string) {
    printf("%s", p_string);
}

void console_write_number(i32 number) {
    printf("%d", number);
}

void console_write_string(const char* p_string, const char* p_argment_string) {
    printf("%s%s\n", p_string, p_argment_string);
}

void console_new_line(void) {
    printf("\n");
}

// --------------------------------------------------------------------------------

// slots

// --------------------------------------------------------------------------------

// Signals

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)

// --------------------------------------------------------------------------------

// stubs

void cfg_file_parser_init(void) {
    DEBUG_PASS(UNITTEST_OUTPUT_PREFIX "cfg_file_parser_init()");
    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
    CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
}

// --------------------------------------------------------------------------------

static void TEST_CASE_init(void) {

    UT_START_TEST_CASE("TEST_CASE_init")
    {
        initialization();
    }
    UT_END_TEST_CASE()
}

static void TEST_CASE_configure(void) {

    UT_START_TEST_CASE("TEST_CASE_configure")
    {
        CFG_FILE_PARSER_CFG_OBJECT_TYPE cfg_object;

        common_tools_string_clear(cfg_object.key, sizeof(cfg_object.key));
        common_tools_string_append(
            cfg_object.key,
            MCU_TOP_CFG_NAME_OUTPUT_CONSOLE,
            sizeof(cfg_object.key)
        );

        common_tools_string_clear(cfg_object.value, sizeof(cfg_object.value));
        common_tools_string_append(
            cfg_object.value,
            "1",
            sizeof(cfg_object.value)
        );

        CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(&cfg_object);

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(100) == 0) {
            mcu_task_controller_schedule();
        }

        common_tools_string_clear(cfg_object.key, sizeof(cfg_object.key));
        common_tools_string_append(
            cfg_object.key,
            MCU_TOP_CFG_NAME_OUTPUT_FILE,
            sizeof(cfg_object.key)
        );

        common_tools_string_clear(cfg_object.value, sizeof(cfg_object.value));
        common_tools_string_append(
            cfg_object.value,
            "mcu_top.txt",
            sizeof(cfg_object.value)
        );

        CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(&cfg_object);

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(100) == 0) {
            mcu_task_controller_schedule();
        }

        mcu_top_set_enabled(1);

        CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

        CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(6500) == 0) {
            mcu_task_controller_schedule();
        }
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_START_TESTBENCH("Welcome the the UNITTEST for mcu-top v1.0")
    {
        TEST_CASE_init();
        TEST_CASE_configure();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
