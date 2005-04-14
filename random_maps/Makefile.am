#
# Makefile for crossfire 0.95.0 and beyond
#
# This file will eventually be replaced by a configure script.  But for
# now, this simple makefile should do the job.
#
#   Copyright (C) 1992 Frank Tore Johansen
#   Copyright (C) 1998 Mark Wedel
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
#   The author can be reached via e-mail to mark@pyramid.com


pkglib_PROGRAMS = random_map

random_map_SOURCES = standalone.c

noinst_LIBRARIES = librandom_map.a

librandom_map_a_SOURCES = random_map.c \
	room_gen_onion.c \
        room_gen_spiral.c \
	maze_gen.c \
	reader.l \
	floor.c \
	wall.c \
	monster.c \
	door.c \
	decor.c \
	exit.c \
	treasure.c \
	special.c \
	style.c  \
	rogue_layout.c \
        snake.c \
        square_spiral.c \
	expand2x.c \
	maze_gen.h random_map.h room_gen.h

EXTRA_DIST = maze_gen.h random_map.h room_gen.h\
	standalone.c rproto.h expand2x.h Notes

AM_CPPFLAGS = -I$(top_srcdir)/include

LEX_OUTPUT_ROOT = lex.rmap
AM_LFLAGS = -Prmap -i

random_map_LDADD = ../common/libcross.a librandom_map.a

proto:
	cproto -e -o rproto.h.bak $(CPPFLAGS) -I. $(AM_CPPFLAGS) \
		`echo $(librandom_map_a_SOURCES) | $(AWK) -v RS=" " '/.c$$/ { print }'`
	sed -e "s/#if __STDC__/#ifdef __STDC__/" -e "s/__signed/signed/g" -e "/__inline/d" -e "/YY_BUFFER/d" < rproto.h.bak >rproto.h
	chmod 644 rproto.h
	rm -f rproto.h.bak

