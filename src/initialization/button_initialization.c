/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "io_controller.h"

#ifdef EXPANSION_BOARD_PC9670_AVAILABLE
#include "driver_PCA9670.h"
#endif

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

#ifdef EXPANSION_BOARD_PC9670_AVAILABLE

PCA9670_BUILD_INSTANCE(EXT_BUTTON_INSTANCE_01, 0x50)
PCA9670_BUILD_INSTANCE(EXT_BUTTON_INSTANCE_02, 0x51)
PCA9670_BUILD_INSTANCE(EXT_BUTTON_INSTANCE_03, 0x52)

PCA9670_BUILD_INPUT(EXT_PIN_11, 0x50, PCA9670_PIN_NUM_1)
PCA9670_BUILD_INPUT(EXT_PIN_12, 0x50, PCA9670_PIN_NUM_2)
PCA9670_BUILD_INPUT(EXT_PIN_13, 0x50, PCA9670_PIN_NUM_3)
PCA9670_BUILD_INPUT(EXT_PIN_14, 0x50, PCA9670_PIN_NUM_4)
PCA9670_BUILD_INPUT(EXT_PIN_15, 0x50, PCA9670_PIN_NUM_5)
PCA9670_BUILD_INPUT(EXT_PIN_16, 0x50, PCA9670_PIN_NUM_6)
PCA9670_BUILD_INPUT(EXT_PIN_17, 0x50, PCA9670_PIN_NUM_7)
PCA9670_BUILD_INPUT(EXT_PIN_18, 0x50, PCA9670_PIN_NUM_8)

PCA9670_BUILD_INPUT(EXT_PIN_21, 0x51, PCA9670_PIN_NUM_1)
PCA9670_BUILD_INPUT(EXT_PIN_22, 0x51, PCA9670_PIN_NUM_2)
PCA9670_BUILD_INPUT(EXT_PIN_23, 0x51, PCA9670_PIN_NUM_3)
PCA9670_BUILD_INPUT(EXT_PIN_24, 0x51, PCA9670_PIN_NUM_4)
PCA9670_BUILD_INPUT(EXT_PIN_25, 0x51, PCA9670_PIN_NUM_5)
PCA9670_BUILD_INPUT(EXT_PIN_26, 0x51, PCA9670_PIN_NUM_6)
PCA9670_BUILD_INPUT(EXT_PIN_27, 0x51, PCA9670_PIN_NUM_7)
PCA9670_BUILD_INPUT(EXT_PIN_28, 0x51, PCA9670_PIN_NUM_8)

PCA9670_BUILD_INPUT(EXT_PIN_31, 0x52, PCA9670_PIN_NUM_1)
PCA9670_BUILD_INPUT(EXT_PIN_32, 0x52, PCA9670_PIN_NUM_2)
PCA9670_BUILD_INPUT(EXT_PIN_33, 0x52, PCA9670_PIN_NUM_3)
PCA9670_BUILD_INPUT(EXT_PIN_34, 0x52, PCA9670_PIN_NUM_4)
PCA9670_BUILD_INPUT(EXT_PIN_35, 0x52, PCA9670_PIN_NUM_5)
PCA9670_BUILD_INPUT(EXT_PIN_36, 0x52, PCA9670_PIN_NUM_6)
PCA9670_BUILD_INPUT(EXT_PIN_37, 0x52, PCA9670_PIN_NUM_7)
PCA9670_BUILD_INPUT(EXT_PIN_38, 0x52, PCA9670_PIN_NUM_8)

#endif

void button_initialization(void) {

	PASS(); // local_button_watcher_initialization()

	io_input_controller_init();
	
	extern_input_01_init();
	extern_input_02_init();
	extern_input_03_init();
	extern_input_04_init();
	
	#ifdef EXPANSION_BOARD_PC9670_AVAILABLE
	{
		EXT_BUTTON_INSTANCE_01_init();
		
		EXT_PIN_11_init();
		EXT_PIN_12_init();
		EXT_PIN_13_init();
		EXT_PIN_14_init();
		EXT_PIN_15_init();
		EXT_PIN_16_init();
		EXT_PIN_17_init();
		EXT_PIN_18_init();
		
		EXT_BUTTON_INSTANCE_02_init();
		
		EXT_PIN_21_init();
		EXT_PIN_22_init();
		EXT_PIN_23_init();
		EXT_PIN_24_init();
		EXT_PIN_25_init();
		EXT_PIN_26_init();
		EXT_PIN_27_init();
		EXT_PIN_28_init();
		
		EXT_BUTTON_INSTANCE_03_init();
		
		EXT_PIN_31_init();
		EXT_PIN_32_init();
		EXT_PIN_33_init();
		EXT_PIN_34_init();
		EXT_PIN_35_init();
		EXT_PIN_36_init();
		EXT_PIN_37_init();
		EXT_PIN_38_init();
	}
	#endif

	#if config_HAS_ONBOARD_BUTTONS == 1
	onboard_input_01_init();
	onboard_input_02_init();
	#endif
}

// ----------------------------------------------------------------------------------------------------------------------------

