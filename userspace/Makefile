
OBJS := \
	start.ao \
	main.o \

LIBC_DIR := ../libc
LIBC_OBJ := $(LIBC_DIR)/libc.o

OBJS_TO_LINK := $(OBJS) $(LIBC_OBJ)

INCLUDE := -I$(LIBC_DIR)

all: libc main

libc:
	make -C $(LIBC_DIR) clean
	make -C $(LIBC_DIR) LIBCMODE=USERSPACE

main: main.o start.ao
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJS_TO_LINK) -lgcc

main.o: main.cpp start.S
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

%.ao: %.S
	$(ASM) -felf32 $< -o $@

clean:
	rm -f *.o *.ao