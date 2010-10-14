include commands.mk

CFLAGS  := -std=c99 -O2 $(shell pkg-config --cflags pidgin purple gtk+-2.0 gobject-2.0) -fPIC -W -Wall -Wextra -pedantic
LDFLAGS := $(shell pkg-config --libs pidgin purple gtk+-2.0 gobject-2.0)

SRC  = $(wildcard *.c)
OBJ  = $(foreach obj, $(SRC:.c=.o), $(notdir $(obj)))
DEP  = $(SRC:.c=.d)

PREFIX    ?= /usr/local
LOCALEDIR ?= $(PREFIX)/share/locale
PLUGINDIR ?= $(shell pkg-config --variable=plugindir pidgin)

ifdef DEBUG
CFLAGS += -ggdb
endif

commit = $(shell ./hash.sh)
ifneq ($(commit), UNKNOWN)
CFLAGS+=-DCOMMIT="\"$(commit)\""
endif

ifndef DISABLE_NLS
CFLAGS+=-DENABLE_NLS=1 -DLOCALEDIR="\"$(LOCALEDIR)\""
endif

VPATH:=po

.PHONY: all clean

all: pbar.so

pbar.so: $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -Wp,-MMD,$*.d -c $(CFLAGS) -o $@ $< 

clean:
	$(RM) po/*.mo
	$(RM) $(DEP)
	$(RM) $(OBJ)
	$(RM) pbar.so

po/pbar.pot: po/POTFILES.in
	$(XGETTEXT) --files-from $< --keyword=N_ -o $@ --no-wrap --no-location

po/%.mo: po/%.po
	$(MSGFMT) -c -o $@ $<


-include $(DEP)

