SUBDIRS = include

if CFCITYBELL_PLUGIN
plugin_LTLIBRARIES = cfcitybell.la
else
plugin_LTLIBRARIES =
endif


plugindir=$(pkglibdir)/plugins
noinst_HEADERS = include/cfcitybell.h	../common/include/plugin_common.h
cfcitybell_la_SOURCES = cfcitybell.c\
	../common/plugin_common.c

cfcitybell_la_LIBADD =
cfcitybell_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/../common/include -I$(top_srcdir)/include

