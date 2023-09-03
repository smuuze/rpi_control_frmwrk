
SENSOR_PATH = $(APP_PATH)/sensor
INC_PATH += $(EXPANSION_BOARD_PATH)

# -----------------------------------------------------------------------

# Light resistor
ifneq '' '$(findstring SENSOR_GM5528,$(SENSOR_MODULE_CFG))'

	SENSOR_GM5528_PATH = $(SENSOR_PATH)/GM5528
	INC_PATH += $(SENSOR_GM5528_PATH)

	DEFS += -D HAS_LIGHT_SENSOR_GM5528=1
	CSRCS += $(SENSOR_GM5528_PATH)/gm5528_driver.c
endif

# -----------------------------------------------------------------------

# Temperature and humidity sensor
ifneq '' '$(findstring SENSOR_SHT31,$(SENSOR_MODULE_CFG))'

	SENSOR_SHT31_PATH = $(SENSOR_PATH)/sht31
	INC_PATH += $(SENSOR_SHT31_PATH)

	DEFS  += -D HAS_SENSOR_SHT31=1
	CSRCS += $(SENSOR_SHT31_PATH)/sht31_driver.c

endif

# -----------------------------------------------------------------------

# analog digital converter
ifneq '' '$(findstring SENSOR_ADS1115,$(SENSOR_MODULE_CFG))'

	SENSOR_ADS1115_PATH = $(SENSOR_PATH)/ads1115
	INC_PATH += $(SENSOR_ADS1115_PATH)

	DEFS  += -D HAS_SENSOR_ADS1115=1
	CSRCS += $(SENSOR_ADS1115_PATH)/ads1115_driver.c

endif

# -----------------------------------------------------------------------
