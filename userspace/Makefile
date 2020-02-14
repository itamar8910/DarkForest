
OBJS := \
	start.ao \

APPS := \
	HelloWorld.app \
	shell.app \
	terminal.app \
	cat.app \
	ls.app \
	echo.app \
	touch.app \
	mkdir.app \
	WindowServer.app \
	gui.app \

CURRENT_DIR := $(realpath ./)
GLOBAL_DIR := $(realpath ../)
LIBC_DIR := $(realpath ../libc)
LIBC_OBJ := $(LIBC_DIR)/libc.o

OBJS_TO_LINK := $(OBJS) $(LIBC_OBJ)

export INCLUDE := -I$(LIBC_DIR) -I$(CURRENT_DIR) -I$(GLOBAL_DIR)

all: libc start.ao apps

libc:
	make -C $(LIBC_DIR) clean
	make -C $(LIBC_DIR) LIBCMODE=USERSPACE

apps: $(APPS)

%.app: FORCE
	make -C $*
	$(CC) -T linker.ld -o $*.app $(LDFLAGS) $(OBJS_TO_LINK) $*/$*.o -lgcc

FORCE:

%.ao: %.S
	$(ASM) -felf32 $< -o $@

CLEAN_SUBDIRS := $(wildcard */.)

.PHONY: $(CLEAN_SUBDIRS)

clean: $(CLEAN_SUBDIRS)
	rm -f *.o *.ao *.app

$(CLEAN_SUBDIRS):
	$(MAKE) -C $@ clean