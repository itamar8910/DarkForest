
LIBC_DIR := ../../libc
INCLUDE := -I$(LIBC_DIR)

all: main.o

main.o: main.cpp
	$(CC) $(INCLUDE) -DUSERSPACE -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o