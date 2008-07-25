#
# Makefile for crossfire 0.95.0 and beyond
#
# This file will eventually be replaced by a configure script.  But for
# now, this simple makefile should do the job.
#
#   Copyright (C) 1992 Frank Tore Johansen
#   Copyright (C) 2001 Mark Wedel
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#   The author can be reached via e-mail to mwedel@sonic.net

plugindir=$(pkglibdir)/plugins

SUBDIRS = include

if PYTHON_PLUGIN
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

cfpython_la_LIBADD = @PYTHON_LIB@ @PY_LIBS@
cfpython_la_LDFLAGS = -module -avoid-version -rdynamic
AM_CPPFLAGS = -I$(srcdir)/include -I$(top_srcdir)/plugins/common/include -I$(top_srcdir)/include @PY_INCLUDES@

proto:
	cproto -D__CEXTRACT__ -o include/cfpython_proto.h $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(cfpython_la_CPSOURCES) " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	chmod 644 include/cfpython_proto.h
