

OBJS := \
	Cstring.o

HEADERS := $(OBJS:.o=.h)

TARGET := libc.o

all: $(TARGET)


$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

