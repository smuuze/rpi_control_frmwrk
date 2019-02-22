#-----------------------------------------------------------------------------
#       Makefile for TRACER
#-----------------------------------------------------------------------------

ifneq '' '$(findstring ENABLED,$(TRACER_CFG))'

	DEFS += -D TRACER_ENABLED=1

	CSRCS += $(TRACER_INC_PATH)/tracer.c
	
	ifneq '' '$(findstring USART0,$(TRACER_CFG))'
	DRIVER_MODULE_CFG += USART0
	DEFS += -D TRACER_INTERFACE_USART0=1
	endif

endif