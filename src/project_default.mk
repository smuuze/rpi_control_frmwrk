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
VERSION         ?= 0x011A

# -----------------------------------------------------------------------
# Definitions: [-D name[=definition]...] [-U name...]
# Things that might be added to DEFS:
#   BOARD             Board used: see $(BRDS_PATH)/board.h
#   EXT_BOARD         Extension board used (if any): see $(BRDS_PATH)/board.h
DEFS += -D BOARD=$(BOARD)
DEFS += -D BOARD_ID=$(BOARD_ID)
DEFS += -D HW_DEVICE_TYPE=$(HW_DEVICE_TYPE)
DEFS += -D SW_DEVICE_TYPE=$(SW_DEVICE_TYPE)
DEFS += -D VERSION=$(VERSION)


#! Man koennte den Namen auch so gestalten dass der Dateiname gleich die
#! Version usw. traegt.
#! PROJECT = hello_$(HW_DEVICE_TYPE)_$(VERSION_STRING)

# Praktikantenvariante:  IBLCHIF_CTRL ?= -mega128 -d

# CPU: name of the cpu directory in framework/cpu/$(CPUSERIES)/

ATPROGRAM_CLOCK		= 1Mhz

BASE_PATH    = ../../../..

#-----------------------------------------------------------------------------
#       Prepare Variable settings
#-----------------------------------------------------------------------------
#       here is the place to overwrite settings done by the tpl/ *.mk
TARGET_FILE_NAME = $(PROJECT)

include $(MAKE_TEMPLATE_PATH)/iar_debus_framework.mk

# -----------------------------------------------------------------------
# Include path

PROTOCOL_PATH	= $(SERV_PATH)/protocol
COMMON_PATH	= $(SERV_PATH)/common
FLASH_PATH	= $(SERV_PATH)/ui/flash
CRYPT_PATH	= $(SERV_PATH)/crypt
CFGPIECES_PATH  = $(SERV_PATH)/cfgpieces/PRD5A_derivate

INC_PATH 	+= $(APP_PATH)
INC_PATH 	+= $(COMMON_PATH)
INC_PATH 	+= $(CFGPIECES_PATH)
INC_PATH 	+= .

# ---- DEFAULT APP CONFIGURATION ----

APP_TASK_LIST	?= 


##############################################################################

# C source files

INC_PATH += $(RTOS_PORT_PATH)
#CSRCS += $(FWRK_PATH)/utils/misc.c
#CSRCS += $(CPUS_PATH)/common/ffifo.c
#CSRCS += $(CPUS_PATH)/common/bitbuf.c

INC_PATH += $(ISO14443A_PATH)
INC_PATH += $(APP_PATH)
INC_PATH += $(COMMON_PATH)
INC_PATH += $(CFGPIECES_PATH)


# -----------------------------------------------------------------------
BOARD_INC_PATH = ${APP_PATH}/platine
INC_PATH += $(BOARD_INC_PATH)

CSRCS += ${BOARD_INC_PATH}/board_${BOARD_ID}.c

# ---- COMMON MODULES ---------------------------------------------------
COMMON_INC_PATH = $(APP_PATH)/common
INC_PATH += $(COMMON_INC_PATH)

CSRCS += $(COMMON_INC_PATH)/local_module_status.c
CSRCS += $(COMMON_INC_PATH)/local_mutex.c
CSRCS += $(COMMON_INC_PATH)/local_progmem_load.c
CSRCS += $(COMMON_INC_PATH)/local_data_storage_array.c
CSRCS += $(COMMON_INC_PATH)/local_msg_buffer.c

# Projekt-Spezifische Quelldateien --------------------------------------

CSRCS += $(APP_PATH)/specific.c

# ---- SYSTEM INTERFACE -------------------------------------------------

SYSTEM_INC_PATH = $(APP_PATH)/system
INC_PATH += $(SYSTEM_INC_PATH)

CSRCS += $(SYSTEM_INC_PATH)/system_interface.c

# ---- TRACER -----------------------------------------------------------

ifdef TRACER_CFG
	
TRACER_INC_PATH = $(APP_PATH)/tracer
INC_PATH += $(TRACER_INC_PATH)

include $(TRACER_INC_PATH)/make_tracer.mk

endif

# ---- HOST-INTERFACE ---------------------------------------------------

ifneq '' '$(findstring SPI,$(HOST_INTERFACE_TYPE))'
	DEFS += -D HOST_INTERFACE_SPI=1
	DRIVER_MODULE_CFG += SPI0
else
ifneq '' '$(findstring HOST_INTERFACE_TYPE,$(APP_TASK_CFG))'
	DEFS += -D HOST_INTERFACE_USART=1
	DRIVER_MODULE_CFG += USART0
endif
endif

# ---- DRIVER MODULES ---------------------------------------------------

DRIVER_INC_PATH = $(APP_PATH)/driver
INC_PATH += $(DRIVER_INC_PATH)
CSRCS += $(APP_PATH)/driver/local_rtc_driver.c
CSRCS += $(APP_PATH)/driver/local_clk_driver.c
CSRCS += $(APP_PATH)/driver/local_gpio_driver.c

DEFS += -D HAS_DRIVER_I2C0=1
CSRCS += $(APP_PATH)/driver/local_i2c_driver.c

ifneq '' '$(findstring SPI0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_SPI0=1
	CSRCS += $(APP_PATH)/driver/local_spi_driver.c
endif

ifneq '' '$(findstring USART0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_USART0=1
	CSRCS += $(APP_PATH)/driver/local_usart_driver.c
endif

# -----------------------------------------------------------------------

