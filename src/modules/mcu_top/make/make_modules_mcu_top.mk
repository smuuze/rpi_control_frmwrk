# -----------------------------------------------------------------------
# MCU-TOP MODULE
# -----------------------------------------------------------------------

MCU_TOP_PATH = $(APP_PATH)/modules/mcu_top

ifneq '' '$(findstring MCU_TOP,$(MODULES_CFG))'
	DEFS += -D MCU_TOP_AVAILABLE=1
	CSRCS += $(MCU_TOP_PATH)/mcu_top.c

	INC_PATH += $(MCU_TOP_PATH)

endif

# -----------------------------------------------------------------------
