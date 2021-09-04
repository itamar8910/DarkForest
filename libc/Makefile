

LIBC_OBJS := \
	cstring.o \
	printf.o \
	stdlib.o \
	bits.o \
	HeapAllocator.o \
	asserts.o \
	runtime_misc.o \
	logging.o \
	syscalls.o \
	mman.o \
	malloc.o \
	VgaTextCommon.o \
	PS2KeyboardCommon.o \
	Math.o \
	Geometry.o \
	errno.o \

LIBC_USERSPACE_ONLY_OBJS := \
	unistd.o \
	df_unistd.o \
	stdio.o \

ifeq ($(LIBCMODE), USERSPACE)
LIBC_OBJS += $(LIBC_USERSPACE_ONLY_OBJS)
endif

TYPES_DIR := types

TYPES_OBJS := \
	$(TYPES_DIR)/String.o	

FS_DIR := FileSystem

FS_OBJS := \
	$(FS_DIR)/path.o \
	$(FS_DIR)/DirectoryEntry.o	\

HEADERS := $(OBJS:.o=.h)
INCLUDE = -I. -I../

TARGET := libc.o

OBJS := $(LIBC_OBJS) $(TYPES_OBJS) $(FS_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) -D$(LIBCMODE) -DMODE -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(TYPES_DIR)/*.o $(FS_DIR)/*.o

