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
PROJECT = RPI_Hat_ControlBoard_V2

# HW_DEVICE_TYPE ( siehe http://ew.deister-gmbh.de/cgi-bin/ibase/gertyp.cgi )
HW_DEVICE_TYPE = 0x22B2

# SW_DEVICE_TYPE wird im deBus Versionstelegramm und fuer Adressumstellung verwendet
SW_DEVICE_TYPE = 0x22B2

BOARD_ID = 180920

#-----------------------------------------------------------------------------

VERSION_MAJOR		:= 3
VERSION_MINOR		:= 0

#-----------------------------------------------------------------------------

BASE_PATH   = ../../../..
APP_PATH    = ../src
MAKE_PATH   = ../make

#-----------------------------------------------------------------------------

MCU = ATMEGA1284P

#-----------------------------------------------------------------------------

CSRCS += ${APP_PATH}/main_rpi_hat.c

#-----------------------------------------------------------------------------

HOST_INTERFACE_TYPE = SPI

#-----------------------------------------------------------------------------

APP_TASK_CFG += 
APP_TASK_CFG += ADC_ADS1115
APP_TASK_CFG += TEMP_HUM_SHT31
#APP_TASK_CFG += LED_MATRIX
#APP_TASK_CFG += TEST_TRACER

#-----------------------------------------------------------------------------

DRIVER_MODULE_CFG =

#-----------------------------------------------------------------------------

TRACER_CFG =
TRACER_CFG += USART0

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include $(MAKE_PATH)/common_make.mk
