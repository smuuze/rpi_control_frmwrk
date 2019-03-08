/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "local_context.h"
#include "io_controller.h"

//---------- Implementation of Traces -----------------------------------------

//#define TRACES
//#include <traces.h>

#define TRACER_OFF
#include "tracer.h"


#if config_HAS_LED_MATRIX == 1

static IO_OUTPUT_DESCRIPTOR led_01 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_01, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_02 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_02, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_03 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_03, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_04 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_04, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_05 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_05, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_06 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_06, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_07 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_07, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_08 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_08, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

static IO_OUTPUT_DESCRIPTOR led_09 = {
	0, //u8 id;
	IO_TYPE_USER,
	0, //u8 actual_pin_state;
	0, //u8 next_pin_state;
	0, //u32 reference_time;
	0, //u32 duration;
	0, //u32 toggle_period;
	&specific_set_led_09, //IO_OUTPUT_SET_PIN set_pin;
	0 //struct IO_OUTPUT_DESCRIPTOR* _next;
};

#endif

IO_CONTROLLER_BUILD_OUTPUT(extern_output_01, EXTERN_OUTPUT_01)
IO_CONTROLLER_BUILD_OUTPUT(extern_output_02, EXTERN_OUTPUT_02)
IO_CONTROLLER_BUILD_OUTPUT(extern_output_03, EXTERN_OUTPUT_03)
IO_CONTROLLER_BUILD_OUTPUT(extern_output_04, EXTERN_OUTPUT_04)

void output_initialization(void) {

	PASS(); // output_initialization()

	extern_output_01_init();
	extern_output_02_init();
	extern_output_03_init();
	extern_output_04_init();

	#if config_HAS_LED_MATRIX == 1
	GET_SYSTEM(SYS_SIGNAL).led_01 = io_output_controller_register_output(&led_01);
	GET_SYSTEM(SYS_SIGNAL).led_02 = io_output_controller_register_output(&led_02);
	GET_SYSTEM(SYS_SIGNAL).led_03 = io_output_controller_register_output(&led_03);
	GET_SYSTEM(SYS_SIGNAL).led_04 = io_output_controller_register_output(&led_04);
	GET_SYSTEM(SYS_SIGNAL).led_05 = io_output_controller_register_output(&led_05);
	GET_SYSTEM(SYS_SIGNAL).led_06 = io_output_controller_register_output(&led_06);
	GET_SYSTEM(SYS_SIGNAL).led_07 = io_output_controller_register_output(&led_07);
	GET_SYSTEM(SYS_SIGNAL).led_08 = io_output_controller_register_output(&led_08);
	GET_SYSTEM(SYS_SIGNAL).led_09 = io_output_controller_register_output(&led_09);
	#endif
}
