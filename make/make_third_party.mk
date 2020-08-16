
ifneq '' '$(findstring IR_PROTOCOL_SAMSUNG,$(THIRD_PARTY_MODULES))'
	DEFS += -D HAS_IR_PROTOCOL_SAMSUNG=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_samsung.c
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_samsung_commands.c
endif

ifneq '' '$(findstring IR_PROTOCOL_JVC,$(THIRD_PARTY_MODULES))'
	DEFS += -D HAS_IR_PROTOCOL_JVC=1
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_jvc.c
	CSRCS += $(APP_PATH)/3rdparty/ir_protocol/ir_protocol_jvc_commands.c
endif