

LIBC_INCLUDE := ../libc

KERN_OBJS := \
	_init.o \
	boot.ao \
	VgaTTY.o \
	cpu.o \
	logging.o \
	Kassert.o \
	kmalloc.o \
	task.o \
	task.ao \
	Scheduler.o \
	TaskBlocker.o \
	sleep.o \
	KeyboardReader.o \
	syscall.o \
	

DRIVERS_DIR := drivers
DRIV_OBJS := \
	$(DRIVERS_DIR)/VgaText.o \
	$(DRIVERS_DIR)/DebugPort.o \
	$(DRIVERS_DIR)/PIC.o \
	$(DRIVERS_DIR)/PIT.o \
	$(DRIVERS_DIR)/PS2Keyboard.o \

MM_DIR := MM
MM_OBJS := \
	$(MM_DIR)/MemoryManager.o \
	$(MM_DIR)/PageDirectory.o \
	$(MM_DIR)/PageTable.o \

FS_DIR := FileSystem
FS_OBJS := \
	$(FS_DIR)/TarFS.o \
	$(FS_DIR)/RamDisk.o \

LOADER_DIR := Loader
LOADER_OBJS := \
	$(LOADER_DIR)/loader.o \
	$(LOADER_DIR)/ElfParser.o \

INCLUDE := -I. -I$(LIBC_INCLUDE) -I$(DRIVERS_DIR) -I$(MM_DIR)

OBJS := $(KERN_OBJS) $(DRIV_OBJS) $(MM_OBJS) $(FS_OBJS) $(LOADER_OBJS)

HEADERS := $(shell find . -name '*.h')

LIBC_OBJ := ../libc/libc.o
 
# stubs for gcc to insert code for calling ctors of globals
CRTI_OBJ=init/crti.ao
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=init/crtn.ao

OBJ_TO_LINK := $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(LIBC_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ)

TARGET := DarkForest.bin

all: $(TARGET)

$(TARGET): init $(OBJ_TO_LINK)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJ_TO_LINK) -lgcc

init: $(CRTI_OBJ) $(CRTN_OBJ)

# has .ao suffix so we can know to use assembler instead of compiler
%.ao: %.S
	$(ASM) -felf32 $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) $(CPP_FLAGS) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(DRIVERS_DIR)/*.o $(MM_DIR)/*.o *.ao init/*.ao $(TARGET)