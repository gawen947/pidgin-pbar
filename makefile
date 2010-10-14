include commands.mk

CFLAGS  := -std=c99 -O2 $(shell pkg-config --cflags pidgin purple gtk+-2.0 gobject-2.0) -fPIC -W -Wall -Wextra -pedantic
LDFLAGS := $(shell pkg-config --libs pidgin purple gtk+-2.0 gobject-2.0)

SRC  = $(wildcard *.c)
OBJ  = $(foreach obj, $(SRC:.c=.o), $(notdir $(obj)))
DEP  = $(SRC:.c=.d)
POFILES  = $(wildcard *.po)
CATALOGS = $(foreach cat, $(POFILES:.po=.mo), $(notdir $(cat)))

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
locales:=locales
install-locales:=install-locales
uninstall-locales:=uninstall-locales
endif

.PHONY: all clean

all: pbar.so $(locales)

pbar.so: $(OBJ)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -Wp,-MMD,$*.d -c $(CFLAGS) -o $@ $< 

clean:
	$(RM) $(DEP)
	$(RM) $(OBJ)
	$(RM) $(CATALOGS)
	$(RM) pbar.pot
	$(RM) pbar.so

install: $(install-locales)
	$(MKDIR) -p $(PLUGINDIR)
	$(INSTALL_PROGRAM) pbar.so $(PLUGINDIR)

uninstall: $(uninstall-locales)
	$(RM) $(PLUGINDIR)/pbar.so

locales: $(CATALOGS)

CAT_INST_PATH = $(foreach lang, $(POFILES:.po=), $(LOCALEDIR)/$(lang)/LC_MESSAGES/pidgin-pbar.mo)
install-locales: $(CAT_INST_PATH)
$(LOCALEDIR)/%/LC_MESSAGES/pidgin-pbar.mo: %.mo
	$(INSTALL_DATA) $< $@
uninstall-locales:
	$(RM) $(CAT_INST_PATH)

%.mo: %.po
	$(MSGFMT) -c -o $@ $<

pbar.pot: POTFILES.in
	$(XGETTEXT) --files-from $< --keyword=N_ -o $@ --no-wrap --no-location

-include $(DEP)

