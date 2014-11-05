dnl $Id: configure.ac,v 1.5 2005/06/18 12:50:50 murrayc Exp $ -*- m4 -*-

dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.13)

AC_INIT(src/main.cc)

AM_CONFIG_HEADER(config.h)

dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

PACKAGE=prefixsuffix
VERSION=0.5.0

dnl -----------------------------------------------
dnl Package name and version number (DO NOT MODIFY)
dnl -----------------------------------------------

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

GNOME_COMMON_INIT

dnl Checks for programs.
AC_PROG_CXX

dnl Checks for libraries.
PKG_CHECK_MODULES(DEPS, bakery-2.4)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)


#Internationalisation:
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
		   [The domain name to use with gettext.])

ALL_LINGUAS="hu de"

AM_GLIB_GNU_GETTEXT
AC_PROG_INTLTOOL


dnl Create makefiles
AC_OUTPUT(
	Makefile
	src/Makefile
	po/Makefile.in
	help/Makefile
	help/C/Makefile
        docs/Makefile
)
