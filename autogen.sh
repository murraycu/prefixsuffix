#! /bin/sh -e
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.
(
  cd "$srcdir" &&
  mm-common-prepare --copy --force &&
  autopoint --force &&
  AUTOPOINT='intltoolize --automake --copy' autoreconf --force --install
) || exit
test -n "$NOCONFIGURE" || "$srcdir/configure" --enable-maintainer-mode "$@"
