# -----------------------------------------------------------------------
# KEYPAD MODULE
# -----------------------------------------------------------------------

MODULE_KEYPAD_PATH = $(APP_PATH)/modules/keypad

ifneq '' '$(findstring KEYPAD_3X4,$(MODULES_CFG))'
	DEFS += -D KEYPAD_3x4_CONTROLLER_AVAILABLE=1
	CSRCS += $(MODULE_KEYPAD_PATH)/keypad_controller_3x4.c

	DRIVER_MODULE_CFG += KEYPAD_3x4
endif

# -----------------------------------------------------------------------
