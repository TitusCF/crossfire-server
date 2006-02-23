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

if PERL_PLUGIN
noinst_HEADERS = ../common/include/plugin_common.h
plugin_LTLIBRARIES = cfperl.la
cfperl_la_SOURCES = cfperl.xs ../common/plugin_common.c

cfperl_la_LIBADD = $(PERLLIB)
cfperl_la_LDFLAGS = -module -avoid-version -Xlinker -export-dynamic -rdynamic -Xlinker -Bsymbolic
AM_CPPFLAGS = -I$(srcdir)/include -I../common/include -I$(top_srcdir)/include $(PERLFLAGS)

SUFFIXES = .xs

.xs.c: typemap
	$(PERL) -MExtUtils::Embed -e xsinit -- -std cf
	$(PERL) @PERLPRIVLIBEXP@/ExtUtils/xsubpp -C++ -typemap @PERLPRIVLIBEXP@/ExtUtils/typemap -typemap typemap -prototypes cfperl.xs >$@

endif

proto:
	cproto -D__CEXTRACT__ -o include/plugin_template_proto.h $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(SOURCES) " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	chmod 644 include/plugin_template_proto.h

