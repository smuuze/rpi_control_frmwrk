
POWER_MANAGEMENT_PATH = $(APP_PATH)/power_management

#-----------------------------------------------------------------------------

DEFS  += -D HAS_POWER_MANAGEMENT_MODULE=1
CSRCS += $(POWER_MANAGEMENT_PATH)/power_management.c
CSRCS += $(POWER_MANAGEMENT_PATH)/power_management_initialization.c

ifneq '' '$(findstring POWER_5V_MODULE,$(POWER_MANAGEMENT_CFG))'

	DEFS  += -D HAS_POWER_MANAGEMENT_MODULE_5V=1
	CSRCS += $(POWER_MANAGEMENT_PATH)/power_module_5V.c

endif
