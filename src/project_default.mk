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

#! Man koennte den Namen auch so gestalten dass der Dateiname gleich die
#! Version usw. traegt.
#! PROJECT = hello_$(HW_DEVICE_TYPE)_$(VERSION_STRING)

# Praktikantenvariante:  IBLCHIF_CTRL ?= -mega128 -d

# CPU: name of the cpu directory in framework/cpu/$(CPUSERIES)/

ATPROGRAM_CLOCK		= 1Mhz

BASE_PATH    = ../../../..
# --------------------------------------------------------------------------------
RTOS_PATH  = $(SERV_PATH)/125khz/avr
RTOS_PORT_PATH = $(SERV_PATH)/125khz/avr
# --------------------------------------------------------------------------------
ISO14443A_PATH = $(SERV_PATH)/iso14443a
SM4x00_PATH = $(SERV_PATH)/Legic/SM4x00
SAMX_PATH = $(SERV_PATH)/iso14443a/samx
KEYPAD_PATH = $(SERV_PATH)/ui/keypad
KHZ125_PATH = $(SERV_PATH)/125khz
KHZ125_PORT_PATH = $(SERV_PATH)/125khz/avr
PROTOCOL_PATH=$(SERV_PATH)/protocol
SIGNALING_PATH = $(SERV_PATH)/ui/signaling
BIOMETRY_PATH = $(SERV_PATH)/protocol/biometry
COMMON_PATH=$(SERV_PATH)/common
PRD5A_DERIVATE_PATH=$(SERV_PATH)/common/PRD5A_derivate
FLASH_PATH = $(SERV_PATH)/ui/flash
CRYPT_PATH = $(SERV_PATH)/crypt
# --------------------------------------------------------------------------------
CFGPIECES_PATH = $(SERV_PATH)/cfgpieces/PRD5A_derivate
# --------------------------------------------------------------------------------


#-----------------------------------------------------------------------------
#       Prepare Variable settings
#-----------------------------------------------------------------------------
#       here is the place to overwrite settings done by the tpl/ *.mk
TARGET_FILE_NAME = $(PROJECT)

include $(MAKE_TEMPLATE_PATH)/iar_debus_framework.mk

APP_PATH		?= ../src


##############################################################################

# C source files

INC_PATH += $(RTOS_PORT_PATH)
CSRCS += $(FWRK_PATH)/utils/misc.c
CSRCS += $(CPUS_PATH)/common/ffifo.c
CSRCS += $(CPUS_PATH)/common/bitbuf.c

INC_PATH += $(ISO14443A_PATH)
INC_PATH += $(APP_PATH)
INC_PATH += $(COMMON_PATH)
INC_PATH += $(CFGPIECES_PATH)


# -----------------------------------------------------------------------
COMMON_INC_PATH = ../src/common
INC_PATH += $(COMMON_INC_PATH)

CSRCS += $(COMMON_INC_PATH)/local_module_status.c
CSRCS += $(COMMON_INC_PATH)/local_mutex.c
CSRCS += $(COMMON_INC_PATH)/local_progmem_load.c
CSRCS += $(COMMON_INC_PATH)/local_data_storage_array.c

# Framework-Spezifische Quelldateien ------------------------------------

##CSRCS += $(PRD5A_DERIVATE_PATH)/timer.c



# Projekt-Spezifische Quelldateien --------------------------------------

CSRCS += $(APP_PATH)/specific.c

#CSRCS += $(APP_PATH)/db_comms.c
CSRCS += $(COMMON_INC_PATH)/local_msg_buffer.c

# -----------------------------------------------------------------------
SYSTEM_INC_PATH = ../src/system
INC_PATH += $(SYSTEM_INC_PATH)
CSRCS += $(SYSTEM_INC_PATH)/system_interface.c

# -----------------------------------------------------------------------
DRIVER_INC_PATH = ../src/driver
INC_PATH += $(DRIVER_INC_PATH)
CSRCS += $(APP_PATH)/driver/local_i2c_driver.c
CSRCS += $(APP_PATH)/driver/local_spi_driver.c
CSRCS += $(APP_PATH)/driver/local_rtc_driver.c

ifdef MIT_LOCAL_FAKERTOS
CSRCS += $(APP_PATH)/driver/local_usart_driver.c
endif

# -----------------------------------------------------------------------

COMMAND_MANAGEMENT_INC_PATH = ../src/command_management
INC_PATH += $(COMMAND_MANAGEMENT_INC_PATH)

CSRCS += $(COMMAND_MANAGEMENT_INC_PATH)/command_controller.c

# -----------------------------------------------------------------------

DRIVER_INC_PATH = ../src/driver
INC_PATH += $(DRIVER_INC_PATH)


# -----------------------------------------------------------------------

COMMAND_HANDLER_INC_PATH = ../src/command_handler
INC_PATH += $(COMMAND_HANDLER_INC_PATH)

CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_command_handler.c

# -----------------------------------------------------------------------

PROTOCOL_MANAGEMENT_INC_PATH = ../src/protocol_management
INC_PATH += $(PROTOCOL_MANAGEMENT_INC_PATH)

