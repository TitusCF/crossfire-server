pkglibexec_PROGRAMS = random_map

random_map_SOURCES = main.c

noinst_LIBRARIES = librandom_map.a

librandom_map_a_SOURCES = \
	decor.c \
	door.c \
	exit.c \
	expand2x.c \
	floor.c \
	maze_gen.c \
	monster.c \
	random_map.c \
	reader.l \
	rogue_layout.c \
	room_gen_onion.c \
	room_gen_spiral.c \
	snake.c \
	special.c \
	square_spiral.c \
	style.c  \
	treasure.c \
	wall.c

EXTRA_DIST = \
	README \
	main.c

AM_CPPFLAGS = -I$(top_srcdir)/include -I${top_srcdir}/include/random_maps

LEX_OUTPUT_ROOT = lex.rmap
AM_LFLAGS = -Prmap -i

random_map_LDADD = ../common/libcross.a librandom_map.a

CLEANFILES = reader.c
