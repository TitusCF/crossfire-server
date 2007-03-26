include $(top_srcdir)/check-fragment.am

CHECK_ROOT=../../
CHECK_FOLDER=unit/crossedit
CHECK_PARENT_NAME=Unit

LDADD = $(top_builddir)/common/libcross.a  $(top_builddir)/crossedit/Cnv/libCnv.a $(top_builddir)/crossedit/libcrossedit.a $(LIBDL) @CHECK_LIBS@

../../logs/unit/crossedit:
	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/crossedit)

$(top_builddir)/common/libcross.a:
	( cd $(top_builddir)/common ; make libcross.a )

$(top_builddir)/crossedit/libcrossedit.a:
	( cd $(top_builddir)/crossedit ; make libcrossedit.a )

$(top_builddir)/crossedit/Cnv/libCnv.a:
	( cd $(top_builddir)/crossedit/Cnv ; make libCnv.a )

TESTS= check_App check_Attr check_Bitmaps check_CrEdit check_CrFace check_CrList check_crossedit check_CrUtil check_Edit check_MapAttr check_png check_Str check_stubs check_xutil

clean-local:
	$(RM) -f .autorun.xml

check-local: check-html
clean-local: clean-unit
