
#include "config.h"  // immer als erstes einbinden!

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

void specific_i2c_power_down(void) {
	I2C_SCL_ON();
	I2C_SDA_ON();
}

void specific_spi_power_down(void) {

}

void BEEPER_ON(void) {

}

void BEEPER_OFF(void) {

}
