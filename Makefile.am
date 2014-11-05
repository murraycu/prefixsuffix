SUBDIRS = src po help docs

# Desktop entry
desktopdir		= $(datadir)/applications
desktop_in_files	= prefixsuffix.desktop.in
desktop_DATA 		= $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

# Icon
icondir = $(datadir)/pixmaps
icon_DATA = prefixsuffix.png

EXTRA_DIST = $(desktop_in_files) $(desktop_DATA) $(icon_DATA) \
             intltool-extract.in intltool-merge.in intltool-update.in
