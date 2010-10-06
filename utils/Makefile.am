nodist_bin_SCRIPTS = crossloop.pl crossloop player_dl.pl
dist_bin_SCRIPTS = crossloop.web
nodist_pkglib_SCRIPTS =
dist_pkglib_SCRIPTS = mktable.script add_throw.perl metaserver.pl
dist_noinst_SCRIPTS = ad2c.script mktable.script player_dl.html cfmeta-rrd

EXTRA_DIST =
AM_CPPFLAGS = -I$(top_srcdir)/include

CLEANFILES = crossloop.pl crossloop

SED = sed

# For some reason, Sun's make fails if you try to use $<
# in the following entries.  Sun's man page does say $<
# is legal.  Easy enough to just replace them with real entries
# however.

crossloop.pl: crossloop.pl.tmpl
	$(SED) s%BINDIR%$(bindir)%g < crossloop.pl.tmpl > crossloop.pl

crossloop: crossloop.tmpl
	$(SED) s%BINDIR%$(bindir)%g < crossloop.tmpl > crossloop

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

