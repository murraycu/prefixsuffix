AC_INIT([PrefixSuffix],[0.5.0],[http://bugzilla.gnome.org/enter_bug.cgi?product=PrefixSuffix],[prefixsuffix])
AC_PREREQ(2.63)

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_MACRO_DIR([macros])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10 -Wno-portability no-define nostdinc tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

dnl Checks for programs.
AC_PROG_CXX

dnl Checks for libraries.
PKG_CHECK_MODULES(DEPS, gtkmm-3.0)


# i18n
IT_PROG_INTLTOOL([0.35])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

AC_SUBST([GETTEXT_PACKAGE], [prefixsuffix])
AC_DEFINE([GETTEXT_PACKAGE], [PACKAGE_TARNAME], [Define to the gettext package name.])


dnl Create makefiles
AC_OUTPUT(
	Makefile
	src/Makefile
	po/Makefile.in
	help/Makefile
	help/C/Makefile
        docs/Makefile
)
