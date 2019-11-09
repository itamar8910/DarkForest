

LIBC_OBJS := \
	string.o \
	printf.o \
	stdlib.o \
	bits.o \
	HeapAllocator.o \
	asserts.o \
	runtime_misc.o \
	logging.o \

LIBC_USERSPACE_ONLY_OBJS := \
	unistd.o \
	stdio.o \

ifeq ($(LIBCMODE), USERSPACE)
LIBC_OBJS += $(LIBC_USERSPACE_ONLY_OBJS)
endif

TYPES_DIR := types

TYPES_OBJS := \
	$(TYPES_DIR)/String.o	

HEADERS := $(OBJS:.o=.h)
INCLUDE = -I../kernel/ -I.

TARGET := libc.o

OBJS := $(LIBC_OBJS) $(TYPES_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) -D$(LIBCMODE) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

