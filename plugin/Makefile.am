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
noinst_HEADERS = include/plugin_python.h
if PYTHON_PLUGIN
plugin_LTLIBRARIES = plugin_python.la
else
plugin_LTLIBRARIES = 
endif
plugin_python_la_SOURCES = plugin_python.c
plugin_python_la_LIBADD = @PYTHON_LIB@ @PY_LIBS@
plugin_python_la_LDFLAGS = -module -avoid-version
AM_CPPFLAGS = @PY_INCLUDES@ -I$(srcdir)/include -I$(top_srcdir)/include
