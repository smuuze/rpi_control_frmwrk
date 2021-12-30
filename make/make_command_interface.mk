
# -----------------------------------------------------------------------
#       FILE: 	make/make_command_interface.mk
#	AUTHOR:	sebastian lesse
# -----------------------------------------------------------------------

COMMAND_MANAGEMENT_INC_PATH = $(APP_PATH)/command_management
INC_PATH += $(COMMAND_MANAGEMENT_INC_PATH)

# -----------------------------------------------------------------------

DEFS += -D COMMAND_CONTROLLER_AVAILABLE=1

CSRCS += $(COMMAND_MANAGEMENT_INC_PATH)/command_controller.c

# -----------------------------------------------------------------------

COMMAND_HANDLER_INC_PATH = $(APP_PATH)/command_handler
INC_PATH += $(COMMAND_HANDLER_INC_PATH)

ifneq '' '$(findstring RPI_CMD,$(COMMAND_INTERFACE_CFG))'

	DEFS += -D RPI_CMD_HANDLER_AVAILABLE=1

	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_command_handler.c
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_command_handler_table.c
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_system.c
endif

ifneq '' '$(findstring RPI_CMD_IO,$(COMMAND_INTERFACE_CFG))'

	DEFS += -D RPI_CMD_HANDLER_IO_AVAILABLE=1
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_io.c
endif

ifneq '' '$(findstring RPI_CMD_SENSOR,$(COMMAND_INTERFACE_CFG))'

	DEFS += -D RPI_CMD_HANDLER_SENSOR_AVAILABLE=1
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_sensor.c
endif

ifneq '' '$(findstring RPI_CMD_IR_REMOTE,$(COMMAND_INTERFACE_CFG))'

	DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_AVAILABLE=1
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_command_handler_ir_remote.c

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_SAMSUNG,$(COMMAND_INTERFACE_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_SAMSUNG_AVAILABLE=1
		CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_ir_remote_samsung.c
	endif

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_JVC,$(COMMAND_INTERFACE_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_JVC_AVAILABLE=1
		CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_ir_remote_jvc.c
	endif

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_SONY,$(COMMAND_INTERFACE_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_SONY_AVAILABLE=1
		CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_ir_remote_sony.c
	endif

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_LED_LIGHTS,$(COMMAND_INTERFACE_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_LED_LIGHTS_AVAILABLE=1
		CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_ir_remote_led_lights.c
	endif
endif

ifneq '' '$(findstring RPI_CMD_ROUTING,$(COMMAND_INTERFACE_CFG))'

	DEFS += -D RPI_CMD_HANDLER_ROUTING_AVAILABLE=1
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_handler_routing.c
endif

# -----------------------------------------------------------------------

