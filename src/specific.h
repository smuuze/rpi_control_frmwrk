#ifndef _SPECIFIC_H_
#define _SPECIFIC_H_

// Dies ist die specific.h aus dem PRL5A Projekt

#include "config.h"  // immer als erstes einbinden!

/*!
 *
 */
void specific_i2c_power_down(void);

/*!
 *
 */
void specific_spi_power_down(void);

/*!
 *
 * @return
 */
u8 specific_onboard_button_01_pinstate(void);

/*!
 *
 * @return
 */
u8 specific_onboard_button_02_pinstate(void);

/*!
 *
 * @return
 */
u8 specific_extern_input_01_pinstate(void);

/*!
 *
 * @return
 */
u8 specific_extern_input_02_pinstate(void);

/*!
 *
 * @return
 */
u8 specific_extern_input_03_pinstate(void);

/*!
 *
 * @return
 */
u8 specific_extern_input_04_pinstate(void);

/*!
 *
 * @param state
 */
void specific_extern_output_01_set(u8 state);

/*!
 *
 * @param state
 */
void specific_extern_output_02_set(u8 state);

/*!
 *
 * @param state
 */
void specific_extern_output_03_set(u8 state);

/*!
 *
 * @param state
 */
void specific_extern_output_04_set(u8 state);

/*!
 *
 * @param state
 */
void specific_system_output_event_set(u8 state);

/*!
 *
 * @param state
 */
void specific_system_output_busy_set(u8 state);

/*!
 *
 * @param set_on
 */
void specific_set_led_01(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_02(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_03(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_04(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_05(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_06(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_07(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_08(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_led_09(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_output_01(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_output_02(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_output_03(u8 set_on);

/*!
 *
 * @param set_on
 */
void specific_set_output_04(u8 set_on);


#endif  // _SPECIFIC_H_
