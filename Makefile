PREFIX=/usr

INCLUDES= \
		  -Iinclude/ \
		  -Iexternal/PAL-CRT/ \

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic $(INCLUDES)
LDFLAGS=-L/usr/lib

OBJS= \
	src/bk_pal_crt.o \

all: bk_pal_crt.so 

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) -fpic

bk_pal_crt.so: src/bk_pal_crt.o external/PAL-CRT/pal.o external/PAL-CRT/pal_core.o external/PAL-CRT/pal_nes.o external/PAL-CRT/pal_nesrgb.o
	# change PAL_SYSTEM from PAL_SYSTEM_NES_RGB to PAL_SYSTEM_PAL
	sed -i -e 's/#define PAL_SYSTEM PAL_SYSTEM_NESRGB/#define PAL_SYSTEM PAL_SYSTEM_PAL/g' external/PAL-CRT/pal_core.h
	$(CC) -shared -o $@ $^

.PHONY: clean install

clean:
	$(RM) bk_pal_crt.so src/*.o external/PAL-CRT/*.o

install: bk_pal_crt.so
	mkdir -p $(DESTDIR)$(PREFIX)/share/kdenlive/effects
	mkdir -p $(DESTDIR)$(PREFIX)/lib/frei0r-1/

	cp config/kdenlive/frei0r_bk_pal_crt.xml $(DESTDIR)$(PREFIX)/share/kdenlive/effects
	cp bk_pal_crt.so $(DESTDIR)$(PREFIX)/lib/frei0r-1/
