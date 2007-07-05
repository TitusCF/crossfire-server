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
plugin_LTLIBRARIES = cfanim.la
cfanim_la_SOURCES = cfanim.c\
	../common/plugin_common.c

cfanim_la_CPSOURCES = cfanim.c

SUBDIRS=include

cfanim_la_LIBADD =
cfanim_la_LDFLAGS = -module -avoid-version -Xlinker -export-dynamic -rdynamic -Xlinker -Bsymbolic
AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/../common/include -I$(top_srcdir)/include

proto:
	cproto -D__CEXTRACT__ -o include/cfanim_proto.h $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(cfanim_la_CPSOURCES) " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	chmod 644 include/cfanim_proto.h
