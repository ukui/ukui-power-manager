VERSION = 1.0
all:  powersave powerperformance
BINDIR=/usr/bin
LOCALESDIR=/usr/share/locale
MANDIR=/usr/share/man/man8
WARNFLAGS=-Wall -Wshadow -W -Wformat -Wimplicit-function-declaration -Wimplicit-int
CFLAGS?=-O1 -g ${WARNFLAGS}
CC?=gcc

CFLAGS+=-D VERSION=\"$(VERSION)\"

OBJS = powersave.o ./IOscheduler/disk.o ./bluetooth/bluetooth.o ./usbAutoSuspend/usb.o ./wifi/wifi.o  ./urb/urb.o ./misc/misc.o ./CPU/cpu.o ./SataALPM/alpm.o ./WOL/wol.o
powersave: $(OBJS) Makefile
	        $(CC) ${CFLAGS} $(LDFLAGS) $(OBJS) -lpthread -lncursesw -o powersave


objects = powerperformance.o ./usbAutoSuspend/usb.o ./misc/misc.o
powerperformance: $(objects) Makefile
	        $(CC) ${CFLAGS} $(LDFLAGS) $(objects) -lpthread -o powerperformance

SUBDIR = $(shell ls ./ -R | grep /)
SUBDIRS = $(subst :,/,$(SUBDIR))
	SOURCE = $(foreach dir, $(SUBDIRS),$(wildcard $(dir)*.o))
clean:
	rm -rf $(SOURCE) powersave powerperformance
