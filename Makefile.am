# Put compiled object files into the same sub-directory as their sources
AUTOMAKE_OPTIONS = subdir-objects

# Point to our macro directory and pick up user flags from the environment
ACLOCAL_AMFLAGS  = -I macros ${ACLOCAL_FLAGS}

# Make warnings always fatal at distcheck time, and disable execution
# of tools that change files not installed by the package itself
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal --disable-update-mime-database

SUBDIRS = po help docs


AM_CXXFLAGS = $(PREFIXSUFFIX_WXXFLAGS)

bin_PROGRAMS = prefixsuffix

#List of source files needed to build the executable:
prefixsuffix_SOURCES = \
	src/main.cc \
	src/application.h \
	src/application.cc \
	src/constants.h \
	src/mainwindow.h \
	src/mainwindow.cc \
	src/file_renamer.h \
	src/file_renamer.cc \
	src/string_renamer.h \
	src/string_renamer.cc \
	src/bakery/Configuration/Association.h \
	src/bakery/Configuration/Association.cc \
	src/bakery/Configuration/AssociationBase.h \
	src/bakery/Configuration/AssociationBase.cc \
	src/bakery/Configuration/Client.h \
	src/bakery/Configuration/Client.cc \
	data/prefixsuffix-resources.c

EXTRA_DIST = \
	data/prefixsuffix-logo.png \
	data/prefixsuffix.glade

gladedir = $(datadir)/prefixsuffix/glade/

prefixsuffix_CFLAGS = $(DEPS_CFLAGS) \
	-I$(top_builddir)/src -I$(top_srcdir)/src \
	-I$(top_builddir) -I$(top_srcdir)

prefixsuffix_CXXFLAGS = $(DEPS_CFLAGS) \
	$(PREFIXSUFFIX_WXXFLAGS) \
	-I$(top_builddir)/src -I$(top_srcdir)/src \
	-I$(top_builddir) -I$(top_srcdir) \
  -DPREFIXSUFFIX_APP_ID=\"io.github.murraycu.PrefixSuffix\"

prefixsuffix_LDADD = $(DEPS_LIBS)



# Desktop entry
desktopdir		= $(datadir)/applications
desktop_DATA 		= data/prefixsuffix.desktop
@INTLTOOL_DESKTOP_RULE@

# Icon
icondir = $(datadir)/pixmaps
icon_DATA = prefixsuffix.png

# GSettings schemas, enum files and conversion file
#gsettings_ENUM_NAMESPACE = io.github.murraycu.prefixsuffix
#gsettings_ENUM_FILES = $(top_srcdir)/src/*.h $(top_srcdir)/src/backend/*.h
gsettings_SCHEMAS = data/io.github.murraycu.PrefixSuffix.gschema.xml
@GSETTINGS_RULES@


# Generate the GResource source file for .glade and icons files:
prefixsuffix_resource_file = $(srcdir)/data/prefixsuffix.gresource.xml
BUILT_SOURCES = data/prefixsuffix-resources.c
data/prefixsuffix-resources.c: $(prefixsuffix_resource_file) $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(prefixsuffix_resource_file))
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --internal --target=$@ --sourcedir=$(srcdir) --generate-source $(prefixsuffix_resource_file)


EXTRA_DIST += $(prefixsuffix_resource_file) \
            $(desktop_DATA) $(icon_DATA) \
             intltool-extract.in intltool-merge.in intltool-update.in


EXTRA_DIST += data/prefixsuffix.glade \
	$(prefixsuffix_resource_file) 

CLEANFILES = \
	$(desktop_DATA)

include Makefile_tests.am

