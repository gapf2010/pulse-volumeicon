PKGS = gtk+-3.0 ayatana-appindicator3-0.1 libnotify

CFLAGS ?= -O2
CFLAGS += -Wall $(shell pkg-config --cflags $(PKGS))
LDLIBS += $(shell pkg-config --libs $(PKGS))

SOURCES = main.c trayicon.c pulseaudio.c volnotify.c
HEADERS = trayicon.h pulseaudio.h volnotify.h
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = pulse-volumeicon

PREFIX ?= /usr/local
DESTDIR ?=

all: $(EXECUTABLE)

# Libraries go after the objects, or linkers using --as-needed drop them
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(EXECUTABLE)
	install -Dm755 $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all install uninstall clean
