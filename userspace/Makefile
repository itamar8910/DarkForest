
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
	gui2.app \

CURRENT_DIR := $(realpath ./)
GLOBAL_DIR := $(realpath ../)
LIBC_DIR := $(realpath ../libc)
LIBGUI_DIR := $(realpath LibGui/)
LIBC_OBJ := $(LIBC_DIR)/libc.o
LIBGUI_OBJ := $(LIBGUI_DIR)/LibGui.o

OBJS_TO_LINK := $(OBJS) $(LIBC_OBJ) $(LIBGUI_OBJ)

export INCLUDE := -I$(LIBC_DIR) -I$(CURRENT_DIR) -I$(GLOBAL_DIR)

all: libc libgui start.ao apps

libc:
	make -C $(LIBC_DIR) clean
	make -C $(LIBC_DIR) LIBCMODE=USERSPACE

libgui:
	make -C $(LIBGUI_DIR)

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