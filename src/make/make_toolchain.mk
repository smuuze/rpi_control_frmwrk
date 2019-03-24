
CROSS_COMPILER_PREFIX = avr
CROSS_COMPILER_PATH   = /usr/bin

CC      = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)-gcc
CC_SIZE = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)-size
CC_OBJ  = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)-objcopy
CC_LD   = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)-ld

RM	 = rm
RM_FORCE = rm -rf
CP       = cp
MV       = mv