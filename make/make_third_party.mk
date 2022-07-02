
ifneq '' '$(findstring IR_PROTOCOL_SAMSUNG,$(THIRD_PARTY_MODULES))'
	THIRD_PARTY_MODULES += IR_PROTOCOL_TRANSMIT_BUFFER
	DEFS += -D HAS_IR_PROTOCOL_SAMSUNG=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_samsung.c
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_samsung_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_JVC,$(THIRD_PARTY_MODULES))'
	THIRD_PARTY_MODULES += IR_PROTOCOL_TRANSMIT_BUFFER
	DEFS += -D HAS_IR_PROTOCOL_JVC=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_jvc.c
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_jvc_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_SONY,$(THIRD_PARTY_MODULES))'
	THIRD_PARTY_MODULES += IR_PROTOCOL_TRANSMIT_BUFFER
	DEFS += -D HAS_IR_PROTOCOL_SONY=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_sony.c
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_sony_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_NEC,$(THIRD_PARTY_MODULES))'
	THIRD_PARTY_MODULES += IR_PROTOCOL_TRANSMIT_BUFFER
	DEFS += -D HAS_IR_PROTOCOL_NEC=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_nec.c
endif

ifneq '' '$(findstring IR_COMMAND_LED_LIGHT,$(THIRD_PARTY_MODULES))'
	DEFS += -D HAS_IR_COMMAND_LED_LIGHT=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_command_led_lights.c
endif

ifneq '' '$(findstring IR_PROTOCOL_TRANSMIT_BUFFER,$(THIRD_PARTY_MODULES))'
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_transmit_buffer.c
endif
