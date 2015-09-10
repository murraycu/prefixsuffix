AC_INIT([PrefixSuffix],[0.6.6],[http://bugzilla.gnome.org/enter_bug.cgi?product=PrefixSuffix],[prefixsuffix])
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

AX_CXX_COMPILE_STDCXX_11([noext],[optional])

dnl Checks for libraries.
PKG_CHECK_MODULES(DEPS, gtkmm-3.0)

# Allow use of mm-common macros for the warnings option.
MM_PREREQ([0.9.5])

AC_LANG([C++])
MM_ARG_ENABLE_WARNINGS([PREFIXSUFFIX_WXXFLAGS],
                       [-Wall],
                       [-pedantic -Wall -Wextra -Wformat-security -Wno-missing-field-initializers -DGSEAL_ENABLE],
                       [G GDK GDK_PIXBUF GTK GLIBMM GDKMM CAIROMM PANGOMM GTKMM SIGCXX])


# i18n
IT_PROG_INTLTOOL([0.35])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

AC_SUBST([GETTEXT_PACKAGE], [prefixsuffix])
AC_DEFINE([GETTEXT_PACKAGE], [PACKAGE_TARNAME], [Define to the gettext package name.])

GLIB_GSETTINGS

AC_CONFIG_FILES([Makefile
                 po/Makefile.in
                 data/prefixsuffix.desktop.in
                 data/org.gnome.prefixsuffix.gschema.xml
                 help/Makefile
                 help/C/Makefile
                 docs/Makefile])

AC_OUTPUT
