# $Id: Makefile 9 2008-02-17 08:09:01Z rockins $

TOPDIR = $(shell pwd);
CC = gcc
LD = ld
MAKEDEP = gccmakedep
CFLAGS += -Wall 
CFLAGS += -g3
LIBS = -L./libavi -lavi -lxvidcore -lSDL -lpthread
INCS = -I./Include -I./libavi -I/usr/include/SDL
CFLAGS += $(INCS)
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
TARGET = ccplayer

.phony: all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

depend:
	$(MAKEDEP) -- $(CFLAGS) -- $(SRC)
	
clean:
	rm -f ./*.o $(TARGET)

# DO NOT DELETE
core.o: core.c /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
  /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/alloca.h /usr/include/assert.h libavi/libavi.h \
  Include/common.h Include/decode.h /usr/include/pthread.h \
  /usr/include/sched.h /usr/include/bits/sched.h /usr/include/signal.h \
  /usr/include/bits/setjmp.h Include/common.h Include/event.h \
  libavi/libavi.h Include/pingpong_buf_mgmt.h /usr/include/string.h \
  Include/output.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h \
  /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h \
  /usr/include/SDL/SDL_config-i386.h /usr/include/SDL/SDL_platform.h \
  /usr/include/strings.h /usr/include/inttypes.h /usr/include/stdint.h \
  /usr/include/ctype.h /usr/include/iconv.h /usr/include/SDL/begin_code.h \
  /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h \
  /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h \
  /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h \
  /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h \
  /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h \
  /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h \
  /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h \
  /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h \
  /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h \
  /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h \
  Include/event.h Include/pingpong_buf_mgmt.h
decode.o: decode.c /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
  /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/alloca.h /usr/include/string.h /usr/include/assert.h \
  Include/common.h /usr/local/include/xvid.h Include/decode.h \
  /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h \
  /usr/include/signal.h /usr/include/bits/setjmp.h Include/common.h \
  Include/event.h libavi/libavi.h Include/pingpong_buf_mgmt.h
event.o: event.c /usr/include/stdlib.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/typesizes.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
  /usr/include/assert.h Include/event.h /usr/include/pthread.h \
  /usr/include/sched.h /usr/include/bits/sched.h /usr/include/signal.h \
  /usr/include/bits/setjmp.h Include/common.h
output.o: output.c /usr/include/stdlib.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/typesizes.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
  /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h \
  /usr/include/signal.h /usr/include/bits/setjmp.h \
  /usr/local/include/xvid.h /usr/include/assert.h Include/common.h \
  Include/event.h Include/common.h Include/output.h \
  /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h \
  /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h \
  /usr/include/SDL/SDL_config-i386.h /usr/include/SDL/SDL_platform.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/strings.h /usr/include/inttypes.h \
  /usr/include/stdint.h /usr/include/ctype.h /usr/include/iconv.h \
  /usr/include/SDL/begin_code.h /usr/include/SDL/close_code.h \
  /usr/include/SDL/SDL_audio.h /usr/include/SDL/SDL_error.h \
  /usr/include/SDL/SDL_endian.h /usr/include/SDL/SDL_mutex.h \
  /usr/include/SDL/SDL_thread.h /usr/include/SDL/SDL_rwops.h \
  /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_cpuinfo.h \
  /usr/include/SDL/SDL_events.h /usr/include/SDL/SDL_active.h \
  /usr/include/SDL/SDL_keyboard.h /usr/include/SDL/SDL_keysym.h \
  /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h \
  /usr/include/SDL/SDL_joystick.h /usr/include/SDL/SDL_quit.h \
  /usr/include/SDL/SDL_loadso.h /usr/include/SDL/SDL_timer.h \
  /usr/include/SDL/SDL_version.h Include/pingpong_buf_mgmt.h
pingpong_buf_mgmt.o: pingpong_buf_mgmt.c /usr/include/stdio.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h \
  /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
  /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/alloca.h /usr/include/pthread.h /usr/include/sched.h \
  /usr/include/bits/sched.h /usr/include/signal.h \
  /usr/include/bits/setjmp.h /usr/include/assert.h Include/common.h \
  Include/pingpong_buf_mgmt.h /usr/include/string.h
