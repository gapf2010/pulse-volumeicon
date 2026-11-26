Using pulse-volumeicon with lightweight window managers
=======================================================

pulse-volumeicon works in full desktops (KDE Plasma, Xfce) without any
setup. Lightweight window managers such as FVWM, Fluxbox, Openbox or
Window Maker need a few pieces in place first. This page explains what
they are and how to set them up.

How the tray icon is shown
--------------------------

There are two tray protocols on Linux:

- **StatusNotifierItem (SNI)**, the modern one, used by KDE Plasma, Xfce
  and Waybar.
- **XEmbed**, the older X11 one, used by most lightweight trays
  (stalonetray, trayer, tint2, Fluxbox's toolbar, i3bar).

pulse-volumeicon uses libayatana-appindicator, which speaks SNI. When no
SNI tray is running, the library falls back to an XEmbed icon, so the
icon still appears in older trays. Expect it to take a second or two to
show up while the library checks for an SNI tray before falling back.

Checklist
---------

Under a lightweight window manager, make sure you have all of these:

1. **A system tray.** Some window managers have one built in, others
   need a separate tray program (see the table below).
2. **A D-Bus session bus.** The appindicator library, desktop
   notifications and the "only one copy running" check all use it.
   Display managers usually start one for you. If you start X with
   `startx`, start your window manager through `dbus-launch` in
   `~/.xinitrc`, for example:

       exec dbus-launch --exit-with-session fvwm3

   Without a session bus the icon still starts, but you may get no
   notifications, and nothing stops a second copy from running.
3. **A notification daemon** (optional), such as `dunst` or
   `xfce4-notifyd`, to see the volume popups. Without one, the icon
   works but volume changes show no popup.
4. **Start the tray before pulse-volumeicon**, or add a short delay, so
   the icon finds the tray.

Window managers at a glance
---------------------------

| Window manager | Built-in tray | What to do |
| --- | --- | --- |
| FVWM (fvwm2, fvwm3) | No | Run `stalonetray` or `trayer`, optionally swallowed into FvwmButtons |
| Fluxbox | Yes, in the toolbar (XEmbed) | Make sure `systemtray` is in `session.screen0.toolbar.tools` |
| Openbox | No | Run a panel with a tray, such as `tint2`, or `stalonetray` |
| Window Maker | No | Run `stalonetray` (or a tray dockapp) |
| IceWM | Yes, in the taskbar (XEmbed) | Nothing extra |
| i3 | Yes, in i3bar (XEmbed) | Nothing extra; i3bar shows the tray on the primary output by default |
| Sway, Hyprland | Through Waybar (SNI) | Enable Waybar's `tray` module |

Examples
--------

### FVWM

FVWM has no tray of its own. A minimal setup with stalonetray in the
top-right corner, in your FVWM config:

```
AddToFunc StartFunction
+ I Exec exec stalonetray --geometry 1x1-0+0 --icon-size 22 --grow-gravity E
+ I Exec exec sh -c "sleep 2; exec pulse-volumeicon"
```

If you use an FvwmButtons panel, you can swallow stalonetray into it so
the tray looks like part of the panel.

### Fluxbox

Fluxbox's toolbar has a tray. Check that `~/.fluxbox/init` lists it:

```
session.screen0.toolbar.tools: prevworkspace, workspacename, nextworkspace, iconbar, systemtray, clock
```

Then start the icon from `~/.fluxbox/startup`, before the final
`exec fluxbox` line:

```
pulse-volumeicon &
```

### Openbox

Openbox has no panel. With tint2 (which has a tray) in
`~/.config/openbox/autostart`:

```
tint2 &
(sleep 2; pulse-volumeicon) &
```

### Window Maker

Window Maker has no tray. Start stalonetray and the icon from
`~/GNUstep/Library/WindowMaker/autostart`:

```
stalonetray --icon-size 22 &
(sleep 2; pulse-volumeicon) &
```

Troubleshooting
---------------

**The icon never appears.** Check that a tray is running (see the table
above). If it is, the library's XEmbed fallback may not be working on
your system. Run [snixembed](https://git.sr.ht/~steef/snixembed) before
pulse-volumeicon. It shows SNI icons in XEmbed trays.

**Middle-click doesn't mute.** In XEmbed fallback mode, middle-click may
not be passed on to the app. Use Mute in the left-click menu instead.

**No volume popups.** Start a notification daemon such as `dunst`, and
make sure a D-Bus session bus is running.

**Two speaker icons.** There is no D-Bus session bus, so the
"only one copy running" check can't work. Start your window manager
through `dbus-launch` as shown in the checklist.

**The volume doesn't change.** Run `pactl get-sink-volume @DEFAULT_SINK@`
in a terminal. If it fails, install the package that provides `pactl`
(`pulseaudio-utils` on Fedora and Debian/Ubuntu, `libpulse` on Arch),
and check that PulseAudio or PipeWire's `pipewire-pulse` is running.
