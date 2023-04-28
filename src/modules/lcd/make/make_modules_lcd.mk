# -----------------------------------------------------------------------
# LCD MODULE
# -----------------------------------------------------------------------

MODULE_LCD_PATH = $(APP_PATH)/modules/lcd

ifneq '' '$(findstring LCD_16X2,$(MODULES_CFG))'
	DEFS += -D LCD_CONTROLLER_AVAILABLE=1
	CSRCS += $(MODULE_LCD_PATH)/lcd_controller_16x2.c

	DRIVER_MODULE_CFG += LCD_16X2
endif

# -----------------------------------------------------------------------
