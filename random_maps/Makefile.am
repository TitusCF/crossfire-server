pkglibexec_PROGRAMS = random_map

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

CLEANFILES = reader.c
