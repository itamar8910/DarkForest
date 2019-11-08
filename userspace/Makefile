
OBJS := \
	start.ao \
	main.o \


all: main

main: main.o start.ao
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $(OBJS) -lgcc

main.o: main.cpp start.S
	$(CC) -c $< -o $@ $(CFLAGS)

%.ao: %.S
	$(ASM) -felf32 $< -o $@

clean:
	rm -f *.o *.ao