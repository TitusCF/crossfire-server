#
# Makefile for crossfire 0.95.0 and beyond
#
# This file will eventually be replaced by a configure script.  But for
# now, this simple makefile should do the job.
#
#   Copyright (C) 2001 Mark Wedel & Crossfire Development Team
#   Copyright (C) 1992 Frank Tore Johansen
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
#   The authors can be reached via e-mail to crossfire-devel@real-time.com

pkgstatedir = @pkgstatedir@
pkgconfdir = @pkgconfdir@

dist_noinst_SCRIPTS = collect.pl util.pl checkarch.pl
dist_pkgdata_DATA = artifacts def_help formulae  messages races smooth\
	animations archetypes  bmaps.paths crossfire.0 \
	crossfire.1 faces attackmess image_info materials

CONF_FILES = settings metaserver2 motd dm_file forbid ban_file exp_table news rules stat_bonus
VAR_FILES = highscore banish_file bookarch temp.maps clockdata
EXTRA_DIST = README .collect-stamp $(CONF_FILES) treasures.bld

helpdir=$(pkgdatadir)/help
dist_help_DATA = help/*.en help/*.fr

wizhelpdir=$(pkgdatadir)/wizhelp
dist_wizhelp_DATA = wizhelp/*.en wizhelp/*.fr

admdir=$(pkgdatadir)/adm
dist_adm_SCRIPTS =  \
	adm/map_info adm/map_expand.pl adm/mapslitter.pl adm/archsearch.pl \
	adm/collect_images.pl

i18ndir=$(pkgdatadir)/i18n
dist_i18n_DATA = i18n/messages.en \
	i18n/messages.fr

MAINTAINERCLEANFILES = archetypes faces .collect-stamp \
	animations smooth treasures.bld bmaps.paths crossfire.0 crossfire.1

PERL = @PERL@

crossfire.0: bmaps.paths
	$(PERL) $(srcdir)/adm/collect_images.pl -src $(srcdir) -png

.collect-stamp:
	touch .collect-stamp

collect:
	$(PERL) -I$(srcdir) collect.pl $(builddir)/arch
	$(PERL) $(srcdir)/adm/collect_images.pl -src $(srcdir) -png

archonly:
	$(PERL) -I$(srcdir) collect.pl $(builddir)/arch ARCHONLY

archetypes faces animations bmaps.paths smooth: .collect-stamp
	touch .collect-stamp
	$(MAKE) collect

install-data-local: archetypes crossfire.0 faces animations smooth collect.pl util.pl
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgconfdir)
	for i in $(CONF_FILES); do \
	    if [ ! -f $(DESTDIR)$(pkgconfdir)/$$i ]; then \
		    echo "Installing $$i"; \
		    $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(pkgconfdir); \
	    fi \
	done

	$(mkinstalldirs) $(DESTDIR)$(pkgstatedir)
	for i in $(VAR_FILES) ; do \
		touch $(DESTDIR)$(pkgstatedir)/$$i ; \
	done

	@for i in account datafiles players unique-items template-maps maps; do \
	    if [ ! -d $(DESTDIR)$(pkgstatedir)/$$i ]; then \
		echo "Creating directory $(pkgstatedir)/$$i"; \
		$(mkinstalldirs) $(DESTDIR)$(pkgstatedir)/$$i; \
	    fi; \
	done
# Special case above because the name we install to is different
# than the name of the file
	if [ ! -d $(DESTDIR)$(pkgdatadir)/$$i ]; then \
		echo "Creating directory $(pkgdatadir)/$$i"; \
		$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$i; \
	fi; \
	$(INSTALL_DATA) $(builddir)/treasures.bld $(DESTDIR)$(pkgdatadir)/treasures


# Uninstall unchanged config/var files so make distcheck is happy
# but leave changed file, so no configuration/game state is lost
uninstall-local:
	-for i in $(CONF_FILES); do \
		if test ! -f $(DESTDIR)$(pkgconfdir)/$$i ; then \
			continue ; \
		fi ; \
		if diff -q $(srcdir)/$$i $(DESTDIR)$(pkgconfdir)/$$i ; then \
			rm -f $(DESTDIR)$(pkgconfdir)/$$i ; \
		fi ; \
	done
	-for i in $(VAR_FILES) ; do \
		if test ! -f $(DESTDIR)$(pkgstatedir)/$$i ; then \
			continue ;\
		fi ; \
		if test `wc -c $(DESTDIR)$(pkgstatedir)/$$i|$(AWK) '{ print $$1 }'` -eq "0" ; then \
			rm -f $(DESTDIR)$(pkgstatedir)/$$i ; \
		fi ; \
	done


# This block makes the archive of the arch directory.
archive::
	$(AMTAR) -cvh --exclude=dev --exclude=.svn -f $(PACKAGE)-$(VERSION)-arch.tar arch
	$(GZIP) -cv9 $(PACKAGE)-$(VERSION)-arch.tar > ../$(PACKAGE)-$(VERSION)-arch.tar.gz
	$(BZIP) -cv --repetitive-best $(PACKAGE)-$(VERSION)-arch.tar > ../$(PACKAGE)-$(VERSION)-arch.tar.bz2
	$(RM) $(PACKAGE)-$(VERSION)-arch.tar

libArch:
	tar cvf A`perl puredate.pl`.tar arch treasures
	gzip A`perl puredate.pl`.tar
