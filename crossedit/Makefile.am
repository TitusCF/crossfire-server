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
#   The author can be reached via e-mail to mwedel@scruz.net

bin_PROGRAMS = crossedit
noinst_LIBRARIES = libcrossedit.a
crossedit_SOURCES = \
	crossedit.c 
libcrossedit_a_SOURCES = \
	Attr.c \
	MapAttr.c \
	CrFace.c \
	CrEdit.c \
	CrList.c \
	CrUtil.c \
	Edit.c \
	App.c \
	Bitmaps.c \
	Str.c \
	xutil.c \
    stubs.c \
	CrFace.h \
	CrFaceP.h \
	CrEdit.h \
	CrEditP.h \
	CrList.h \
	CrListP.h \
	CrUtil.h \
	App.h \
	Edit.h \
	Bitmaps.h \
	Str.h \
	Attr.h \
	MapAttr.h \
	Crossedit.ad.h \
	Defines.h  \
	proto.h

noinst_HEADERS = png.c
EXTRA_DIST = README TODO Crossedit.ad Crossedit.ad.h 

SUBDIRS = include Cnv bitmaps doc

crossedit_LDADD = ../common/libcross.a libcrossedit.a Cnv/libCnv.a $(X11LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(srcdir)/include -I$(srcdir)/Cnv \
	      -DDATADIR=\"$(pkgdatadir)\" -DLIBDIR=\"$(pkglibdir)\" \
	      -DLOCALDIR=\"$(pkgstatedir)\"

../common/libcross.a:
	( cd ../common ; make libcross.a )

Crossedit.ad.h: Crossedit.ad
	-../utils/ad2c.script Crossedit.ad > Crossedit.ad.h

proto:	
	cproto -e -o /proto.h -I. $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(SOURCES) " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	chmod 644 proto.h
