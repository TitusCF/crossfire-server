
noinst_LIBRARIES = libCnv.a
libCnv_a_SOURCES = CnvUtil.c CnvBrowse.c CnvNotify.c CnvMenu.c CnvFiles.c\
	CnvPath.c CnvPrompt.c Cnv.h config.h

EXTRA_DIST = excloff.xbm exclon.xbm flagoff.xbm flagon.xbm notify.xbm prompt.xbm\
	submenu.xbm Makefile.in README

AM_CPPFLAGS = -I$(srcdir)/../include -I$(top_srcdir)/include

EXTRA_PROGRAMS = test 
test_SOURCES = test.c
test_LDADD = libCnv.a $(X11LIBS)
