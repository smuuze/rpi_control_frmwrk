# -----------------------------------------------------------------------
# CORE: Shared-Memory
# -----------------------------------------------------------------------

SHARED_MEMORY_PATH = $(APP_PATH)/core/shared_memory

DEFS += -D SHARED_MEMORY_AVAILABLE=1
CSRCS += $(SHARED_MEMORY_PATH)/shared_memory.c
INC_PATH += $(SHARED_MEMORY_PATH)

# -----------------------------------------------------------------------
