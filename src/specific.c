
#include "config.h"  // immer als erstes einbinden!


#define noTRACES
#include <traces.h>

void specific_i2c_power_down(void) {
	I2C0_SCL_ON();
	I2C0_SDA_ON();
}

void specific_spi_power_down(void) {

}

void BEEPER_ON(void) {

}

void BEEPER_OFF(void) {

}
