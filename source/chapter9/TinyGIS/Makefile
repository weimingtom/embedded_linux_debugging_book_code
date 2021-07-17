CC = arm-linux-gcc
TARGET := TinyGIS
HEADER := 	-I. -I/home/cyc/minigui/libminigui-1.3.3/arm-build/include
LIBRARY := 	-L/home/cyc/minigui/libminigui-1.3.3/arm-build/lib \
			-L/home/cyc/minigui/tslib/arm-linux-build/lib/ \
			-lmgext -lminigui -lts -lpthread -lm
SRCS := gui.c gps.c
CFLAGS += 	-g -DPXA_XSCALE $(HEADER) $(LIBRARY)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $@ $(SRCS) $(CFLAGS)

clean:
	rm -rf ./*.o $(TARGET)
