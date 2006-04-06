BUILT_SOURCES=../../logs/unit/common
noinst_LIBRARIES = libstubs.a
libstubs_a_SOURCES=stubs_common.c
INCLUDES = -I../../../include -I../../include
AM_CPPFLAGS = -DRESSOURCES=\"$(srcdir)/../../resources\" -DLOGDIR=\"$(srcdir)/../../logs\" -DSOURCE_ROOT=\"$(srcdir)/../../../\"
LDADD = ../../../common/libcross.a libstubs.a $(LIBDL) @CHECK_LIBS@

../../logs/unit/common:
	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/common)
../../../common/libcross.a:
	( cd ../../../common ; make libcross.a )
	

TESTS= check_anim check_arch check_button check_exp check_friend check_glue check_holy check_image check_info check_init check_item check_links check_living check_loader check_logger check_los check_map check_object check_path check_player check_porting check_quest check_readable check_recipe check_re-cmp check_region check_shstr check_time check_treasure check_utils

noinst_PROGRAMS=$(TESTS)

