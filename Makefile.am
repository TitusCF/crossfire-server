XSLTENGINE= @XSLTENGINE@
CROSSEDIT = @CROSSEDIT@

if HAVE_CHECK
  UNIT_TEST = test
endif

SUBDIRS = common random_maps socket server include lib utils doc plugins devel $(UNIT_TEST) $(CROSSEDIT)

EXTRA_DIST = AUTHORS ChangeLog COPYING DEVELOPERS autogen.sh run_win32.bat

DISTCHECK_CONFIGURE_FLAGS = --localstatedir=$$dc_install_base/var --mandir=$$dc_install_base/man

archives: dist
	(cd doc; make archive)
	(cd lib; make archive)

proto:
	(cd common; make proto)
	(cd server; make proto)
	(cd plugins; make proto)
	@if [ "X$(NO_X)" != "Xyes" ]; then \
		(cd crossedit; make proto) \
	fi
