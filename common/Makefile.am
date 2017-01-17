noinst_LIBRARIES = libcross.a

libcross_a_SOURCES = \
	anim.c \
	arch.c\
	artifact.c\
	button.c\
	cf_random.c \
	dialog.c \
	exp.c\
	friend.c\
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
	output_file.c \
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

CLEANFILES = loader.c
