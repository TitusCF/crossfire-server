


CHECK_ROOT=../../
CHECK_FOLDER=unit/crossedit
CHECK_PARENT_NAME=Unit
include $(top_srcdir)/check-fragment.am
LDADD = ../../../common/libcross.a  ../../../crossedit/Cnv/libCnv.a ../../../crossedit/libcrossedit.a $(LIBDL) @CHECK_LIBS@

../../logs/unit/crossedit:
	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/crossedit)
../../../common/libcross.a:
	( cd ../../../common ; make libcross.a )
../../../crossedit/libcrossedit.a:
	( cd ../../../crossedit ; make libcrossedit.a )
../../../crossedit/Cnv/libCnv.a:
	( cd ../../../crossedit/Cnv ; make libCnv.a )

TESTS= check_App check_Attr check_Bitmaps check_CrEdit check_CrFace check_CrList check_crossedit check_CrUtil check_Edit check_MapAttr check_png check_Str check_stubs check_xutil


check-local: check-html
clean-local: clean-unit