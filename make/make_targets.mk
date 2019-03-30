
# --------- Message Output

MSG_COMPILING		:= Compiling
MSG_DEPENDENCY		:= Generating dependency for 
MSG_LINKING		:= Linking to
MSG_PROG_LOCATION	:= Your programm can be found at
MSG_LISTING		:= - Generating Disassembly
MSG_FINISH		:= --------------- Make done ---------------

# --------- 

TARGET			:= $(PROJECT)
VERSION			:= $(VERSION_MAJOR).$(VERSION_MINOR)
OBJECT_DIRECTORY	:= obj
RELEASE_DIRECTORY	:= release/$(VERSION)
FORMAT			:= ihex
DEBUG_ENABLED		:= -DTRACER_ENABLED

OBJECTS			:= $(CSRCS:.c=.o)
DEBUG_OBJECTS		:= $(CSRCS:.c=.a)
DEPENDENCY_OBJECTS	:= $(CSRCS:.c=.f)

LOCAL_OBJECTS		:= $(notdir $(OBJECTS))
LOCAL_DEBUG_OBJECTS	:= $(notdir $(DEBUG_OBJECTS))

# --------- 

all: release_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(TARGET).elf
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET).elf
	$(VERBOSE) $(ECHO) $(MSG_FINISH)
	
debug: debug_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(TARGET).elf
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET).elf
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

release: release_dir release_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(RELEASE_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(RELEASE_DIRECTORY)/$(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(RELEASE_DIRECTORY)/$(TARGET).elf
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(RELEASE_DIRECTORY)/$(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

eclipse: obj_dir $(DEPENDENCY_OBJECTS)

clean:
	$(VERBOSE) $(ECHO) - Removing object directory from filesystem
	$(VERBOSE) $(RM) $(RM_FLAGS) $(OBJECT_DIRECTORY)
	$(VERBOSE) $(ECHO) - Removing generated program-files
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).hex
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).lss
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).elf
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

release_obj: obj_dir $(OBJECTS)
	$(VERBOSE) $(ECHO) - Generating Relase-Object - Version: $(VERSION)
	$(VERBOSE) $(CC) $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $(LOCAL_OBJECTS:%=$(OBJECT_DIRECTORY)/%) -o $(OBJECT_DIRECTORY)/$(TARGET).elf

debug_obj: obj_dir $(DEBUG_OBJECTS)
	$(VERBOSE) $(ECHO) - Generating Debug-Object - Version: $(VERSION)
	$(VERBOSE) $(CC) $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $(DEBUG_ENABLED) $(LOCAL_DEBUG_OBJECTS:%=$(OBJECT_DIRECTORY)/%) -o $(OBJECT_DIRECTORY)/$(TARGET).elf

hex_file:
	$(VERBOSE) $(ECHO) - Generating $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_COPY) -O $(FORMAT) $(OBJECT_DIRECTORY)/$(TARGET).elf $(OBJECT_DIRECTORY)/$(TARGET).hex
	
lss_file:
	$(VERBOSE) $(ECHO) $(MSG_LISTING)
	$(VERBOSE) $(CC_DUMP) -h -S $(OBJECT_DIRECTORY)/$(TARGET).elf > $(OBJECT_DIRECTORY)/$(TARGET).lss

obj_dir:
	$(VERBOSE) $(ECHO) - Creating Object directory: $(OBJECT_DIRECTORY)
	$(VERBOSE) $(MK) $(OBJECT_DIRECTORY)

release_dir:
	$(VERBOSE) $(ECHO) - Creating Release directory: $(RELEASE_DIRECTORY)
	$(VERBOSE) $(MK) $(RELEASE_DIRECTORY)

# --------- 

prog_size:
	$(CC_SIZE) --mcu=$(MCU_NAME) $(OBJECT_DIRECTORY)/$(TARGET).elf

# --------- 

tracer: 
	#$(TRACER_BIN_PATH)/$(TRACER_EXE_FILE) $(TRACER_WORKING_PATH)
	$(TRACER_BIN_PATH)/$(TRACER_EXE_FILE)

# --------- 


eeprom: $(TARGET)_eeprom.hex

$(TARGET)_eeprom.hex:
	$(CC_OBJ) -O ihex -j .eeprom --change-section-lma .eeprom=1 $(TARGET).elf $(TARGET)_eeprom.hex

.c.o:
	$(VERBOSE) $(ECHO) $(MSG_COMPILING) $(notdir $<)
	$(VERBOSE) $(CC) -c $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $< -o $(OBJECT_DIRECTORY)/$(notdir $@)

.c.a:
	$(VERBOSE) $(ECHO) $(MSG_COMPILING) $(notdir $<)
	$(VERBOSE) $(CC) -c $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(DEBUG_ENABLED) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $< -o $(OBJECT_DIRECTORY)/$(notdir $@)
	
.c.f:
	$(VERBOSE) $(ECHO) $(MSG_DEPENDENCY) $(notdir $<)
	$(VERBOSE) $(CC) -MM -c $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(DEBUG_ENABLED) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $< -o $(OBJECT_DIRECTORY)/$(notdir $@)
