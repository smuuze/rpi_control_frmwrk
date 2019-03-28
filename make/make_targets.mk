
# --------- Message Output

MSG_COMPILING		:= Compiling
MSG_LINKING		:= Linking to
MSG_PROG_LOCATION	:= Your programm can be found at
MSG_LISTING		:= - Generating Disassembly
MSG_FINISH		:= --------------- Make done ---------------

# --------- 

TARGET			:= $(PROJECT)
VERSION			:= $(VERSION_MAJOR).$(VERSION_MINOR)
OBJECT_DIRECTORY	:= obj

# --------- 

all: obj_dir release_obj lss_file hex_file
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).o $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)
	
debug:


release:

clean:
	$(VERBOSE) $(ECHO) - Removing object directory from filesystem
	$(VERBOSE) $(RM) $(RM_FLAGS) $(OBJECT_DIRECTORY)
	$(VERBOSE) $(ECHO) - Removing generated program-files
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).hex
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).lss
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

release_obj:
	$(VERBOSE) $(ECHO) - Generating Relase-Object - Version: $(VERSION)
	$(VERBOSE) $(CC) $(DEFS) $(CFLAGS) $(LIBS) $(INC_PATH:%=-I%) $(CSRCS) -o $(OBJECT_DIRECTORY)/$(TARGET).o

debug_obj:
	$(VERBOSE) $(ECHO) - Generating Debug-Object - Version: $(VERSION)
	$(VERBOSE) $(CC) $(DEFS) $(DEBUG_ENABLED) $(CFLAGS) $(LIBS) $(INC_PATH:%=-I%) $(CSRCS) -o $(OBJECT_DIRECTORY)/$(TARGET).o

hex_file: elf_file
	$(VERBOSE) $(ECHO) - Generating $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_COPY) -O $(FORMAT) $(OBJECT_DIRECTORY)/$(TARGET).o $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_SIZE) $(OBJECT_DIRECTORY)/$(TARGET).o 
	
elf_file:
	$(VERBOSE) $(ECHO) - Generating $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(CC) $(LDFLAGS) $(MCU) $(OBJECTS) -o $(TARGET).elf
	
lss_file:
	$(VERBOSE) $(ECHO) $(MSG_LISTING)
	$(VERBOSE) $(CC_DUMP) -h -S $(OBJECT_DIRECTORY)/$(TARGET).o > $(OBJECT_DIRECTORY)/$(TARGET).lss

obj_dir:
	$(VERBOSE) $(ECHO) - Creating Object directory: $(OBJECT_DIRECTORY)
	$(VERBOSE) $(MK) $(OBJECT_DIRECTORY)

release_dir:
	$(VERBOSE) $(ECHO) - Creating Release directory: $(RELEASE_DIRECTORY)
	$(VERBOSE) $(MK) $(RELEASE_DIRECTORY)

# --------- 

hex: $(TARGET).hex

eeprom: $(TARGET)_eeprom.hex

$(TARGET).hex: $(TARGET).elf
	$(CC_OBJ) -O ihex -j .data -j .text $(TARGET).elf $(TARGET).hex

$(TARGET)_eeprom.hex: $(TARGET).elf
	$(CC_OBJ) -O ihex -j .eeprom --change-section-lma .eeprom=1 $(TARGET).elf $(TARGET)_eeprom.hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(TARGET).elf

.c.o:
	$(CC) $(CFLAGS) -mmcu=$(MCU) $< -o $@

size:
	$(CC_SIZE) --mcu=$(MCU) -C $(TARGET).elf

program:
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:w:$(TARGET).hex:a

clean_tmp:
	$(RM) $(RM_FLAGS) *.o
	$(RM) $(RM_FLAGS *.elf