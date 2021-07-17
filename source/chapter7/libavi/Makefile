CC = gcc
AR = ar
TARGET = libavi.a libavi.so.0.0 test_libavi
SOURCE = libavi.c libavi.h
LIB = -L. -lavi

.phony:
all: $(TARGET)

libavi.a:
	$(CC) -fPIC -g -c -o libavi.o libavi.c
	$(AR) rcs $@ libavi.o

libavi.so.0.0:
	$(CC) -fPIC -g -c -o libavi.o libavi.c
	$(CC) -shared -g -Wl,-soname,libavi.so.0 -o libavi.so.0.0 libavi.o

test_libavi: test_libavi.c libavi.so.0.0
	$(CC) -g -o $@ $< $(LIB)
	
clean:
	rm -f libavi.a libavi.so.* libavi.o test_libavi
