#-----------------------------------------------------------------------------
#       Makefile fuer AVR-GCC Projekte
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# PROJECT: Name des Hexfiles usw.
#-----------------------------------------------------------------------------
# VERSION_STRING muss (!) zwei Ziffern nach dem '.' enthalten, weil
# aus dem String noch weitere Varianten von Version erzeugt
# werden. Beispiel:
# VERSION_STRING = v1.07
# VERSION_DIGITS = 107
# VERSION        = 0x107

# PROJEKT Name des Hexfiles
PROJECT = RPI_Hat_IR_Board

BOARD_ID = 191005

#-----------------------------------------------------------------------------

VERSION_MAJOR		:= 1
VERSION_MINOR		:= 1

#-----------------------------------------------------------------------------

BASE_PATH   = ..
APP_PATH    = $(BASE_PATH)/src
MAKE_PATH   = $(BASE_PATH)/make

#-----------------------------------------------------------------------------

MCU = ATMEGA1284P

#-----------------------------------------------------------------------------

CSRCS += ${APP_PATH}/main_rpi_hat.c

#-----------------------------------------------------------------------------

HOST_INTERFACE_TYPE = I2C

#-----------------------------------------------------------------------------

APP_TASK_CFG += 
#APP_TASK_CFG += LED_MATRIX
#APP_TASK_CFG += TEST_TRACER
APP_TASK_CFG += EVENT

#-----------------------------------------------------------------------------

EXPANSION_BOARD_CFG = 
#EXPANSION_BOARD_CFG += SENSOR_SHT31_ADS1115
#EXPANSION_BOARD_CFG += GPIO_PCA9670

#-----------------------------------------------------------------------------

DRIVER_MODULE_CFG =
DRIVER_MODULE_CFG += GPIO
DRIVER_MODULE_CFG += RTC
DRIVER_MODULE_CFG += CLK
DRIVER_MODULE_CFG += TIMER0
#DRIVER_MODULE_CFG += USART0
#DRIVER_MODULE_CFG += USART1

#-----------------------------------------------------------------------------

SENSOR_MODULE_CFG =
#SENSOR_MODULE_CFG += LIGHT_SENSOR_GM5528

#-----------------------------------------------------------------------------

MANAGEMENT_MODULE_CFG =
#MANAGEMENT_MODULE_CFG += POWER
MANAGEMENT_MODULE_CFG += IO
#MANAGEMENT_MODULE_CFG += RPI_PROTOCOL

#-----------------------------------------------------------------------------

TRACER_CFG =
#TRACER_CFG += USART0
#TRACER_CFG += BAUDRATE_230400
#TRACER_CFG += PARITY_NONE
#TRACER_CFG += DATABITS_8
#TRACER_CFG += STOPBITS_1

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include $(MAKE_PATH)/common_make.mk
