MCU_NAME 	= universal
CPU_FAMILY 	= universal
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/universal
INC_PATH	+= /usr/include
DEFS 		+= -D__universal__

#CFLAGS += -pthread
