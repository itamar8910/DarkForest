

LIBC_INCLUDE := ../libc
DRIVERS_DIR := drivers
INCLUDE := -I$(LIBC_INCLUDE) -I$(DRIVERS_DIR)

KERN_OBJS := \
	kernel.o \
	VgaTTY.o

DRIV_OBJS := \
	$(DRIVERS_DIR)/VgaText.o

HEADERS := $(OBJS:.o=.h)

OBJS := $(KERN_OBJS) $(DRIV_OBJS) boot.ao

LIBC_OBJ := ../libc/libc.o

TARGET = myos.bin

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $^ $(LIBC_OBJ) -lgcc

# has .ao suffix so we can know to assemble it (instead of compiling)
boot.ao: boot.S
	$(ASM) -felf32 $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(DRIVERS_DIR)/*.o *.ao