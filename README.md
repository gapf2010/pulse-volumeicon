pulse-volumeicon
================

A small system tray icon for PulseAudio / PipeWire volume control.

- **Scroll** over the icon to change the volume (5% per step)
- **Middle click** to toggle mute
- **Left click** opens a menu with the current level, Mute and Quit
- A desktop notification shows the new level on each change

The icon uses the StatusNotifierItem protocol (via libayatana-appindicator),
so it works in KDE Plasma, Xfce, Waybar and other modern trays. On GNOME it
needs the AppIndicator extension.

Volume is controlled through `pactl` on the default sink, so it works with
PulseAudio 15+ and with PipeWire (through `pipewire-pulse`).

Dependencies
------------

- GTK 3
- libayatana-appindicator (GTK 3 version, `ayatana-appindicator3-0.1`)
- libnotify
- `pactl` at runtime

Package names on a few distros:

- Fedora: `gtk3-devel libayatana-appindicator-gtk3-devel libnotify-devel`
- Arch / Omarchy: `gtk3 libayatana-appindicator libnotify libpulse`
- Slackware: GTK 3 and libnotify are in the base install;
  libayatana-appindicator is available from SlackBuilds.org

Building
--------

    make
    ./pulse-volumeicon

To install to `/usr/local/bin`:

    sudo make install

The Makefile honours `PREFIX`, `DESTDIR`, `CFLAGS`, `CPPFLAGS` and `LDFLAGS`.

Packaging
---------

Packaging files for several distributions are included:

- Fedora: `packaging/fedora/pulse-volumeicon.spec`
- Debian / Ubuntu: `debian/`
- Arch / Omarchy: `packaging/arch/PKGBUILD`
- Slackware: `packaging/slackware/` (SlackBuilds.org format)

License
-------

MIT, see `LICENSE`.
