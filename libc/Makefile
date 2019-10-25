

LIBC_OBJS := \
	string.o \
	printf.o \
	stdlib.o \
	bits.o \

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
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

