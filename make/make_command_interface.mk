
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
	CSRCS += $(COMMAND_HANDLER_INC_PATH)/rpi_cmd_ir_remote.c
endif

# -----------------------------------------------------------------------

