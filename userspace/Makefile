
OBJS := \
	start.ao \

# NOTE: remember to add line in make clean target (or make it automatic)
APPS := \
	HelloWorld.app \
	shell.app \
	terminal.app \

LIBC_DIR := ../libc
LIBC_OBJ := $(LIBC_DIR)/libc.o

OBJS_TO_LINK := $(OBJS) $(LIBC_OBJ)

export INCLUDE := -I$(LIBC_DIR) -I../../

all: libc start.ao apps

libc:
	make -C $(LIBC_DIR) clean
	make -C $(LIBC_DIR) LIBCMODE=USERSPACE

apps: $(APPS)

%.app: FORCE
	make -C $*
	$(CC) -T linker.ld -o $*.app $(LDFLAGS) $(OBJS_TO_LINK) $*/$*.o -lgcc

FORCE:

# main: main.o start.ao
# 	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJS_TO_LINK) -lgcc

# main.o: main.cpp start.S
# 	$(CC) $(INCLUDE) -DUSERSPACE -c $< -o $@ $(CFLAGS)

%.ao: %.S
	$(ASM) -felf32 $< -o $@

clean:
	make -C shell/ clean
	make -C HelloWorld/ clean
	make -C terminal/ clean
	rm -f *.o *.ao *.app