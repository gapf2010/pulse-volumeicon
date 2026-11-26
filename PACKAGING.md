Building the packages
=====================

Every package is built from the tagged GitHub release tarball, so the
release must be tagged (for example `v0.1.0`) before building any of them.
Each package installs `/usr/bin/pulse-volumeicon` and a menu entry at
`/usr/share/applications/pulse-volumeicon.desktop`.

| Distro | Packaging files | Built with | Result |
| --- | --- | --- | --- |
| Fedora | `packaging/fedora/pulse-volumeicon.spec` | `rpmbuild` | `.rpm` |
| Debian / Ubuntu | `debian/` | `dpkg-buildpackage` | `.deb` |
| Arch / Omarchy | `packaging/arch/PKGBUILD` | `makepkg` | `.pkg.tar.zst` |
| Slackware | `packaging/slackware/` | the SlackBuild | `.tgz` |

Contents:

- [Tagging a release](#tagging-a-release)
- [Fedora](#fedora)
- [Debian / Ubuntu](#debian--ubuntu)
- [Arch / Omarchy](#arch--omarchy)
- [Slackware](#slackware)
- [Releasing a new version](#releasing-a-new-version)

Tagging a release
-----------------

Commit the packaging files, then create and push the tag:

```bash
git tag -a v0.1.0 -m "pulse-volumeicon 0.1.0"
git push origin v0.1.0
```

GitHub then serves the release tarball at
`https://github.com/gapf2010/pulse-volumeicon/archive/v0.1.0/pulse-volumeicon-0.1.0.tar.gz`.
The Fedora spec and the Slackware `.info` file point to that URL, and the
Arch PKGBUILD points to the same tag.

Two files need a checksum of that tarball, which can only be computed once
the tag exists: `sha256sums` in the Arch PKGBUILD and `MD5SUM` in the
Slackware `.info` file. The Arch and Slackware sections below show how to
fill them in.

Fedora
------

The spec builds a normal RPM with Fedora's own compiler and hardening
flags. It depends on `pulseaudio-utils` for `pactl`, which also works on
Fedora's default PipeWire setup.

1. Install the packaging tools and the build dependencies:

   ```bash
   sudo dnf install rpm-build rpmdevtools
   sudo dnf builddep packaging/fedora/pulse-volumeicon.spec
   ```

2. Create the `~/rpmbuild` tree and copy the spec into it:

   ```bash
   rpmdev-setuptree
   cp packaging/fedora/pulse-volumeicon.spec ~/rpmbuild/SPECS/
   ```

3. Download the release tarball named in `Source0`:

   ```bash
   spectool -g -R ~/rpmbuild/SPECS/pulse-volumeicon.spec
   ```

4. Build the binary and source RPMs:

   ```bash
   rpmbuild -ba ~/rpmbuild/SPECS/pulse-volumeicon.spec
   ```

5. Install the result from `~/rpmbuild/RPMS/x86_64/`:

   ```bash
   sudo dnf install ~/rpmbuild/RPMS/x86_64/pulse-volumeicon-0.1.0-1.*.rpm
   ```

Optional checks: `rpmlint` on the spec and RPMs catches packaging
mistakes. `mock --rebuild ~/rpmbuild/SRPMS/pulse-volumeicon-0.1.0-1.*.src.rpm`
builds in a clean chroot, the way Fedora's build servers do.

Debian / Ubuntu
---------------

The `debian/` folder at the top of the repo builds the `.deb` straight from
a git checkout. It needs a release with `libayatana-appindicator3-dev` and
debhelper 13, so Debian 12 or Ubuntu 22.04 and newer.

1. Install the build tools and dependencies:

   ```bash
   sudo apt install build-essential devscripts debhelper pkg-config \
       libgtk-3-dev libayatana-appindicator3-dev libnotify-dev
   ```

2. Check out the release tag:

   ```bash
   git checkout v0.1.0
   ```

3. Build the binary package (`-us -uc` skips signing):

   ```bash
   dpkg-buildpackage -us -uc -b
   ```

4. Install it. The `.deb` lands in the folder above the repo:

   ```bash
   sudo apt install ../pulse-volumeicon_0.1.0-1_amd64.deb
   ```

To also build a source package (needed for a PPA or for Debian itself), put
the release tarball next to the repo under Debian's naming first, then run
`dpkg-buildpackage -us -uc` without `-b`:

```bash
curl -L -o ../pulse-volumeicon_0.1.0.orig.tar.gz \
    https://github.com/gapf2010/pulse-volumeicon/archive/v0.1.0/pulse-volumeicon-0.1.0.tar.gz
```

`debian/rules clean` removes the build leftovers, and
`lintian ../pulse-volumeicon_0.1.0-1_amd64.deb` checks the package against
Debian policy.

Arch / Omarchy
--------------

The PKGBUILD downloads the tagged tarball and builds it with `makepkg`.
`libpulse` provides `pactl`, which also works with PipeWire.

1. Install the build tools (`pacman-contrib` provides `updpkgsums`):

   ```bash
   sudo pacman -S --needed base-devel pacman-contrib
   ```

2. Copy the PKGBUILD to a build folder, so the download and build files
   stay out of the repo:

   ```bash
   mkdir -p ~/build/pulse-volumeicon
   cp packaging/arch/PKGBUILD ~/build/pulse-volumeicon/
   cd ~/build/pulse-volumeicon
   ```

3. Replace the `SKIP` checksum with the real one. This downloads the
   tarball and rewrites `sha256sums` in the PKGBUILD:

   ```bash
   updpkgsums
   ```

4. Build, pull in missing dependencies (`-s`) and install (`-i`):

   ```bash
   makepkg -si
   ```

The package file is `pulse-volumeicon-0.1.0-1-x86_64.pkg.tar.zst`. Copy the
updated PKGBUILD back to `packaging/arch/` and commit it, so the checksum is
saved. `namcap` checks the PKGBUILD and the package. To publish on the AUR,
the PKGBUILD goes in its own AUR git repo together with a `.SRCINFO` made by
`makepkg --printsrcinfo > .SRCINFO`.

Slackware
---------

The files in `packaging/slackware/` follow the
[SlackBuilds.org](https://slackbuilds.org) layout, targeting Slackware 15.0.
GTK 3, libnotify and PulseAudio 15 (with `pactl`) come with the base
install. The only extra dependency is libayatana-appindicator, which is
[on SlackBuilds.org](https://slackbuilds.org/repository/15.0/libraries/libayatana-appindicator/)
and requires libayatana-indicator.

1. Install libayatana-appindicator and its dependencies from
   SlackBuilds.org. A tool such as `sbopkg` or `sboinstall` resolves the
   chain for you.

2. Copy the SlackBuild files to a build folder and download the release
   tarball there:

   ```bash
   mkdir -p ~/slackbuilds/pulse-volumeicon
   cp packaging/slackware/* ~/slackbuilds/pulse-volumeicon/
   cd ~/slackbuilds/pulse-volumeicon
   wget https://github.com/gapf2010/pulse-volumeicon/archive/v0.1.0/pulse-volumeicon-0.1.0.tar.gz
   ```

3. Put the tarball's checksum into `MD5SUM` in `pulse-volumeicon.info`,
   replacing `REPLACE_WITH_MD5SUM_OF_RELEASE_TARBALL`:

   ```bash
   md5sum pulse-volumeicon-0.1.0.tar.gz
   ```

4. Run the SlackBuild as root:

   ```bash
   su -c ./pulse-volumeicon.SlackBuild
   ```

5. Install the package it writes to `/tmp`:

   ```bash
   su -c 'installpkg /tmp/pulse-volumeicon-0.1.0-x86_64-1_SBo.tgz'
   ```

Use `upgradepkg` instead of `installpkg` for later versions. The usual
SlackBuild variables work: `OUTPUT=` changes the output folder, and `TAG=`
or `BUILD=` change the package name. To submit to SlackBuilds.org, upload
the folder (README, `.info`, SlackBuild, `slack-desc`, `doinst.sh`) as a
tarball with the form on their site.

Releasing a new version
-----------------------

For a new release (say 0.2.0), update the version in these files, commit,
then tag `v0.2.0` and push the tag as described above.

| File | What to change | Helper |
| --- | --- | --- |
| `packaging/fedora/pulse-volumeicon.spec` | `Version:`, `Release:` back to 1, a new `%changelog` entry | `rpmdev-bumpspec -n 0.2.0 -c "New release"` |
| `debian/changelog` | A new entry at the top, version `0.2.0-1` | `dch -v 0.2.0-1` |
| `packaging/arch/PKGBUILD` | `pkgver=`, `pkgrel=1`, then the checksum | `updpkgsums` after tagging |
| `packaging/slackware/pulse-volumeicon.info` | `VERSION`, the `DOWNLOAD` URL, `MD5SUM` | `md5sum` after tagging |
| `packaging/slackware/pulse-volumeicon.SlackBuild` | The default in `VERSION=${VERSION:-...}` | |

The Arch and Slackware checksums can only be filled in once the tag exists.
Commit them in a follow-up commit; the tarball doesn't change because of it.

If you fix only the packaging and not the code, bump the package's own
revision instead: `Release:` in the spec, the `-1` in `debian/changelog`,
`pkgrel` in the PKGBUILD, or `BUILD` in the SlackBuild.
