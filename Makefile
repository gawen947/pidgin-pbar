include commands.mk

OPTS    := -O2
CFLAGS  := -std=c99 $(OPTS) $(shell pkg-config --cflags pidgin purple gtk+-2.0 gobject-2.0) -fPIC -Wall
LDFLAGS := $(shell pkg-config --libs pidgin purple gtk+-2.0 gobject-2.0)

SRC  = $(wildcard *.c)
OBJ  = $(foreach obj, $(SRC:.c=.o), $(notdir $(obj)))
DEP  = $(SRC:.c=.d)
POFILES  = $(wildcard *.po)
CATALOGS = $(foreach cat, $(POFILES:.po=.mo), $(notdir $(cat)))

PREFIX    ?= /usr/local
LOCALEDIR ?= $(PREFIX)/share/locale
PLUGINDIR ?= $(shell pkg-config --variable=plugindir pidgin)
DATADIR   ?= $(shell pkg-config --variable=datadir pidgin)

PBAR_VERSION := $(shell cat VERSION)
CFLAGS += -DVERSION="\"$(PBAR_VERSION)\""

ifndef DISABLE_DEBUG
CFLAGS += -ggdb
endif

commit = $(shell ./hash.sh)
ifneq ($(commit), UNKNOWN)
CFLAGS += -DCOMMIT="\"$(commit)\""
CFLAGS += -DPARTIAL_COMMIT="\"$(shell echo $(commit) | cut -c1-8)\""
endif

ifndef DISABLE_NLS
CFLAGS += -DENABLE_NLS=1 -DLOCALEDIR="\"$(LOCALEDIR)\""
locales           := locales
install-locales   := install-locales
uninstall-locales := uninstall-locales
endif

CFLAGS += -DDATADIR="\"$(DATADIR)\""

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
	$(RM) messages.pot
	$(RM) pbar.so

install: $(install-locales)
	$(INSTALL_DIR) $(DESTDIR)$(PLUGINDIR)
	$(INSTALL_LIB) pbar.so $(DESTDIR)$(PLUGINDIR)

uninstall: $(uninstall-locales)
	$(RM) $(PLUGINDIR)/pbar.so

locales: $(CATALOGS)

CAT_INST_PATH = $(foreach lang, $(POFILES:.po=), $(DESTDIR)$(LOCALEDIR)/$(lang)/LC_MESSAGES/pidgin-pbar.mo)

install-locales: $(CAT_INST_PATH)

$(DESTDIR)$(LOCALEDIR)/%/LC_MESSAGES/pidgin-pbar.mo: %.mo
	$(INSTALL_DIR) $(DESTDIR)$(LOCALEDIR)/$(basename $<)/LC_MESSAGES
	$(INSTALL_DATA) $< $@

uninstall-locales:
	$(RM) $(CAT_INST_PATH)

%.mo: %.po
	$(MSGFMT) -c -o $@ $<

messages.pot: POTFILES.in
	$(XGETTEXT) --no-wrap -m -c --files-from $< --keyword --keyword=_ --keyword=N_ -o $@

-include $(DEP)

