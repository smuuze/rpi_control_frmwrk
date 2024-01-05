# -----------------------------------------------------------------------
# CFG-FILE-PARSER MODULE
# -----------------------------------------------------------------------

CFG_FILE_PARSER_PATH = $(APP_PATH)/modules/cfg_file_parser

ifneq '' '$(findstring CFG_FILE_PARSER,$(USER_INTERFACE_CFG))'
	DEFS += -D CFG_FILE_PARSER_AVAILABLE=1
	CSRCS += $(CFG_FILE_PARSER_PATH)/cfg_file_parser.c

	INC_PATH += $(CFG_FILE_PARSER_PATH)
endif

# -----------------------------------------------------------------------
