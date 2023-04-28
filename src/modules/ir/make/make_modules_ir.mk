# -----------------------------------------------------------------------
# INFRARED PROTOCOL MODULES
# -----------------------------------------------------------------------

MODULE_IR_PATH     = $(APP_PATH)/modules/ir
MODULE_IR_CFG      =
MODULES_IR_CMD_CFG =

# -----------------------------------------------------------------------

ifneq '' '$(findstring IR_COMMAND_LED_LIGHT,$(MODULES_CFG))'

	ifeq '' '$(findstring IR_PROTOCOL_NEC,$(MODULES_CFG))'
		MODULES_CFG += IR_PROTOCOL_NEC
	endif

	MODULES_IR_CMD_CFG += RPI_CMD_IR_REMOTE_LED_LIGHTS

	DEFS += -D HAS_IR_COMMAND_LED_LIGHT=1
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_led_lights_commands.c
endif

# -----------------------------------------------------------------------

ifneq '' '$(findstring IR_PROTOCOL_SAMSUNG,$(MODULES_CFG))'

	MODULE_IR_CFG += IR_TRANSMIT_BUFFER
	MODULE_IR_CFG += IR_REMOTE_TASK
	MODULES_IR_CMD_CFG += RPI_CMD_IR_REMOTE_SAMSUNG

	DEFS += -D HAS_IR_PROTOCOL_SAMSUNG=1
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_samsung.c
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_samsung_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_JVC,$(MODULES_CFG))'

	MODULE_IR_CFG += IR_TRANSMIT_BUFFER
	MODULE_IR_CFG += IR_REMOTE_TASK
	MODULES_IR_CMD_CFG += RPI_CMD_IR_REMOTE_JVC
	
	DEFS += -D HAS_IR_PROTOCOL_JVC=1
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_jvc.c
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_jvc_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_SONY,$(MODULES_CFG))'

	MODULE_IR_CFG += IR_TRANSMIT_BUFFER
	MODULE_IR_CFG += IR_REMOTE_TASK
	MODULES_IR_CMD_CFG += RPI_CMD_IR_REMOTE_SONY
	
	DEFS += -D HAS_IR_PROTOCOL_SONY=1
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_sony.c
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_sony_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_NEC,$(MODULES_CFG))'

	MODULE_IR_CFG += IR_TRANSMIT_BUFFER
	MODULE_IR_CFG += IR_REMOTE_TASK
	
	DEFS += -D HAS_IR_PROTOCOL_NEC=1
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_nec.c
endif

# -----------------------------------------------------------------------

ifneq '' '$(findstring IR_REMOTE_TASK,$(MODULE_IR_CFG))'
DEFS += -D IR_PROTOCOL_AVAILABLE=1
CSRCS += $(MODULE_IR_PATH)/ir_protocol_task.c
endif

# -----------------------------------------------------------------------

ifneq '' '$(findstring IR_TRANSMIT_BUFFER,$(MODULE_IR_CFG))'
	CSRCS += $(MODULE_IR_PATH)//ir_protocol_transmit_buffer.c
endif

# -----------------------------------------------------------------------

ifneq '' '$(findstring RPI_CMD_IR_REMOTE,$(MODULES_IR_CMD_CFG))'

	DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_AVAILABLE=1
	CSRCS += $(MODULE_IR_PATH)/ir_protocol_rpi_cmd_handler.c

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_SAMSUNG,$(MODULES_IR_CMD_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_SAMSUNG_AVAILABLE=1
		CSRCS += $(MODULE_IR_PATH)/ir_protocol_samsung_rpi_cmd_handler.c
	endif

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_JVC,$(MODULES_IR_CMD_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_JVC_AVAILABLE=1
		CSRCS += $(MODULE_IR_PATH)/ir_protocol_jvc_rpi_cmd_handler.c
	endif

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_SONY,$(MODULES_IR_CMD_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_SONY_AVAILABLE=1
		CSRCS += $(MODULE_IR_PATH)/ir_protocol_sony_rpi_cmd_handler.c
	endif

	ifneq '' '$(findstring RPI_CMD_IR_REMOTE_LED_LIGHTS,$(MODULES_IR_CMD_CFG))'
		DEFS += -D RPI_CMD_HANDLER_IR_REMOTE_LED_LIGHTS_AVAILABLE=1
		CSRCS += $(MODULE_IR_PATH)/ir_protocol_led_lights_rpi_cmd_handler.c
	endif
endif

# -----------------------------------------------------------------------