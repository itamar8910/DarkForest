
TARGET := LibGui.o

WIDGETS_DIR := Widgets

OBJS := \
	Window.o \
	GuiManager.o \
	$(WIDGETS_DIR)/Widget.o \
	$(WIDGETS_DIR)/TextView.o \

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) --relocatable $^ -o $@

%.o: %.cpp
	$(CC) $(INCLUDE) -DUSERSPACE -DMODE -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(WIDGETS_DIR)/*.o
