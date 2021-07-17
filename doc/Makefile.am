SUBDIRS = scripts Developers playbook spoiler spell-docs spoiler-html playbook-html

dist_noinst_DATA = handbook.ps spoiler.ps
dist_noinst_SCRIPTS = include_html.pl
dist_man6_MANS = crossfire-server.man \
	crossloop.web.man crossloop.man

EXTRA_DIST = README \
	commands.txt       \
	media-tags.txt     plugins.txt   \
	runes-guide.txt    skills.txt         stats.txt     \
	style-guide.txt    survival-guide.txt

DTOP=$(top_srcdir)/$(PACKAGE)-doc
BASENAME = @BASENAME@

spoiler.ps:
	(cd spoiler; $(MAKE) spoiler)

handbook.ps:
	(cd playbook; $(MAKE) handbook)

spell-docs:
	(cd spell-docs; $(MAKE) spell-docs)

crosslib.doc:
	(cd ../common; make doc)

archive: spoiler playbook spell-docs
	$(mkinstalldirs) $(DTOP)
	cp $(EXTRA_DIST) $(DTOP)
	cp spoiler.ps handbook.ps $(DTOP)
	cp `find spell-docs/*.txt` `find spell-docs/*.ps` $(DTOP)
	(cd spoiler-html ; make archive )
	(cd playbook-html ; make archive )
	(cd $(DTOP)/..; gtar --exclude=CVS -cvhzf $(PACKAGE)-$(VERSION).doc.tar.gz `$(BASENAME) $(DTOP)`)
	$(RM) -rf $(DTOP)
