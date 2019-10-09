

OBJS := \
	string.o \
	printf.o \
	stdlib.o \
	bits.o \

HEADERS := $(OBJS:.o=.h)
INCLUDE = -I../kernel/

TARGET := libc.o

all: $(TARGET)


$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

