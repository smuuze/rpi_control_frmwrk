
#include "config.h"  // immer als erstes einbinden!


#define noTRACES
#include <traces.h>

void specific_i2c_power_down(void) {
	I2C0_SCL_ON();
	I2C0_SDA_ON();
}

void specific_spi_power_down(void) {

}


u8 specific_onboard_button_01_pinstate(void) {
	return SWITCH_IN_1_PIN();
}

u8 specific_onboard_button_02_pinstate(void) {
	return SWITCH_IN_2_PIN();
}

u8 specific_extern_input_01_pinstate(void) {
	return EXT_INPUT_1_PIN();
}

u8 specific_extern_input_02_pinstate(void) {
	return EXT_INPUT_2_PIN();
}

u8 specific_extern_input_03_pinstate(void) {
	return EXT_INPUT_3_PIN();
}

u8 specific_extern_input_04_pinstate(void) {
	return EXT_INPUT_4_PIN();
}

void specific_system_output_event_set(u8 state) {

	if (state != 0) {
		EVENT_IRQ_ON();
	} else {
		EVENT_IRQ_OFF();
	}
}

void specific_system_output_busy_set(u8 state) {

	if (state != 0) {
		IS_BUSY_ON();
	} else {
		IS_BUSY_OFF();
	}
}

void specific_extern_output_01_set(u8 state) {

	if (state != 0) {
		EXT_OUTPUT_1_ON();
	} else {
		EXT_OUTPUT_1_OFF();
	}
}

void specific_extern_output_02_set(u8 state) {

	if (state != 0) {
		EXT_OUTPUT_2_ON();
	} else {
		EXT_OUTPUT_2_OFF();
	}
}

void specific_extern_output_03_set(u8 state) {

	if (state != 0) {
		EXT_OUTPUT_3_ON();
	} else {
		EXT_OUTPUT_3_OFF();
	}
}

void specific_extern_output_04_set(u8 state) {

	if (state != 0) {
		EXT_OUTPUT_4_ON();
	} else {
		EXT_OUTPUT_4_OFF();
	}
}

void specific_set_led_01(u8 set_on) {
	if (set_on != 0) {
		LED_1_ON();
	} else {
		LED_1_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_02(u8 set_on) {
	if (set_on != 0) {
		LED_2_ON();
	} else {
		LED_2_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_03(u8 set_on) {
	if (set_on != 0) {
		LED_3_ON();
	} else {
		LED_3_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_04(u8 set_on) {
	if (set_on != 0) {
		LED_4_ON();
	} else {
		LED_4_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_05(u8 set_on) {
	if (set_on != 0) {
		LED_5_ON();
	} else {
		LED_5_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_06(u8 set_on) {
	if (set_on != 0) {
		LED_6_ON();
	} else {
		LED_6_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_07(u8 set_on) {
	if (set_on != 0) {
		LED_7_ON();
	} else {
		LED_7_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_08(u8 set_on) {
	if (set_on != 0) {
		LED_8_ON();
	} else {
		LED_8_OFF();
	}
}

/*!
 *
 * @param set_on
 */
void specific_set_led_09(u8 set_on) {
	if (set_on != 0) {
		LED_9_ON();
	} else {
		LED_9_OFF();
	}
}

void specific_set_output_01(u8 set_on) {
	if (set_on != 0) {
		EXT_OUTPUT_1_ON();
	} else {
		EXT_OUTPUT_1_OFF();
	}
}

void specific_set_output_02(u8 set_on) {
	if (set_on != 0) {
		EXT_OUTPUT_2_ON();
	} else {
		EXT_OUTPUT_2_OFF();
	}
}

void specific_set_output_03(u8 set_on) {
	if (set_on != 0) {
		EXT_OUTPUT_3_ON();
	} else {
		EXT_OUTPUT_3_OFF();
	}
}

void specific_set_output_04(u8 set_on) {
	if (set_on != 0) {
		EXT_OUTPUT_4_ON();
	} else {
		EXT_OUTPUT_4_OFF();
	}
}

void BEEPER_ON(void) {

}

void BEEPER_OFF(void) {

}
