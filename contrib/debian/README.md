
Debian
====================
This directory contains files used to package pandemiad/pandemia-qt
for Debian-based Linux systems. If you compile pandemiad/pandemia-qt yourself, there are some useful files here.

## pandemia: URI support ##


pandemia-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install pandemia-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your pandemiaqt binary to `/usr/bin`
and the `../../share/pixmaps/pandemia128.png` to `/usr/share/pixmaps`

pandemia-qt.protocol (KDE)

