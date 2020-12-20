/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "io_management/io_controller.h"
#include "power_management/power_management.h"

#ifdef EXPANSION_BOARD_PC9670_AVAILABLE
#include "driver_PCA9670.h"
#endif

//----------------------------------------------------------------------------

#if config_HAS_ONBOARD_BUTTONS == 1
IO_CONTROLLER_BUILD_INPUT(onboard_input_01, ONBOARD_INPUT_01)
IO_CONTROLLER_BUILD_INPUT(onboard_input_02, ONBOARD_INPUT_02)
#endif

#ifdef HAS_GPIO_EXTERN_OUTPUT_01
IO_CONTROLLER_BUILD_INPUT(extern_input_01, EXTERN_INPUT_01)
#endif

#ifdef HAS_GPIO_EXTERN_OUTPUT_02
IO_CONTROLLER_BUILD_INPUT(extern_input_02, EXTERN_INPUT_02)
#endif

#ifdef HAS_GPIO_EXTERN_OUTPUT_03
IO_CONTROLLER_BUILD_INPUT(extern_input_03, EXTERN_INPUT_03)
#endif

#ifdef HAS_GPIO_EXTERN_OUTPUT_04
IO_CONTROLLER_BUILD_INPUT(extern_input_04, EXTERN_INPUT_04)
#endif

#ifdef EXPANSION_BOARD_PC9670_AVAILABLE

PCA9670_BUILD_INSTANCE(EXT_BUTTON_INSTANCE_01, 0x20)
PCA9670_BUILD_INSTANCE(EXT_BUTTON_INSTANCE_02, 0x22)
PCA9670_BUILD_INSTANCE(EXT_BUTTON_INSTANCE_03, 0x24)

PCA9670_BUILD_INPUT(EXT_PIN_11, 0x20, PCA9670_PIN_NUM_1)
PCA9670_BUILD_INPUT(EXT_PIN_12, 0x20, PCA9670_PIN_NUM_2)
PCA9670_BUILD_INPUT(EXT_PIN_13, 0x20, PCA9670_PIN_NUM_3)
PCA9670_BUILD_INPUT(EXT_PIN_14, 0x20, PCA9670_PIN_NUM_4)
PCA9670_BUILD_INPUT(EXT_PIN_15, 0x20, PCA9670_PIN_NUM_5)
PCA9670_BUILD_INPUT(EXT_PIN_16, 0x20, PCA9670_PIN_NUM_6)
PCA9670_BUILD_INPUT(EXT_PIN_17, 0x20, PCA9670_PIN_NUM_7)
PCA9670_BUILD_INPUT(EXT_PIN_18, 0x20, PCA9670_PIN_NUM_8)

PCA9670_BUILD_INPUT(EXT_PIN_21, 0x22, PCA9670_PIN_NUM_1)
PCA9670_BUILD_INPUT(EXT_PIN_22, 0x22, PCA9670_PIN_NUM_2)
PCA9670_BUILD_INPUT(EXT_PIN_23, 0x22, PCA9670_PIN_NUM_3)
PCA9670_BUILD_INPUT(EXT_PIN_24, 0x22, PCA9670_PIN_NUM_4)
PCA9670_BUILD_INPUT(EXT_PIN_25, 0x22, PCA9670_PIN_NUM_5)
PCA9670_BUILD_INPUT(EXT_PIN_26, 0x22, PCA9670_PIN_NUM_6)
PCA9670_BUILD_INPUT(EXT_PIN_27, 0x22, PCA9670_PIN_NUM_7)
PCA9670_BUILD_INPUT(EXT_PIN_28, 0x22, PCA9670_PIN_NUM_8)

PCA9670_BUILD_INPUT(EXT_PIN_31, 0x24, PCA9670_PIN_NUM_1)
PCA9670_BUILD_INPUT(EXT_PIN_32, 0x24, PCA9670_PIN_NUM_2)
PCA9670_BUILD_INPUT(EXT_PIN_33, 0x24, PCA9670_PIN_NUM_3)
PCA9670_BUILD_INPUT(EXT_PIN_34, 0x24, PCA9670_PIN_NUM_4)
PCA9670_BUILD_INPUT(EXT_PIN_35, 0x24, PCA9670_PIN_NUM_5)
PCA9670_BUILD_INPUT(EXT_PIN_36, 0x24, PCA9670_PIN_NUM_6)
PCA9670_BUILD_INPUT(EXT_PIN_37, 0x24, PCA9670_PIN_NUM_7)
PCA9670_BUILD_INPUT(EXT_PIN_38, 0x24, PCA9670_PIN_NUM_8)

#endif

void button_initialization(void) {

	DEBUG_PASS("button_initialization()");

	#ifdef HAS_MANAGEMENT_MODULE_IO
	{
		DEBUG_PASS("button_initialization() - io_input_controller_init()");
		io_input_controller_init();
		
		#ifdef HAS_GPIO_EXTERN_OUTPUT_01
		DEBUG_PASS("button_initialization() - extern_input_01_init()");
		extern_input_01_init();
		#endif
		
		#ifdef HAS_GPIO_EXTERN_OUTPUT_01
		DEBUG_PASS("button_initialization() - extern_input_02_init()");
		extern_input_02_init();
		#endif
		
		#ifdef HAS_GPIO_EXTERN_OUTPUT_01
		DEBUG_PASS("button_initialization() - extern_input_03_init()");
		extern_input_03_init();
		#endif
		
		#ifdef HAS_GPIO_EXTERN_OUTPUT_01
		DEBUG_PASS("button_initialization() - extern_input_04_init()");
		extern_input_04_init();
		#endif
	}
	#endif
	
	#ifdef EXPANSION_BOARD_PC9670_AVAILABLE
	{
		PASS(); // button_initialization() - Initializing external Buttons

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
	{
		DEBUG_PASS("button_initialization() - Initializing onboard Buttons");
		onboard_input_01_init();
		onboard_input_02_init();
	}
	#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
