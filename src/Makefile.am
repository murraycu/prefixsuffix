gladedir = $(datadir)/prefixsuffix/glade/

# Let the code know where the glade dir is:
INCLUDES =  -DPREFIXSUFFIX_GLADEDIR=\""$(gladedir)/"\"

bin_PROGRAMS = prefixsuffix

#List of source files needed to build the executable:
prefixsuffix_SOURCES = main.cc mainwindow.h mainwindow.cc
EXTRA_DIST = prefixsuffix-logo.png prefixsuffix.glade

CXXFLAGS = @CXXFLAGS@ $(DEPS_CFLAGS)
LDADD = $(DEPS_LIBS)

# Install the glade file:
# The glade prefix of glade_DATA seems to cause it to be installed in <appname>/glade/
# Because that's how gladedir is defined.
glade_DATA = prefixsuffix.glade
EXTRA_DIST = $(glade_DATA) 

