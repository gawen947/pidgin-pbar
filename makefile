CC=gcc
RM=rm -f
INSTALL=install

COMMIT=$(shell ./hash.sh)

CFLAGS+=-O2 -Wall
ifdef DEBUG
CFLAGS+=-g
endif

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

all: $(plugin)

$(plugin): $(OBJS)
$(plugin): CFLAGS := $(CFLAGS) $(PIDGIN_CFLAGS) \
							$(GOBJECT_CFLAGS) $(GTK_CFLAGS) \
							$(INTERNAL_CFLAGS) \

$(plugin): LIBS := $(PIDGIN_LIBS) $(GTK_LIBS) \
							$(GOBJECT_LIBS) \

# $(P)CC shows [CC] and the next line shows the nice output
%.o:: %.c *.h
	$(P)CC
	$(Q)$(CC) -DCOMMIT="\"$(COMMIT)\"" $(CFLAGS) -Wp,-MMD,$(dir $@).$(notdir $@).d -o $@ -c $<

# Make the shared object/lib
%.so::
	$(P)SHLIB
	$(Q)$(CC) -shared -o $@ $^ $(LIBS)

clean:
	$(RM) $(plugin) $(OBJS)

install: $(plugin)
	mkdir -p $(PIDGIN_PLUGINDIR)
	$(INSTALL) $(plugin) $(PIDGIN_PLUGINDIR)

uninstall:
	$(RM) $(PIDGIN_PLUGINDIR)/$(plugin)
