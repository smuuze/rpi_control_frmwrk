
PROTOCOL_PATH =  $(APP_PATH)/protocol_management

ifneq '' '$(findstring RPI_PROTOCOL,$(PROTOCOL_CFG))'

	DEFS  += -D HAS_MANAGEMENT_MODULE_RPI_PROTOCOL=1

	ifneq '' '$(findstring RPI_PROTOCOL_I2C,$(PROTOCOL_CFG))'
		CSRCS += $(PROTOCOL_PATH)/rpi_protocol_handler_i2c.c

	else ifneq '' '$(findstring RPI_PROTOCOL_HOST,$(PROTOCOL_CFG))'
		CSRCS += $(PROTOCOL_PATH)/rpi_protocol_handler_host.c

	else ifneq '' '$(findstring RPI_PROTOCOL_CLIENT,$(PROTOCOL_CFG))'
		CSRCS += $(PROTOCOL_PATH)/rpi_protocol_handler_client.c
	else
		CSRCS += $(PROTOCOL_PATH)/rpi_protocol_handler.c
	endif
endif

ifneq '' '$(findstring MQTT_PROTOCOL,$(PROTOCOL_CFG))'

	DEFS  += -D HAS_PROTOCOL_MQTT=1
	
	CSRCS += $(PROTOCOL_PATH)/mqtt/mqtt_interface.c
	CSRCS += $(PROTOCOL_PATH)/mqtt/mqtt_application.c
	
	LIBS += -l paho-mqtt3c
endif