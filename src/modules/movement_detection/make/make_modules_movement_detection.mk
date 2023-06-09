# -----------------------------------------------------------------------
# MOVEMENT-DETECTION MODULE
# -----------------------------------------------------------------------

MODULE_MOVEMENT_DETECTION_PATH = $(APP_PATH)/modules/movement_detection

ifneq '' '$(findstring MOVEMENT_DETECTION,$(MODULES_CFG))'

	DEFS += -D MOVEMENT_DETECTION_CONTROLLER_AVAILABLE=1
	CSRCS += $(MODULE_MOVEMENT_DETECTION_PATH)/movement_detection_controller.c

	ifneq '' '$(findstring MOVEMENT_DETECTION_MQTT,$(MODULES_CFG))'
		DEFS += -D MOVEMENT_DETECTION_MQTT_AVAILABLE=1
	endif
endif

# -----------------------------------------------------------------------
