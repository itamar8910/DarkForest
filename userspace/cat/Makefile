
LIBC_DIR := ../../libc
INCLUDE += -I$(LIBC_DIR)

TARGET := cat.o

OBJS := \
	main.o \

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp
	$(CC) $(INCLUDE) -DUSERSPACE -DMODE -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o