COMMAND_MANAGEMENT_INC_PATH = $(APP_PATH)/command_management
INC_PATH += $(COMMAND_MANAGEMENT_INC_PATH)

CSRCS += $(COMMAND_MANAGEMENT_INC_PATH)/command_controller.c

# -----------------------------------------------------------------------

COMMAND_HANDLER_INC_PATH = $(APP_PATH)/command_handler
INC_PATH += $(COMMAND_HANDLER_INC_PATH)

CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_command_handler.c

# -----------------------------------------------------------------------

PROTOCOL_MANAGEMENT_INC_PATH = $(APP_PATH)/protocol_management
INC_PATH += $(PROTOCOL_MANAGEMENT_INC_PATH)

CSRCS += $(PROTOCOL_MANAGEMENT_INC_PATH)/rpi_protocol_handler.c

# -----------------------------------------------------------------------

TIME_MANAGEMENT_INC_PATH = $(APP_PATH)/time_management
INC_PATH += $(TIME_MANAGEMENT_INC_PATH)

CSRCS += $(TIME_MANAGEMENT_INC_PATH)/time_management.c

# ---- TASK MANAGEMENT -------------------------------------------------------------------

MCU_TASK_MANAGEMENT_INC_PATH = $(APP_PATH)/mcu_task_management
INC_PATH += $(MCU_TASK_MANAGEMENT_INC_PATH)

APP_TASK_INC_PATH = $(APP_PATH)/app_tasks
INC_PATH += $(APP_TASK_INC_PATH)

CSRCS += $(MCU_TASK_MANAGEMENT_INC_PATH)/mcu_task_controller.c
CSRCS += $(MCU_TASK_MANAGEMENT_INC_PATH)/mcu_idle_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_event_task.c
CSRCS += $(APP_TASK_INC_PATH)/local_cmd_mcu_task.c

ifneq '' '$(findstring LED_MATRIX,$(APP_TASK_CFG))'
DEFS  += -D HAS_APP_TASK_LED_MATRIX=1
CSRCS += $(APP_TASK_INC_PATH)/local_led_mcu_task.c
endif

ifneq '' '$(findstring DEBUS,$(APP_TASK_CFG))'
DEFS += -D HAS_APP_TASK_DEBUS=1
CSRCS += $(APP_TASK_INC_PATH)/local_debus_mcu_task.c
endif

ifneq '' '$(findstring TEST_TRACER,$(APP_TASK_CFG))'
DEFS += -D HAS_APP_TASK_TEST_TRACER=1
CSRCS += $(APP_TASK_INC_PATH)/test_tracer_mcu_task.c
endif

# ---- EXPANSION BOARDS -------------------------------------------------------------------

EXPANSION_BOARD_PATH = $(APP_PATH)/expansion_boards
INC_PATH += $(EXPANSION_BOARD_PATH)

ifneq '' '$(findstring SENSOR_SHT31_ADS1115,$(EXPANSION_BOARD_CFG))'
	DEFS  += -D HAS_APP_TASK_ADC_ADS1115=1
	DEFS  += -D HAS_APP_TASK_TEMP_HUM_SHT31=1
	
	CSRCS += $(EXPANSION_BOARD_PATH)/local_ads1115_mcu_task.c
	CSRCS += $(EXPANSION_BOARD_PATH)/local_sht31_mcu_task.c
endif

ifneq '' '$(findstring GPIO_PCA9670,$(EXPANSION_BOARD_CFG))'
	DEFS  += -D EXPANSION_BOARD_PC9670_AVAILABLE=1
	CSRCS += $(EXPANSION_BOARD_PATH)/local_sht31_mcu_task.c
endif

# -----------------------------------------------------------------------

IO_CONTROLLER_INC_PATH = $(APP_PATH)/io_management
INC_PATH += $(IO_CONTROLLER_INC_PATH)

CSRCS += $(IO_CONTROLLER_INC_PATH)/io_output_controller.c
CSRCS += $(IO_CONTROLLER_INC_PATH)/io_input_controller.c

# -----------------------------------------------------------------------

INITIALIZATION_INC_PATH = $(APP_PATH)/initialization
INC_PATH += $(INITIALIZATION_INC_PATH)

CSRCS += $(INITIALIZATION_INC_PATH)/button_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/output_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/protocol_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/command_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/task_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/system_initialization.c
CSRCS += $(INITIALIZATION_INC_PATH)/initialization.c

# -----------------------------------------------------------------------

# TK-Debug: fehlt noch: C++ source files:
CPPSRCS +=

# -----------------------------------------------------------------------

# Assembler source files
IARSRCS +=
ASSRCS += 
# ASSRCS += $(COMMON_PATH)/format_and_conversion/algo.S


ifdef DEBUS_INTERFACE_CFG

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
#include $(SERV_PATH)/debus/debus_interface.mk

endif


# -----------------------------------------------------------------------
# (gemeinsame) WebConfig - Beschreibung fuer verschiedene Konfigurationen
IARSRCS_WEBCONFIG_ROAD_SCRIPT = ../config/webconfig_road.cfg

# -----------------------------------------------------------------------
# Library path
LIB_PATH +=

# -----------------------------------------------------------------------
# Libraries to link with the project
LIBS +=

# -----------------------------------------------------------------------
# Device/Platform/Board include path
PLATFORM_INC_PATH += $(CPUS_PATH)/include
PLATFORM_INC_PATH += $(BRDS_PATH)

# -----------------------------------------------------------------------
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
#$(foreach var,$(.VARIABLES), $(info $(var) : $(value $(var)) )  )
