/**
 * @file 	unittest_json_parser.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-02-06
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "protocol_management/json/protocol_json_parser.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE			1
#define TEST_CASE_ID_ADD_SINGLE_VALUE		2
#define TEST_CASE_ID_ADD_NEGATIVE_VALUE		3
#define TEST_CASE_ID_ADD_GROUP			4
#define TEST_CASE_ID_ADD_VALUE_INTO_GROUP	5
#define TEST_CASE_ID_ADD_STRING_INTO_GROUP	6
#define TEST_CASE_ID_CLOSE_GROUP		7
#define TEST_CAS_ID_CMD_RESPONSE_ROUTING	8

// --------------------------------------------------------------------------------

#define JSON_TEMP_STRING_LENGTH			1024

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots callbacks

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

JSON_PARSER_CREATE_OBJECT(UT_JASON_OBJECT)

// --------------------------------------------------------------------------------

static void TEST_CASE_initialize(void) {

	UT_START_TEST_CASE("initilize Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		UT_JASON_OBJECT_initialize();

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 1);
		UT_COMPARE_STRING(temp_string, "{");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_single_value(void) {

	UT_START_TEST_CASE("Add single value to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ADD_SINGLE_VALUE);

		UT_JASON_OBJECT_add_integer("SINGLE_INTEGER", 1024);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 22);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_negative_value(void) {

	UT_START_TEST_CASE("Add negative value to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ADD_NEGATIVE_VALUE);

		UT_JASON_OBJECT_add_integer("NEGATIVE_INTEGER", -2048);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 47);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_group(void) {

	UT_START_TEST_CASE("Add group to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ADD_GROUP);

		UT_JASON_OBJECT_start_group("GROUP_NAME");

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 62);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_value_into_group(void) {

	UT_START_TEST_CASE("Add value into group to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ADD_VALUE_INTO_GROUP);

		UT_JASON_OBJECT_add_integer("GROUP_VALUE", 123456789);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 85);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{\"GROUP_VALUE\":123456789");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_string(void) {

	UT_START_TEST_CASE("Add string into group to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ADD_STRING_INTO_GROUP);

		UT_JASON_OBJECT_add_string("STRING_VALUE", "this is a string");

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 119);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{\"GROUP_VALUE\":123456789,\"STRING_VALUE\":\"this is a string\"");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_close_group(void) {

	UT_START_TEST_CASE("closing group to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		UT_JASON_OBJECT_end_group();

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 120);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{\"GROUP_VALUE\":123456789,\"STRING_VALUE\":\"this is a string\"}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_multiple_groups_without_closing(void) {

	UT_START_TEST_CASE("Add multiple groups without closing to Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ADD_GROUP);

		UT_JASON_OBJECT_start_group("GROUP_2");
		UT_JASON_OBJECT_start_group("GROUP_3");
		UT_JASON_OBJECT_start_group("GROUP_4");
		UT_JASON_OBJECT_start_group("GROUP_5");

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 165);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{\"GROUP_VALUE\":123456789,\"STRING_VALUE\":\"this is a string\"},\"GROUP_2\":{\"GROUP_3\":{\"GROUP_4\":{\"GROUP_5\":{");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_finish(void) {

	UT_START_TEST_CASE("Finish Json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		UT_JASON_OBJECT_finish();

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 170);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{\"GROUP_VALUE\":123456789,\"STRING_VALUE\":\"this is a string\"},\"GROUP_2\":{\"GROUP_3\":{\"GROUP_4\":{\"GROUP_5\":{}}}}}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_value_to_finished_object(void) {

	UT_START_TEST_CASE("add value to finished object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		UT_JASON_OBJECT_start_group("GROUP_XX");
		UT_JASON_OBJECT_add_integer("GROUP_VALUE", 123456789);
		UT_JASON_OBJECT_end_group();
		UT_JASON_OBJECT_finish();

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 170);
		UT_COMPARE_STRING(temp_string, "{\"SINGLE_INTEGER\":1024,\"NEGATIVE_INTEGER\":-2048,\"GROUP_NAME\":{\"GROUP_VALUE\":123456789,\"STRING_VALUE\":\"this is a string\"},\"GROUP_2\":{\"GROUP_3\":{\"GROUP_4\":{\"GROUP_5\":{}}}}}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_start_report(void) {

	UT_START_TEST_CASE("Start Report")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		UT_JASON_OBJECT_start_group("REPORT");

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 11);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_version_response(void) {

	UT_START_TEST_CASE("add version to json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		COMMON_GENERIC_BUFFER_TYPE response_buffer = {
			.length = 4,
			.data = {0x01, 0x00, 0x05, 0x00}
		};

		UT_JASON_OBJECT_add_rpi_response(&response_buffer);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 26);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\"");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_temperature_response(void) {

	UT_START_TEST_CASE("add temperature to json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		COMMON_GENERIC_BUFFER_TYPE response_buffer = {
			.length = 6,
			.data = {0x07, 0x00, 0xFC, 0x17, 0xF9, 0x00}
		};

		UT_JASON_OBJECT_add_rpi_response(&response_buffer);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 89);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\",\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":-4,\"MAX\":23,\"MIN\":-7,\"MEAN\":0}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_humidity_response(void) {

	UT_START_TEST_CASE("add humidity to json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		COMMON_GENERIC_BUFFER_TYPE response_buffer = {
			.length = 6,
			.data = {0x08, 0x00, 0x02D, 0x2D, 0x29, 0x2B}
		};

		UT_JASON_OBJECT_add_rpi_response(&response_buffer);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 150);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\",\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":-4,\"MAX\":23,\"MIN\":-7,\"MEAN\":0},\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":45,\"MAX\":45,\"MIN\":41,\"MEAN\":43}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_ambilight_response(void) {

	UT_START_TEST_CASE("add ambilight to json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		COMMON_GENERIC_BUFFER_TYPE response_buffer = {
			.length = 6,
			.data = {0x0A, 0x00, 0x002, 0x2B, 0x00, 0x08}
		};

		UT_JASON_OBJECT_add_rpi_response(&response_buffer);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 209);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\",\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":-4,\"MAX\":23,\"MIN\":-7,\"MEAN\":0},\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":45,\"MAX\":45,\"MIN\":41,\"MEAN\":43},\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":2,\"MAX\":43,\"MIN\":0,\"MEAN\":8}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_output_state_response(void) {

	UT_START_TEST_CASE("add output_state to json object")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		COMMON_GENERIC_BUFFER_TYPE response_buffer_1 = {
			.length = 16,
			.data = {0x05, 0x00, 0x01, 0x01, 0xBB, 0x32, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00}
		};

		COMMON_GENERIC_BUFFER_TYPE response_buffer_2 = {
			.length = 16,
			.data = {0x05, 0x00, 0x02, 0x01, 0xBB, 0x32, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00}
		};

		COMMON_GENERIC_BUFFER_TYPE response_buffer_3 = {
			.length = 16,
			.data = {0x05, 0x00, 0x03, 0x01, 0xBB, 0x32, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00}
		};

		COMMON_GENERIC_BUFFER_TYPE response_buffer_4 = {
			.length = 16,
			.data = {0x05, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00}
		};

		UT_JASON_OBJECT_add_rpi_response(&response_buffer_1);
		UT_JASON_OBJECT_add_rpi_response(&response_buffer_2);
		UT_JASON_OBJECT_add_rpi_response(&response_buffer_3);
		UT_JASON_OBJECT_add_rpi_response(&response_buffer_4);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 596);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\",\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":-4,\"MAX\":23,\"MIN\":-7,\"MEAN\":0},\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":45,\"MAX\":45,\"MIN\":41,\"MEAN\":43},\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":2,\"MAX\":43,\"MIN\":0,\"MEAN\":8},\"OUTPUT_STATE_1\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_2\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_3\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_4\":{\"ERR\":\"OK\",\"PIN_STATE\":\"OFF\",\"ON_TIME_MS\":0,\"DURATION_MS\":0,\"PERIOD_MS\":0}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_add_hostname_response(void) {

	UT_START_TEST_CASE("add_hostname_response")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		char cmd_response[] = "SYS_HOSTNAME=unittest_hostname";

		UT_JASON_OBJECT_add_cli_response(cmd_response);

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 631);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\",\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":-4,\"MAX\":23,\"MIN\":-7,\"MEAN\":0},\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":45,\"MAX\":45,\"MIN\":41,\"MEAN\":43},\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":2,\"MAX\":43,\"MIN\":0,\"MEAN\":8},\"OUTPUT_STATE_1\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_2\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_3\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_4\":{\"ERR\":\"OK\",\"PIN_STATE\":\"OFF\",\"ON_TIME_MS\":0,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"SYS_HOSTNAME\":\"unittest_hostname\"");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_finish_response(void) {

	UT_START_TEST_CASE("finish response")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		UT_JASON_OBJECT_finish();

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 633);
		UT_COMPARE_STRING(temp_string, "{\"REPORT\":{\"VERSION\":\"5.0\",\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":-4,\"MAX\":23,\"MIN\":-7,\"MEAN\":0},\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":45,\"MAX\":45,\"MIN\":41,\"MEAN\":43},\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":2,\"MAX\":43,\"MIN\":0,\"MEAN\":8},\"OUTPUT_STATE_1\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_2\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_3\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":2831035,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"OUTPUT_STATE_4\":{\"ERR\":\"OK\",\"PIN_STATE\":\"OFF\",\"ON_TIME_MS\":0,\"DURATION_MS\":0,\"PERIOD_MS\":0},\"SYS_HOSTNAME\":\"unittest_hostname\"}}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_active_and_complete(void) {

	UT_START_TEST_CASE("Active and Complete")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CLOSE_GROUP);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_active(), 0);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_complete(), 1);

		UT_JASON_OBJECT_initialize();

		char temp_string[JSON_TEMP_STRING_LENGTH];
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_active(), 1);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_complete(), 0);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 1);
		UT_COMPARE_STRING(temp_string, "{");

		UT_JASON_OBJECT_initialize();
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_active(), 1);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_complete(), 0);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 1);
		UT_COMPARE_STRING(temp_string, "{");

		UT_JASON_OBJECT_finish();
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_active(), 0);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_complete(), 1);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 2);
		UT_COMPARE_STRING(temp_string, "{}");
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_get_cmd_response_routing(void) {

	UT_START_TEST_CASE("Get CMD-Response - Rotuing")
	{	
		UT_SET_TEST_CASE_ID(TEST_CAS_ID_CMD_RESPONSE_ROUTING);

		COMMON_GENERIC_BUFFER_TYPE response_buffer = {
			.length = 4,
			.data = {0x0C, 0x00, 0x0B, 0x00}
		};
		char temp_string[JSON_TEMP_STRING_LENGTH];

		UT_JASON_OBJECT_initialize();
		UT_JASON_OBJECT_start_group("RESPONSE");
		UT_JASON_OBJECT_add_rpi_response(&response_buffer);
		UT_JASON_OBJECT_finish();
		UT_JASON_OBJECT_copy_to(temp_string, JSON_TEMP_STRING_LENGTH);

		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_active(), 0);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_is_complete(), 1);
		UT_CHECK_IS_EQUAL(UT_JASON_OBJECT_get_length(), 62);
		UT_COMPARE_STRING(temp_string, "{\"RESPONSE\":{\"ROUTING\":{\"ERR\":\"OK\",\"IR_REMOTE\":{\"ERR\":\"OK\"}}}}");
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for json-parser 1.0")
	{
		TEST_CASE_initialize();
		TEST_CASE_add_single_value();
		TEST_CASE_add_negative_value();
		TEST_CASE_add_group();
		TEST_CASE_add_value_into_group();
		TEST_CASE_add_string();
		TEST_CASE_close_group();
		TEST_CASE_add_multiple_groups_without_closing();
		TEST_CASE_finish();
		TEST_CASE_add_value_to_finished_object();

		TEST_CASE_initialize();
		TEST_CASE_start_report();
		TEST_CASE_add_version_response();
		TEST_CASE_add_temperature_response();
		TEST_CASE_add_humidity_response();
		TEST_CASE_add_ambilight_response();
		TEST_CASE_add_output_state_response();
		TEST_CASE_add_hostname_response();
		TEST_CASE_finish_response();

		TEST_CASE_active_and_complete();

		TEST_CASE_get_cmd_response_routing();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
