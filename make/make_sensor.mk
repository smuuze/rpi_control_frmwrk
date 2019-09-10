
ifneq '' '$(findstring LIGHT_SENSOR_GM5528,$(SENSOR_MODULE_CFG))'
	DEFS += -D HAS_LIGHT_SENSOR_GM5528=1
	CSRCS += $(APP_PATH)/driver/sensor/light_resistor_gm5528.c
endif