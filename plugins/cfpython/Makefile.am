plugindir=$(pkglibdir)/plugins

SUBDIRS = include

if PYTHON_PLUGIN
noinst_PROGRAMS = pyi-generate
plugin_LTLIBRARIES = cfpython.la
else
plugin_LTLIBRARIES =
endif
cfpython_la_SOURCES = cfpython.c\
	cfpython_archetype.c\
	cfpython_object.c\
	cfpython_map.c\
	cfpython_party.c\
	cfpython_region.c\
	cjson.c\
	../common/plugin_common.c\
	../common/hashtable.c

cfpython_la_CPSOURCES = cfpython.c\
	cfpython_archetype.c\
	cfpython_object.c\
	cfpython_party.c\
	cfpython_region.c\
	cfpython_map.c

cfpython_la_LIBADD = @PYTHON_LIBS@
cfpython_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(top_srcdir)/plugins/common/include -I$(top_srcdir)/include @PYTHON_CFLAGS@

pyi_generate_SOURCES = pyi-generate.cpp ${cfpython_la_SOURCES}
pyi_generate_INCLUDES = -I ../include
pyi_generate_CPPFLAGS = $(AM_CPPFLAGS) -O0 -g
pyi_generate_LDFLAGS = @PYTHON_LIBS@
