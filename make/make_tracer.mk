#-----------------------------------------------------------------------------
#       Makefile for TRACER
#-----------------------------------------------------------------------------


TRACER_INC_PATH = $(APP_PATH)/tracer
INC_PATH += $(TRACER_INC_PATH)

ifdef TRACER_CFG
CSRCS += $(TRACER_INC_PATH)/tracer.c
	
ifneq '' '$(findstring USART0,$(TRACER_CFG))'
	DRIVER_MODULE_CFG += USART0
	DEFS += -D TRACER_INTERFACE_USART0=1
endif
endif

TRACER_EXE_FILE = Tracer.exe
TRACER_BIN_PATH = ../../Tracer/Release/1.0
TRACER_WORKING_PATH = ../../../../RPi_Hat_Derivate/$(notdir $(CURDIR))/
