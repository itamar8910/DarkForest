
BUILD := build
INCLUDE_DIR := include
INCLUDE := -I$(INCLUDE_DIR)


all: $(BUILD)/Cstring.o
	$(LD) --relocatable $(BUILD)/Cstring.o -o $(BUILD)/libc.o	

$(BUILD)/Cstring.o: Cstring.cpp $(INCLUDE_DIR)/Cstring.h
	$(CC) $(INCLUDE) -c Cstring.cpp -o $(BUILD)/Cstring.o $(CFLAGS)

clean:
	rm -f $(BUILD)/*

