
OBJS := \
	start.ao \

APPS := \
	HelloWorld.app \
	shell.app \
	TextTerminal.app \
	cat.app \
	ls.app \
	echo.app \
	touch.app \
	mkdir.app \
	WindowServer.app \
	GuiTerminal.app \
	gui2.app \

CURRENT_DIR := $(realpath ./)
GLOBAL_DIR := $(realpath ../)
LIBC_DIR := $(realpath ../libc)
LIBGUI_DIR := $(realpath LibGui/)
LIB_WINDOW_SERVER_DIR := $(realpath LibWindowServer/)

LIBC_OBJ := $(LIBC_DIR)/libc.o
LIBGUI_OBJ := $(LIBGUI_DIR)/LibGui.o
LIB_WINDOW_SERVER_OBJ := $(LIB_WINDOW_SERVER_DIR)/LibWindowServer.o

OBJS_TO_LINK := $(OBJS) $(LIBC_OBJ) $(LIB_WINDOW_SERVER_OBJ) $(LIBGUI_OBJ)

export INCLUDE := -I$(LIBC_DIR) -I$(CURRENT_DIR) -I$(GLOBAL_DIR)

all: libc libgui  lib_windowserver start.ao apps

libc:
	make -C $(LIBC_DIR) clean
	make -C $(LIBC_DIR) LIBCMODE=USERSPACE

libgui:
	make -C $(LIBGUI_DIR)

lib_windowserver:
	make -C $(LIB_WINDOW_SERVER_DIR)

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