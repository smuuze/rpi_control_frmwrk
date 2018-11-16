/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "local_context.h"
#include "button_watcher.h"

#define noTRACES
#include <traces.H>

#if config_HAS_ONBOARD_BUTTONS == 1
static IO_INPUT_DESCRIPTOR onboard_button_01 = {
	0, //u8 id;
	0, //u8 down;
	0, //u8 pressed;
	0, //u8 released;
	0, //u16 __down_time;
	0, //u16 __press_time;
	0, //u16 __release_time;
	&specific_onboard_button_01_pinstate, //IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;
	0 //struct BUTTON_STAT* __next_button;
};

static IO_INPUT_DESCRIPTOR onboard_button_02 = {
	0, //u8 id;
	0, //u8 down;
	0, //u8 pressed;
	0, //u8 released;
	0, //u16 __down_time;
	0, //u16 __press_time;
	0, //u16 __release_time;
	&specific_onboard_button_02_pinstate, //IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;
	0 //struct BUTTON_STAT* __next_button;
};

#endif

static IO_INPUT_DESCRIPTOR extern_input_01 = {
	0, //u8 id;
	0, //u8 down;
	0, //u8 pressed;
	0, //u8 released;
	0, //u16 __down_time;
	0, //u16 __press_time;
	0, //u16 __release_time;
	&specific_extern_input_01_pinstate, //IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;
	0 //struct BUTTON_STAT* __next_button;
};

static IO_INPUT_DESCRIPTOR extern_input_02 = {
	0, //u8 id;
	0, //u8 down;
	0, //u8 pressed;
	0, //u8 released;
	0, //u16 __down_time;
	0, //u16 __press_time;
	0, //u16 __release_time;
	&specific_extern_input_02_pinstate, //IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;
	0 //struct BUTTON_STAT* __next_button;
};

static IO_INPUT_DESCRIPTOR extern_input_03 = {
	0, //u8 id;
	0, //u8 down;
	0, //u8 pressed;
	0, //u8 released;
	0, //u16 __down_time;
	0, //u16 __press_time;
	0, //u16 __release_time;
	&specific_extern_input_03_pinstate, //IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;
	0 //struct BUTTON_STAT* __next_button;
};

static IO_INPUT_DESCRIPTOR extern_input_04 = {
	0, //u8 id;
	0, //u8 down;
	0, //u8 pressed;
	0, //u8 released;
	0, //u16 __down_time;
	0, //u16 __press_time;
	0, //u16 __release_time;
	&specific_extern_input_04_pinstate, //IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;
	0 //struct BUTTON_STAT* __next_button;
};

void button_initialization(void) {

	PASS(); // local_button_watcher_initialization()

	io_input_controller_init();

	GET_SYSTEM(SYS_INPUT).extern_input_01 = io_input_controller_register_input(&extern_input_01);
	GET_SYSTEM(SYS_INPUT).extern_input_02 = io_input_controller_register_input(&extern_input_02);
	GET_SYSTEM(SYS_INPUT).extern_input_03 = io_input_controller_register_input(&extern_input_03);
	GET_SYSTEM(SYS_INPUT).extern_input_04 = io_input_controller_register_input(&extern_input_04);

	#if config_HAS_ONBOARD_BUTTONS == 1
	GET_SYSTEM(SYS_INPUT).onboard_button_01 = io_input_controller_register_input(&onboard_button_01);
	GET_SYSTEM(SYS_INPUT).onboard_button_02 = io_input_controller_register_input(&onboard_button_02);
	#endif
}

// ----------------------------------------------------------------------------------------------------------------------------

