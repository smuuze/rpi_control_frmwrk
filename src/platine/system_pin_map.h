#ifndef _SYS_PIN_MAP_H_
#define _SYS_PIN_MAP_H_

#include "platine/pin_map_cfg.h"

//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(EXTERN_INPUT_01)
INCLUDE_GPIO(EXTERN_INPUT_02)
INCLUDE_GPIO(EXTERN_INPUT_03)
INCLUDE_GPIO(EXTERN_INPUT_04)
INCLUDE_GPIO(ONBOARD_INPUT_01)
INCLUDE_GPIO(ONBOARD_INPUT_02)
INCLUDE_GPIO(MCU_PIN_31)
INCLUDE_GPIO(MCU_PIN_30)

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(EXTERN_OUTPUT_01)
INCLUDE_GPIO(EXTERN_OUTPUT_02)
INCLUDE_GPIO(EXTERN_OUTPUT_03)
INCLUDE_GPIO(EXTERN_OUTPUT_04)
INCLUDE_GPIO(HOST_SPI_CE)
INCLUDE_GPIO(HOST_SPI_MOSI)
INCLUDE_GPIO(HOST_SPI_MISO)
INCLUDE_GPIO(HOST_SPI_SCK)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(MCU_PIN_19)
INCLUDE_GPIO(MCU_PIN_20)
INCLUDE_GPIO(MCU_PIN_21)
INCLUDE_GPIO(MCU_PIN_22)
INCLUDE_GPIO(MCU_PIN_23)
INCLUDE_GPIO(MCU_PIN_24)
INCLUDE_GPIO(MCU_PIN_25)
INCLUDE_GPIO(MCU_PIN_26)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(MCU_PIN_09)
INCLUDE_GPIO(MCU_PIN_10)
INCLUDE_GPIO(READY_INOUT)
INCLUDE_GPIO(MCU_PIN_12)
INCLUDE_GPIO(EVENT_OUTPUT)
INCLUDE_GPIO(MCU_PIN_14)
INCLUDE_GPIO(MCU_PIN_15)
INCLUDE_GPIO(MCU_PIN_16)


#endif
