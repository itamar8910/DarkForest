

LIBC_INCLUDE := ../libc
DRIVERS_DIR := drivers
INCLUDE := -I$(LIBC_INCLUDE) -I$(DRIVERS_DIR)

KERN_OBJS := \
	kernel.o \
	boot.ao \
	VgaTTY.o \
	gdt.ao \
	cpu_descriptors.o \
	isr.ao \
	isr.o\

DRIV_OBJS := \
	$(DRIVERS_DIR)/VgaText.o \
	$(DRIVERS_DIR)/DebugPort.o

HEADERS := $(OBJS:.o=.h)

OBJS := $(KERN_OBJS) $(DRIV_OBJS)
LIBC_OBJ := ../libc/libc.o
 
# stubs for gcc to insert code for calling ctors of globals
CRTI_OBJ=init/crti.ao
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=init/crtn.ao

OBJ_TO_LINK := $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(LIBC_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ)


TARGET = myos.bin

all: $(TARGET)

$(TARGET): init $(OBJS)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJ_TO_LINK) -lgcc

init: $(CRTI_OBJ) $(CRTN_OBJ)

# has .ao suffix so we can know to use assembler instead of compiler
%.ao: %.S
	$(ASM) -felf32 $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(DRIVERS_DIR)/*.o *.ao init/*.ao