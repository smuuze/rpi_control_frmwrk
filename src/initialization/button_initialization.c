/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "io_controller.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#if config_HAS_ONBOARD_BUTTONS == 1

IO_CONTROLLER_BUILD_INPUT(onboard_input_01, ONBOARD_INPUT_01)
IO_CONTROLLER_BUILD_INPUT(onboard_input_02, ONBOARD_INPUT_02)

#endif

IO_CONTROLLER_BUILD_INPUT(extern_input_01, EXTERN_INPUT_01)
IO_CONTROLLER_BUILD_INPUT(extern_input_02, EXTERN_INPUT_02)
IO_CONTROLLER_BUILD_INPUT(extern_input_03, EXTERN_INPUT_03)
IO_CONTROLLER_BUILD_INPUT(extern_input_04, EXTERN_INPUT_04)

void button_initialization(void) {

	PASS(); // local_button_watcher_initialization()

	io_input_controller_init();

	extern_input_01_init();
	extern_input_02_init();
	extern_input_03_init();
	extern_input_04_init();

	#if config_HAS_ONBOARD_BUTTONS == 1
	onboard_input_01_init();
	onboard_input_02_init();
	#endif
}

// ----------------------------------------------------------------------------------------------------------------------------

