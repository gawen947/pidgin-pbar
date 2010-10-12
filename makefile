CC=gcc
RM=rm -f
INSTALL=install
INTLTOOL_UPDATE=intltool-update
XGETTEXT=xgettext
MSGFMT=msgfmt
MKDIR=mkdir
CP=cp

COMMIT=$(shell ./hash.sh)

CFLAGS+=-O2 -Wall
ifdef DEBUG
CFLAGS+=-g
endif
ifdef NLS
CFLAGS+=-DGETTEXT_PACKAGE="\"pidgin-pbar\""
endif
ifneq ($(COMMIT),UNKNOWN)
CFLAGS+=-DCOMMIT="\"$(COMMIT)\""
endif

PREFIX=/usr/local
LOCALEDIR=$(PREFIX)/share/locale
PIDGIN_CFLAGS=`pkg-config --cflags pidgin purple`
PIDGIN_LIBS=`pkg-config --libs pidgin purple`
PIDGIN_PLUGINDIR=`pkg-config --variable=plugindir pidgin`

GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
GTK_LIBS=`pkg-config --libs gtk+-2.0`

GOBJECT_CFLAGS=`pkg-config --cflags gobject-2.0`
GOBJECT_LIBS=`pkg-config --libs gobject-2.0`

# for simple output
P = @printf "[%s] $@\n" # <- space before hash is important!!!
Q = @


SHLIBEXT=so
ifdef STATIC
	plugin=pbar.a
else
	plugin=pbar.$(SHLIBEXT)
	override CFLAGS += -fPIC
endif

.PHONY: all clean

OBJS = pbar.o preferences.o widget.o widget_gtk.o widget_prpl.o purple.o


binary: $(plugin)

$(plugin): $(OBJS)
$(plugin): CFLAGS := $(CFLAGS) $(PIDGIN_CFLAGS) \
							$(GOBJECT_CFLAGS) $(GTK_CFLAGS) \
							$(INTERNAL_CFLAGS) \

$(plugin): LIBS := $(PIDGIN_LIBS) $(GTK_LIBS) \
							$(GOBJECT_LIBS) \

# $(P)CC shows [CC] and the next line shows the nice output
%.o:: %.c *.h
	$(P)CC
	$(Q)$(CC) $(CFLAGS) -Wp,-MMD,$(dir $@).$(notdir $@).d -o $@ -c $<

# Make the shared object/lib
%.so::
	$(P)SHLIB
	$(Q)$(CC) -shared -o $@ $^ $(LIBS)

all: binary locales
clean-binary:
	$(RM) $(plugin) $(OBJS)

install-binary: $(plugin)
	$(MKDIR) -p $(PIDGIN_PLUGINDIR)
	$(INSTALL) -s $(plugin) $(PIDGIN_PLUGINDIR)
uninstall-binary:
	$(RM) $(PIDGIN_PLUGINDIR)/$(plugin)

install: install-binary install-locales
uninstall: uninstall-binary uninstall-locales
clean: clean-binary clean-locales

po/pbar.pot: po/POTFILES.in
	$(XGETTEXT) --files-from $< --keyword=N_ -o $@ --no-wrap --no-location

po/%.mo: po/%.po
	$(MSGFMT) -c -o $@ $<

clean-locales-%:
	$(RM) po/$*.mo

install-locales-%: po/%.mo
	$(INSTALL) $< $(LOCALEDIR)/$*/LC_MESSAGES/pidgin-pbar.mo

uninstall-locales-%:
	$(RM) $(LOCALEDIR)/$*/LC_MESSAGES/pidgin-pbar.mo

locales: po/fr.mo
clean-locales: clean-locales-fr
	$(RM) po/pbar.pot
install-locales: install-locales-fr
uninstall-locales: uninstall-locales-fr
