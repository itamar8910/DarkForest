

LIBC_INCLUDE := ../libc

KERN_OBJS := \
	_init.o \
	boot.ao \
	cpu.o \
	kmalloc.o \
	task.o \
	task.ao \
	Scheduler.o \
	TaskBlocker.o \
	sleep.o \
	syscall.o \
	process.o \
	kernel_symbols.o \
	backtrace.o \
	lock.o \
	BigBuffer.o \
	Pipe.o \
	

DRIVERS_DIR := drivers
DRIV_OBJS := \
	$(DRIVERS_DIR)/VgaText.o \
	$(DRIVERS_DIR)/DebugPort.o \
	$(DRIVERS_DIR)/PIC.o \
	$(DRIVERS_DIR)/PIT.o \
	$(DRIVERS_DIR)/PS2Keyboard.o \
	$(DRIVERS_DIR)/ATADisk.o \
	$(DRIVERS_DIR)/PS2Mouse.o \
	$(DRIVERS_DIR)/PCIBus.o \
	$(DRIVERS_DIR)/RTL8139NetworkCard.o \

MM_DIR := MM
MM_OBJS := \
	$(MM_DIR)/MemoryManager.o \
	$(MM_DIR)/PageDirectory.o \
	$(MM_DIR)/PageTable.o \
	$(MM_DIR)/SharedMemoryManager.o \

FS_DIR := FileSystem
FS_OBJS := \
	$(FS_DIR)/VFS.o \
	$(FS_DIR)/DevFS.o \
	$(FS_DIR)/CharFile.o \
	$(FS_DIR)/FileUtils.o \
	$(FS_DIR)/Fat32FS.o \
	$(FS_DIR)/PtsFS.o \

LOADER_DIR := Loader
LOADER_OBJS := \
	$(LOADER_DIR)/loader.o \
	$(LOADER_DIR)/ElfParser.o \

HAL_DIR := HAL
HAL_OBJS := \
	$(HAL_DIR)/KeyboardDevice.o \
	$(HAL_DIR)/VgaTextDevice.o \
	$(HAL_DIR)/VgaTTY.o \
	$(HAL_DIR)/AsciiKeyboardDevice.o \
	$(HAL_DIR)/VgaDevice.o \
	$(HAL_DIR)/MouseDevice.o \

NETWORK_DIR := Network
NETWORK_OBJS := \
	$(NETWORK_DIR)/Arp.o \
	$(NETWORK_DIR)/Ethernet.o \
	$(NETWORK_DIR)/Icmp.o \
	$(NETWORK_DIR)/IcmpSocket.o \
	$(NETWORK_DIR)/IpV4.o \
	$(NETWORK_DIR)/NetworkManager.o \

INCLUDE := -I. -I.. -I$(LIBC_INCLUDE) -Imode/


OBJS := $(KERN_OBJS) $(DRIV_OBJS) $(MM_OBJS) $(FS_OBJS) $(LOADER_OBJS) $(HAL_OBJS) $(NETWORK_OBJS)

HEADERS := $(shell find . -name '*.h')

LIBC_OBJ := ../libc/libc.o
 
# stubs for gcc to insert code for calling ctors of globals
CRTI_OBJ=init/crti.ao
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=init/crtn.ao

OBJ_TO_LINK := $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(LIBC_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ)

SYMBOLS_FILE := ../init_files/kernel_symbols.txt
LINEINFO_FILE := ../init_files/kernel_lineinfo.txt
SYMBOL_GENERATOR_SCRIPT := ../toolchain/symbol_generator.py

TARGET := DarkForest.bin

all: libc $(TARGET)

libc:
	make -C $(LIBC_INCLUDE) clean
	make -C $(LIBC_INCLUDE) LIBCMODE=KERNEL

$(TARGET): init $(OBJ_TO_LINK)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJ_TO_LINK) -lgcc
	# generate symbol map & line numbers files
	$(SYMBOL_GENERATOR_SCRIPT) $(TARGET) --sym $(SYMBOLS_FILE) --line $(LINEINFO_FILE)

init: $(CRTI_OBJ) $(CRTN_OBJ)

# has .ao suffix so we can know to use assembler instead of compiler
%.ao: %.S
	$(ASM) -felf32 $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) $(CPP_FLAGS) -DKERNEL -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(DRIVERS_DIR)/*.o $(MM_DIR)/*.o *.ao init/*.ao $(FS_DIR)/*.o $(LOADER_DIR)/*.o $(HAL_DIR)/*.o $(NETWORK_DIR)/*.o $(TARGET)