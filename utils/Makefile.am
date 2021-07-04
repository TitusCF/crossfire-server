dist_noinst_SCRIPTS = mktable.script player_dl.html crossloop.web metaserver
dist_bin_SCRIPTS = cfdb_convert crossloop player_dl.pl

EXTRA_DIST =
AM_CPPFLAGS = -I$(top_srcdir)/include

CLEANFILES = crossloop

if MAPPER
noinst_PROGRAMS = crossfire-mapper
crossfire_mapper_SOURCES = mapper.c
crossfire_mapper_INCLUDES = -I ../include
crossfire_mapper_DEPENDENCIES = ../common/libcross.a
crossfire_mapper_LDADD = ../common/libcross.a $(GD_LIBS)
$(crossfire_mapper_DEPENDENCIES) :
	for DEP in $(crossfire_mapper_DEPENDENCIES); \
	do \
	  pushd `dirname $${DEP}` >/dev/null; \
	  $(MAKE) `basename $${DEP}`; \
	  popd >/dev/null; \
	done;
endif

