Name:           pulse-volumeicon
Version:        0.1.0
Release:        1%{?dist}
Summary:        Tray icon for PulseAudio and PipeWire volume control

License:        MIT
URL:            https://github.com/gapf2010/pulse-volumeicon
Source0:        %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  pkgconfig(gtk+-3.0)
BuildRequires:  pkgconfig(ayatana-appindicator3-0.1)
BuildRequires:  pkgconfig(libnotify)
BuildRequires:  desktop-file-utils

# The volume is changed through pactl, which also works with PipeWire
Requires:       pulseaudio-utils

%description
pulse-volumeicon is a small system tray icon to control the sound volume.
Scroll over the icon to change the volume, middle click to toggle mute,
and left click for a menu. It uses the StatusNotifierItem protocol, so it
works in KDE Plasma, Xfce, Waybar and other modern trays, with a fallback
for older XEmbed trays.

%prep
%autosetup

%build
%set_build_flags
%make_build PREFIX=%{_prefix}

%install
%make_install PREFIX=%{_prefix}

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop

%files
%license LICENSE
%doc README.md
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop

%changelog
* Thu Nov 26 2026 German Pulido <1203374+gapf2010@users.noreply.github.com> - 0.1.0-1
- Initial package
