bin_PROGRAMS = crossfire-config

crossfire_config_SOURCES = \
	devel.c

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

#Perhaps should uncomment to be sure crossfire has been compiled but.... Why?
#crossfire_config_LDADD = ../common/libcross.a
#../common/libcross.a:
#	( cd ../common ; make libcross.a )


# Check if everything is installed as it should
#installcheck-local:
#	$(bindir)/crossfire -o
