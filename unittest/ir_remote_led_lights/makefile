
#-----------------------------------------------------------------------------

PROJECT = UNITTEST_ir_remote_led_lights

BOARD_ID = UNITTEST

PLATTFORM = MACOS

#-----------------------------------------------------------------------------

VERSION_MAJOR		:= 1
VERSION_MINOR		:= 0

#-----------------------------------------------------------------------------

BASE_PATH   = ../../..
FRMWRK_PATH = $(BASE_PATH)/rpi_control_frmwrk
APP_PATH    = $(FRMWRK_PATH)/src
MAKE_PATH   = $(FRMWRK_PATH)/make

#-----------------------------------------------------------------------------

MCU = UNITTEST

#-----------------------------------------------------------------------------

CSRCS += unittest_ir_remote_led_lights.c

#-----------------------------------------------------------------------------

#HOST_INTERFACE_TYPE = SPI

#-----------------------------------------------------------------------------

COMMON_MODULES =
COMMON_MODULES += QEUE
#COMMON_MODULES += EXCEPTION
#COMMON_MODULES += DATETIME
COMMON_MODULES += STRING

#-----------------------------------------------------------------------------

APP_TASK_CFG = 
#APP_TASK_CFG += EVENT
#APP_TASK_CFG += EVENT
#APP_TASK_CFG += COPRO_ROUTING
#APP_TASK_CFG += LED_MATRIX
#APP_TASK_CFG += TEST_TRACER

#-----------------------------------------------------------------------------

MODULES_CFG = 
#MODULES_CFG += IR_PROTOCOL_SONY
#MODULES_CFG += IR_PROTOCOL_NEC
MODULES_CFG += IR_COMMAND_LED_LIGHT

#-----------------------------------------------------------------------------

EXPANSION_BOARD_CFG = 
#EXPANSION_BOARD_CFG += SENSOR_SHT31_ADS1115
#EXPANSION_BOARD_CFG += GPIO_PCA9670

#-----------------------------------------------------------------------------

DRIVER_MODULE_CFG =
DRIVER_MODULE_CFG += GPIO
DRIVER_MODULE_CFG += RTC
DRIVER_MODULE_CFG += CLK
#DRIVER_MODULE_CFG += USART0
#DRIVER_MODULE_CFG += I2C0
#DRIVER_MODULE_CFG += SPI0

#-----------------------------------------------------------------------------

PROTOCOL_CFG =
#PROTOCOL_CFG += RPI_PROTOCOL_CLIENT
#PROTOCOL_CFG += RPI_PROTOCOL_HOST
#PROTOCOL_CFG += JSON_PARSER

#-----------------------------------------------------------------------------

SENSOR_MODULE_CFG =
#SENSOR_MODULE_CFG += LIGHT_SENSOR_GM5528

#-----------------------------------------------------------------------------

MANAGEMENT_MODULE_CFG =
MANAGEMENT_MODULE_CFG += TIMER
#MANAGEMENT_MODULE_CFG += POWER
#MANAGEMENT_MODULE_CFG += IO
#MANAGEMENT_MODULE_CFG += RPI_PROTOCOL_HOST

#-----------------------------------------------------------------------------

COMMAND_INTERFACE_CFG = 
#COMMAND_INTERFACE_CFG += RPI_CMD_IO
#COMMAND_INTERFACE_CFG += RPI_CMD_SENSOR
#COMMAND_INTERFACE_CFG += RPI_CMD_ROUTING
#COMMAND_INTERFACE_CFG += RPI_CMD_IR_REMOTE_SONY
#COMMAND_INTERFACE_CFG += RPI_CMD_IR_REMOTE_LED_LIGHTS

#-----------------------------------------------------------------------------

COPRO_INTERFACE_CFG =
#COPRO_INTERFACE_CFG += COPRO1_I2C0

#-----------------------------------------------------------------------------

USER_INTERFACE_CFG =
#USER_INTERFACE_CFG += CFG_FILE_PARSER
#USER_INTERFACE_CFG += CLI_ARG_DEVICE
#USER_INTERFACE_CFG += CLI_ARG_COMMAND
#USER_INTERFACE_CFG += CONSOLE
#USER_INTERFACE_CFG += LCD_16X2
#USER_INTERFACE_CFG += FILE_INTERFACE

#-----------------------------------------------------------------------------

TRACER_CFG =
TRACER_CFG += CONSOLE
#TRACER_CFG += BAUDRATE_230400
#TRACER_CFG += PARITY_NONE
#TRACER_CFG += DATABITS_8
#TRACER_CFG += STOPBITS_1

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include $(MAKE_PATH)/common_make.mk
