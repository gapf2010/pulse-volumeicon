PKGS = gtk+-3.0 ayatana-appindicator3-0.1 libnotify

PKG_CONFIG ?= pkg-config
PKG_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(PKGS))
PKG_LIBS   := $(shell $(PKG_CONFIG) --libs $(PKGS))

# Distro build systems pass their own CFLAGS, CPPFLAGS and LDFLAGS
# (hardening, optimization, LTO...). The library flags are kept in
# separate variables so that overriding CFLAGS on the make command line,
# as some build scripts do, doesn't break the build.
CFLAGS ?= -O2
WARNFLAGS = -Wall

SOURCES = main.c trayicon.c pulseaudio.c volnotify.c
HEADERS = trayicon.h pulseaudio.h volnotify.h
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = pulse-volumeicon

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DATADIR ?= $(PREFIX)/share
DESTDIR ?=

all: $(EXECUTABLE)

# Libraries go after the objects, or linkers using --as-needed drop them
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(PKG_LIBS) $(LDLIBS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CPPFLAGS) $(WARNFLAGS) $(CFLAGS) $(PKG_CFLAGS) -c $< -o $@

install: $(EXECUTABLE)
	install -Dm755 $(EXECUTABLE) $(DESTDIR)$(BINDIR)/$(EXECUTABLE)
	install -Dm644 $(EXECUTABLE).desktop \
		$(DESTDIR)$(DATADIR)/applications/$(EXECUTABLE).desktop

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(EXECUTABLE)
	rm -f $(DESTDIR)$(DATADIR)/applications/$(EXECUTABLE).desktop

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all install uninstall clean
