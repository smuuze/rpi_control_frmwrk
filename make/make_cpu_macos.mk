MCU_NAME 	= macos
CPU_FAMILY 	= universal
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/universal
INC_PATH	+= /usr/include
DEFS 		+= -D__macos__

#CFLAGS += -pthread
