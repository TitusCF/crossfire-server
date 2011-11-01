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

noinst_LIBRARIES = libcross.a

libcross_a_SOURCES = \
	anim.c \
	arch.c\
  artifact.c\
	button.c\
	dialog.c \
	exp.c\
	friend.c\
	glue.c\
	holy.c\
	info.c\
	image.c \
	init.c\
	item.c\
	languages.c\
	links.c\
	living.c\
	loader.l\
	logger.c\
	los.c\
	map.c\
	ob_methods.c \
	ob_types.c \
	object.c\
	path.c\
	porting.c\
	player.c\
	re-cmp.c\
	readable.c\
	recipe.c\
	region.c\
	shstr.c\
	stringbuffer.c\
	time.c\
	treasure.c\
	utils.c

EXTRA_DIST = README
AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\"
AM_LFLAGS = -i

proto:
	cproto -e -o ../include/libproto.h.bak $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(SOURCES) loader.c " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	sed -e "s/__signed/signed/g" -e "/inline/d" -e "/YY_BUFFER/d" -e "/yy_size_t/d" < ../include/libproto.h.bak > ../include/libproto.h
	chmod 644 ../include/libproto.h
	rm -f ../include/libproto.h.bak

CLEANFILES = loader.c
