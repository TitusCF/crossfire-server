
nodist_bin_SCRIPTS = crossloop.pl crossloop 
dist_bin_SCRIPTS = crossloop.web
nodist_pkglib_SCRIPTS = flushlocks
dist_pkglib_SCRIPTS = mktable.script add_throw.perl metaserver.pl
dist_noinst_SCRIPTS = ad2c.script mktable.script

EXTRA_DIST = flushlocks.tmpl 

CLEANFILES = flushlocks crossloop.pl crossloop

SED = sed

# For some reason, Sun's make fails if you try to use $<
# in the following entries.  Sun's man page does say $<
# is legal.  Easy enough to just replace them with real entries
# however.


flushlocks: flushlocks.tmpl
	$(SED) s%xxxx%$(libdir)%g < flushlocks.tmpl > flushlocks

crossloop.pl: crossloop.pl.tmpl
	$(SED) s%BINDIR%$(bindir)%g < crossloop.pl.tmpl > crossloop.pl

crossloop: crossloop.tmpl
	$(SED) s%BINDIR%$(bindir)%g < crossloop.pl.tmpl > crossloop
