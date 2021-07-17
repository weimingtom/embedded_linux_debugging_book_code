CC = arm-linux-gcc
TARGET := gui
HEADER := 
SRCS := gui.c
CFLAGS += 	-g -I/home/cyc/minigui/libminigui-1.3.3/arm-build/include \
			-L/home/cyc/minigui/libminigui-1.3.3/arm-build/lib \
			-L/home/cyc/tslib/arm-linux-build/lib/ \
			-lmgext -lminigui -lts

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf ./*.o $(TARGET)
