
COPRO_SRC_PATH = $(APP_PATH)/copro

ifneq '' '$(findstring COPRO1,$(COPRO_INTERFACE_CFG))'
	COPRO_INTERFACE_CFG += COPRO_BASE
	DEFS += -D COPRO1_AVAILABLE=1

	ifneq '' '$(findstring COPRO1_I2C0,$(COPRO_INTERFACE_CFG))'
		DEFS += -D COPRO1_I2C0=1
	endif

endif

ifneq '' '$(findstring COPRO_BASE,$(COPRO_INTERFACE_CFG))'
	DEFS += -D COPRO_INTERFACE_AVAILABLE=1
	CSRCS += $(COPRO_SRC_PATH)/copro.c
endif

ifneq '' '$(findstring COPRO_ROUTING,$(APP_TASK_CFG))'
	DEFS += -D HAS_COPRO_ROUTING=1
	CSRCS += $(COPRO_SRC_PATH)/copro_routing.c
endif
