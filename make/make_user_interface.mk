
USER_INTERFACE_PATH = $(FRMWRK_PATH)/src/ui

# --------------------------------------------------------------------------------

CONSOLE_INTERFACE_PATH = $(USER_INTERFACE_PATH)/console

ifneq '' '$(findstring CONSOLE,$(USER_INTERFACE_CFG))'
	CSRCS += $(CONSOLE_INTERFACE_PATH)/ui_console.c
endif

# --------------------------------------------------------------------------------

LCD_INTERFACE_PATH = $(USER_INTERFACE_PATH)/lcd

ifneq '' '$(findstring LCD_16X2,$(USER_INTERFACE_CFG))'
	CSRCS += $(LCD_INTERFACE_PATH)/ui_lcd_16x2.c
endif

# --------------------------------------------------------------------------------

CFG_FILE_PARSER_PATH = $(USER_INTERFACE_PATH)/cfg_file_parser

ifneq '' '$(findstring CFG_FILE_PARSER,$(USER_INTERFACE_CFG))'
	DEFS += -D CFG_FILE_PARSER_AVAILABLE=1
	CSRCS += $(CFG_FILE_PARSER_PATH)/cfg_file_parser.c
endif

# --------------------------------------------------------------------------------

LOG_INTERFACE_PATH = $(USER_INTERFACE_PATH)/log_interface

ifneq '' '$(findstring LOG_INTERFACE,$(USER_INTERFACE_CFG))'
	DEFS += -D LOG_INTERFACE_AVAILABLE=1
	CSRCS += $(LOG_INTERFACE_PATH)/log_interface.c
endif

# --------------------------------------------------------------------------------

CFG_FILE_INTERFACE_PATH = $(USER_INTERFACE_PATH)/file_interface

ifneq '' '$(findstring FILE_INTERFACE,$(USER_INTERFACE_CFG))'
	CSRCS += $(CFG_FILE_INTERFACE_PATH)/file_interface.c
endif

# --------------------------------------------------------------------------------

COMMAND_LINE_INTERFACE_PATH = $(USER_INTERFACE_PATH)/command_line

ifneq '' '$(findstring CLI_ARG_DEVICE,$(USER_INTERFACE_CFG))'
	DEFS += -D CLI_DEVICE_ARGUMENT_AVAILABLE=1
	USER_INTERFACE_CFG += CLI_TABLE
	USER_INTERFACE_CFG += CLI_HANDLER
endif

ifneq '' '$(findstring CLI_ARG_LCD,$(USER_INTERFACE_CFG))'
	DEFS += -D CLI_LCD_ARGUMENT_AVAILABLE=1
	USER_INTERFACE_CFG += CLI_TABLE
	USER_INTERFACE_CFG += CLI_HANDLER
endif

ifneq '' '$(findstring CLI_ARG_COMMAND,$(USER_INTERFACE_CFG))'
	DEFS += -D CLI_COMMAND_ARGUMENT_AVAILABLE=1
	USER_INTERFACE_CFG += CLI_TABLE
	USER_INTERFACE_CFG += CLI_HANDLER
endif

# --------------------------------------------------------------------------------

ifneq '' '$(findstring CLI_TABLE,$(USER_INTERFACE_CFG))'
	DEFS += -D CLI_AVAILABLE=1
	CSRCS += $(COMMAND_LINE_INTERFACE_PATH)/command_line_argument_table.c
endif

ifneq '' '$(findstring CLI_HANDLER,$(USER_INTERFACE_CFG))'
	CSRCS += $(COMMAND_LINE_INTERFACE_PATH)/command_line_handler.c
endif

# --------------------------------------------------------------------------------