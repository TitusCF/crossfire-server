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
	animations archetypes  bmaps bmaps.paths crossfire.0 \
	crossfire.1 faces attackmess image_info materials

CONF_FILES = settings motd dm_file forbid ban_file exp_table news rules
VAR_FILES = highscore banish_file bookarch temp.maps clockdata
EXTRA_DIST = README .collect-stamp $(CONF_FILES) treasures treasures.bld

helpdir=$(pkgdatadir)/help
dist_help_DATA = help/apply help/applymode \
	help/bind help/bowmode help/brace \
	help/cast help/drop help/dropall \
	help/golem help/invoke \
	help/keys help/killpets \
	help/listen help/maps \
	help/mark help/melee help/mode \
	help/motd help/mouse help/move \
	help/news help/output \
	help/output-count help/output-sync \
	help/party help/peaceful \
	help/petmode help/pickup help/quests \
	help/quit help/range help/rename \
	help/rules \
	help/save help/shout help/showpets \
	help/sort_inventory help/spells \
	help/statistics help/take help/traps \
	help/unbind help/usekeys \
	help/whereabouts

wizhelpdir=$(pkgdatadir)/wizhelp
dist_wizhelp_DATA = wizhelp/abil wizhelp/addexp \
	wizhelp/banish \
	wizhelp/create wizhelp/debug \
	wizhelp/dump wizhelp/dumpallarchtypes \
	wizhelp/dumpallmaps wizhelp/dumpallobjects \
	wizhelp/dumpbelow wizhelp/dumpfriendlyobjects \
	wizhelp/forget_spell wizhelp/free \
	wizhelp/goto wizhelp/hide wizhelp/insert_into \
	wizhelp/invisible \
	wizhelp/kick wizhelp/learn_special_prayer \
	wizhelp/learn_spell wizhelp/mon_aggr \
	wizhelp/nodm \
	wizhelp/nowiz wizhelp/overlay_save \
	wizhelp/patch wizhelp/plugin \
	wizhelp/pluglist wizhelp/plugout \
	wizhelp/possess \
	wizhelp/printlos wizhelp/remove \
	wizhelp/reset wizhelp/set_god \
	wizhelp/shutdown wizhelp/server_speed \
	wizhelp/spellreset wizhelp/ssdumptable wizhelp/stats \
	wizhelp/style_info wizhelp/summon \
	wizhelp/teleport wizhelp/time \
	wizhelp/toggle_shout \
	wizhelp/who wizhelp/wizcast wizhelp/wizhelp \
	wizhelp/wizpass

admdir=$(pkgdatadir)/adm
dist_adm_SCRIPTS =  \
	adm/map_info adm/map_expand.pl adm/mapslitter.pl adm/archsearch.pl \
	adm/collect_images.pl

MAINTAINERCLEANFILES = archetypes bmaps faces .collect-stamp

PERL = @PERL@

crossfire.0: bmaps.paths
	$(PERL) $(srcdir)/adm/collect_images.pl -png

.collect-stamp:
	touch .collect-stamp

collect:
	$(PERL) -I$(srcdir) collect.pl $(srcdir)/arch
	$(PERL) $(srcdir)/adm/collect_images.pl -png

archonly:
	$(PERL) -I$(srcdir) collect.pl $(srcdir)/arch ARCHONLY

archetypes: .collect-stamp
	make collect

faces: .collect-stamp
	make collect

animations: .collect-stamp
	make collect

bmaps.paths bmaps: .collect-stamp
	make collect

install-data-local: archetypes crossfire.0 faces animations collect.pl util.pl
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

	@for i in datafiles players unique-items template-maps maps; do \
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
	$(INSTALL_DATA) $(srcdir)/treasures.bld $(DESTDIR)$(pkgdatadir)/treasures


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
