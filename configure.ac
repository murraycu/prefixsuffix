AC_INIT([PrefixSuffix],[0.6.9],[https://github.com/murraycu/prefixsuffix/issues],[prefixsuffix])
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
AC_PROG_RANLIB

AX_CXX_COMPILE_STDCXX([14], [noext],[optional])

dnl Checks for libraries.
PKG_CHECK_MODULES(DEPS, gtkmm-4.0)

# Allow use of mm-common macros for the warnings option.
MM_PREREQ([0.9.5])

AC_LANG([C++])
MM_ARG_ENABLE_WARNINGS([PREFIXSUFFIX_WXXFLAGS],
                       [-Wall],
                       [-pedantic -Wall -Wextra -Wsuggest-override -Wshadow -Wformat-security -Wno-missing-field-initializers -DGSEAL_ENABLE],
                       [G GDK GDK_PIXBUF GTK GLIBMM GDKMM CAIROMM PANGOMM GTKMM SIGCXX])


# TODO: Just use the regular PREFIXSUFFIX_WXXFLAGS,
# (and use -I instead of -isystem in Makefile_tests.am)
# if this googletest problem is fixed:
# https://github.com/google/googletest/issues/533
MM_ARG_ENABLE_WARNINGS([PREFIXSUFFIX_GTEST_WXXFLAGS],
                       [-Wall],
                       [-pedantic -Wall -Wextra -Wshadow -Wformat-security -Wno-missing-field-initializers -DGSEAL_ENABLE],
                       [])

# i18n
IT_PROG_INTLTOOL([0.35])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

AC_SUBST([GETTEXT_PACKAGE], [prefixsuffix])
AC_DEFINE([GETTEXT_PACKAGE], [PACKAGE_TARNAME], [Define to the gettext package name.])

GLIB_GSETTINGS

# GResources
GLIB_COMPILE_RESOURCES=`$PKG_CONFIG --variable glib_compile_resources gio-2.0`
AC_SUBST(GLIB_COMPILE_RESOURCES)

AC_CONFIG_FILES([Makefile
                 po/Makefile.in
                 data/prefixsuffix.desktop.in
                 data/io.github.murraycu.PrefixSuffix.gschema.xml
                 help/Makefile
                 help/C/Makefile
                 docs/Makefile])

AC_OUTPUT
