PREFIX=/usr

INCLUDES= \
		  -Iinclude/ \
		  -Iexternal/PAL-CRT/ \

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -Ofast $(INCLUDES)
LDFLAGS=-L/usr/lib

OBJS= \
	bk_pal_crt.so \
	bk_wavr.so \

KDENLIVE_CONFIGS= \
		  config/kdenlive/frei0r_bk_pal_crt.xml \
		  config/kdenlive/frei0r_bk_wavr.xml \

all: change_pal_system bk_pal_crt.so bk_wavr.so

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) -fpic

change_pal_system:
	# change PAL_SYSTEM from PAL_SYSTEM_NES_RGB to PAL_SYSTEM_PAL
	sed -i -e 's/#define PAL_SYSTEM PAL_SYSTEM_NESRGB/#define PAL_SYSTEM PAL_SYSTEM_PAL/g' external/PAL-CRT/pal_core.h

bk_pal_crt.so: src/bk_pal_crt.o external/PAL-CRT/pal.o external/PAL-CRT/pal_core.o external/PAL-CRT/pal_nes.o external/PAL-CRT/pal_nesrgb.o
	$(CC) -shared -o $@ $^

bk_wavr.so: src/bk_wavr.o
	$(CC) -shared -o $@ $^

.PHONY: clean install

clean:
	$(RM) *.so src/*.o external/PAL-CRT/*.o

install: $(OBJS)
	mkdir -p $(DESTDIR)$(PREFIX)/share/kdenlive/effects
	mkdir -p $(DESTDIR)$(PREFIX)/lib/frei0r-1

	cp $(KDENLIVE_CONFIGS) $(DESTDIR)$(PREFIX)/share/kdenlive/effects/
	cp $(OBJS) $(DESTDIR)$(PREFIX)/lib/frei0r-1/
