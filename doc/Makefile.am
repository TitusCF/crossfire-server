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

SUBDIRS = scripts Developers playbook spoiler spell-docs spoiler-html playbook-html

dist_noinst_DATA = handbook.ps spoiler.ps
dist_noinst_SCRIPTS = include_html.pl
dist_man6_MANS = crossedit.man crossfire-server.man crossfire-config.man \
	crossloop.web.man crossloop.man

EXTRA_DIST = Crossedit.doc PlayerStats \
	README RunTimeCommands SurvivalGuide alchemy.doc \
	experience metaserver multigod plugins skills.doc \
	spellcasters_guide_to_runes

DTOP=$(top_srcdir)/$(PACKAGE)-doc
BASENAME = @BASENAME@

spoiler.ps:
	(cd spoiler; $(MAKE) spoiler)

handbook.ps:
	(cd playbook; $(MAKE) handbook)

crosslib.doc:
	(cd ../common; make doc)

# workaround for bug in automake 1.6.2 & 1.6.3 (& probably others)
uninstall-local:
	rm -f $(DESTDIR)$(man6dir)/crossedit.6*
	rm -f $(DESTDIR)$(man6dir)/crossfire.6*
	rm -f $(DESTDIR)$(man6dir)/crossfire-config.6*

archive: spoiler playbook
	$(mkinstalldirs) $(DTOP)
	cp $(EXTRA_DIST) $(DTOP)
	cp spoiler.ps handbook.ps $(DTOP)
	cp spell-docs/*.txt spell-docs/*.ps $(DTOP)
	(cd spoiler-html ; make archive )
	(cd playbook-html ; make archive )
	(cd $(DTOP)/..; gtar --exclude=CVS -cvhzf $(PACKAGE)-$(VERSION).doc.tar.gz `$(BASENAME) $(DTOP)`)
	$(RM) -rf $(DTOP)
