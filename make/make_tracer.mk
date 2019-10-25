#-----------------------------------------------------------------------------
#       Makefile for TRACER
#-----------------------------------------------------------------------------


TRACER_INC_PATH = $(APP_PATH)/tracer
INC_PATH += $(TRACER_INC_PATH)

ifdef TRACER_CFG

ifneq '' '$(findstring CONSOLE,$(TRACER_CFG))'
	CSRCS += $(TRACER_INC_PATH)/tracer_console.c
endif
	
ifneq '' '$(findstring USART0,$(TRACER_CFG))'
	DRIVER_MODULE_CFG += USART0
	DEFS += -D TRACER_INTERFACE_USART0=1
	CSRCS += $(TRACER_INC_PATH)/tracer.c
endif
	
ifneq '' '$(findstring USART1,$(TRACER_CFG))'
	DRIVER_MODULE_CFG += USART1
	DEFS += -D TRACER_INTERFACE_USART1=1
	CSRCS += $(TRACER_INC_PATH)/tracer.c
endif

ifneq '' '$(findstring BAUDRATE_9600,$(TRACER_CFG))'
	DEFS += -D TRACER_BAUDRATE_9600=1
endif

ifneq '' '$(findstring BAUDRATE_230400,$(TRACER_CFG))'
	DEFS += -D TRACER_BAUDRATE_230400=1
endif

endif

TRACER_VERSION	= 1.1

TRACER_EXE_FILE = Tracer.exe
TRACER_BIN_PATH = ../../Tracer/Release/$(TRACER_VERSION)
TRACER_WORKING_PATH = ../../../../RPi_Hat_Derivate/$(notdir $(CURDIR))/
