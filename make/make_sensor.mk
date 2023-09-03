
SENSOR_PATH = $(APP_PATH)/sensor
INC_PATH += $(EXPANSION_BOARD_PATH)

# -----------------------------------------------------------------------

ifneq '' '$(findstring LIGHT_SENSOR_GM5528,$(SENSOR_MODULE_CFG))'

	SENSOR_GM5528_PATH = $(SENSOR_PATH)/GM5528
	INC_PATH += $(SENSOR_GM5528_PATH)

	DEFS += -D HAS_LIGHT_SENSOR_GM5528=1
	CSRCS += $(SENSOR_GM5528_PATH)/light_resistor_gm5528.c
endif

# -----------------------------------------------------------------------

ifneq '' '$(findstring SENSOR_SHT31,$(SENSOR_MODULE_CFG))'

	SENSOR_SHT31_PATH = $(SENSOR_PATH)/sht31
	INC_PATH += $(SENSOR_SHT31_PATH)

	DEFS  += -D HAS_SENSOR_SHT31=1
	CSRCS += $(SENSOR_SHT31_PATH)/sht31_interface.c

endif

# -----------------------------------------------------------------------
