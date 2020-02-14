
TARGET := WindowServer.o

OBJS := \
	main.o \
	Vga.o \
	WindowServerHandler.o \
	Window.o \

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp
	$(CC) $(INCLUDE) -DUSERSPACE -DMODE -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o
