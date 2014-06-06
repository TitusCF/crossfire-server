plugindir=$(pkglibdir)/plugins
plugin_LTLIBRARIES = cfanim.la
cfanim_la_SOURCES = cfanim.c\
	../common/plugin_common.c

cfanim_la_CPSOURCES = cfanim.c

SUBDIRS=include

cfanim_la_LIBADD =
cfanim_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/../common/include -I$(top_srcdir)/include
