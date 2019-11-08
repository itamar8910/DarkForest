
OBJS := main.o \

all: $(OBJS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
	$(CC) -T linker.ld -o $* $(LDFLAGS) $@ -lgcc
