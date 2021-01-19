pkgstatedir = @pkgstatedir@
pkgconfdir = @pkgconfdir@

dist_noinst_SCRIPTS = collect util.pl
dist_pkgdata_DATA = \
	arch/artifacts \
	arch/attackmess \
	arch/formulae \
	arch/image_info \
	arch/materials \
	arch/messages \
	arch/races \
	def_help \
	crossfire.arc \
	crossfire.trs \
	crossfire.face \
	crossfire.tar

CONF_FILES = \
	config/ban_file \
	config/dm_file \
	config/exp_table \
	config/forbid \
	config/metaserver2 \
	config/motd \
	config/news \
	config/rules \
	config/settings \
	config/stat_bonus

VAR_FILES = highscore banish_file bookarch temp.maps clockdata
EXTRA_DIST = README .collect-stamp $(CONF_FILES)

helpdir=$(pkgdatadir)/help
dist_help_DATA = help/*.en help/*.fr

wizhelpdir=$(pkgdatadir)/wizhelp
dist_wizhelp_DATA = wizhelp/*.en wizhelp/*.fr

admdir=$(pkgdatadir)/adm
dist_adm_SCRIPTS = adm/archsearch.pl adm/collect_images.pl

i18ndir=$(pkgdatadir)/i18n
dist_i18n_DATA = i18n/messages.en \
	i18n/messages.fr

MAINTAINERCLEANFILES = .collect-stamp \
	crossfire.arc crossfire.trs crossfire.face crossfire.tar

PERL = @PERL@

# This target is used to prevent collecting a second time after things have
# already been collected. To force a re-collect, use 'do-collect' target.
.collect-stamp:
	touch .collect-stamp

do-collect:
	rm -f .collect-stamp
	make collect

# This target is just used for making do-collect actually force a re-collect.
collect: crossfire.arc

crossfire.arc: .collect-stamp
	rm -f crossfire.arc crossfire.face crossfire.trs crossfire.tar
	../server/crossfire-server -data . \
	  -pack-assets archs crossfire.arc \
	  -pack-assets faces crossfire.face \
	  -pack-assets treasures crossfire.trs \
	  -pack-assets images crossfire.tar

crossfire.trs: crossfire.arc

crossfire.face: crossfire.arc

crossfire.tar: crossfire.arc

install-data-local: util.pl
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgconfdir)
	for i in $(CONF_FILES); do \
	    if [ ! -f $(DESTDIR)$(pkgconfdir)/`basename $$i` ]; then \
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
	$(AMTAR) -ch --exclude=dev --exclude=.svn -f ${PACKAGE}-arch-${VERSION}.tar arch

libArch:
	tar cvf A`perl puredate.pl`.tar arch treasures
	gzip A`perl puredate.pl`.tar