CSRCS += $(PROTOCOL_MANAGEMENT_INC_PATH)/rpi_protocol_handler.c

# -----------------------------------------------------------------------

MCU_TASK_MANAGEMENT_INC_PATH = ../src/mcu_task_management
INC_PATH += $(MCU_TASK_MANAGEMENT_INC_PATH)

CSRCS += $(MCU_TASK_MANAGEMENT_INC_PATH)/mcu_idle_task.c
CSRCS += $(MCU_TASK_MANAGEMENT_INC_PATH)/mcu_task_controller.c

APP_TASK_INC_PATH = ../src/app_tasks
INC_PATH += $(APP_TASK_INC_PATH)

CSRCS += $(APP_TASK_INC_PATH)/local_ads1115_mcu_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_sht31_mcu_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_cmd_mcu_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_led_mcu_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_event_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_debus_mcu_task.c

# -----------------------------------------------------------------------

IO_CONTROLLER_INC_PATH = ../src/io_management
INC_PATH += $(IO_CONTROLLER_INC_PATH)
CSRCS += $(IO_CONTROLLER_INC_PATH)/io_output_controller.c
CSRCS += $(IO_CONTROLLER_INC_PATH)/button_watcher.c

# -----------------------------------------------------------------------

INITIALIZATION_INC_PATH = ../src/initialization
INC_PATH += $(INITIALIZATION_INC_PATH)
CSRCS += $(INITIALIZATION_INC_PATH)/button_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/output_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/protocol_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/command_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/task_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/system_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/initialization.c

# -----------------------------------------------------------------------

# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
INC_PATH += $(SERV_PATH)/rtc

# -----------------------------------------------------------------------

# TK-Debug: fehlt noch: C++ source files:
CPPSRCS +=

# Assembler source files

ASSRCS += $(COMMON_PATH)/format_and_conversion/algo.S

IARSRCS +=

DEBUS_INTERFACE_CFG += HOST
DEBUS_INTERFACE_CFG += AES
DEBUS_INTERFACE_CFG += SIO0

ifdef MIT_COPRO
DEBUS_INTERFACE_CFG += COPRO1_SSM
DEBUS_INTERFACE_CFG += MSGFIFO
DEBUS_INTERFACE_CFG += BLHOOKS
DEBUS_INTERFACE_CFG += I2C
endif

#$(info DEBUS_INTERFACE_CFG: $(DEBUS_INTERFACE_CFG))
include $(SERV_PATH)/debus/debus_interface.mk

# (gemeinsame) WebConfig - Beschreibung fuer verschiedene Konfigurationen
# die Sparversion
#ifndef DOORLOXX
IARSRCS_WEBCONFIG_ROAD_SCRIPT = ../config/webconfig_road.cfg
#else
#IARSRCS_WEBCONFIG_ROAD_SCRIPT = $(SERV_PATH)/doorloxx/config_doorloxx/webconfig_road.cfg
#endif
# Library path
LIB_PATH +=

# Libraries to link with the project
LIBS +=

# Device/Platform/Board include path
PLATFORM_INC_PATH += $(CPUS_PATH)/include
PLATFORM_INC_PATH += $(BRDS_PATH)

# Include path
INC_PATH  +=  .
INC_PATH += ../common

# Definitions: [-D name[=definition]...] [-U name...]
# Things that might be added to DEFS:
#   BOARD             Board used: see $(BRDS_PATH)/board.h
#   EXT_BOARD         Extension board used (if any): see $(BRDS_PATH)/board.h
DEFS += -D BOARD=$(BOARD)
DEFS += -D HW_DEVICE_TYPE=$(HW_DEVICE_TYPE)
DEFS += -D SW_DEVICE_TYPE=$(SW_DEVICE_TYPE)
DEFS += -D VERSION=$(VERSION)

# Options to request or suppress warnings: [-fsyntax-only] [-pedantic[-errors]] [-w] [-Wwarning...]
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
WARNINGS += -Wall
WARNINGS += -Wstrict-prototypes

# Options for debugging: [-g]...
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
DEBUG += -g

# Options that control optimization: [-O[0|1|2|3|s]]...
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
# OPTIMIZATION  += -O3 -mcall-prologues
OPTIMIZATION  += -Os
OPTIMIZATION += -mcall-prologues
OPTIMIZATION += -ffunction-sections
OPTIMIZATION += -fshort-enums
OPTIMIZATION += -funsigned-char
OPTIMIZATION += -funsigned-bitfields
#OPTIMIZATION += -fpack-struct
#OPTIMIZATION += -mshort-calls

# Extra flags to use when preprocessing
PP_EXTRA_FLAGS +=

# Extra flags to use when compiling
C_EXTRA_FLAGS +=

# Extra flags to use when assembling
AS_EXTRA_FLAGS +=

# Extra flags to use when linking
LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax
#-Wl,--gc-sections

# Include the generic Makefile
#$(info MAKE_TARGET_RULES: $(MAKE_TARGET_RULES) )
include $(GENERIC_TARGET_RULES) $(MAKE_TARGET_RULES)

# Debug-Ausgabe aller Variablen
# $(foreach var,$(.VARIABLES), $(info $(var) : $(value $(var)) )  )
