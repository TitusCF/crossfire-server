

CHECK_ROOT=../../
CHECK_FOLDER=unit/common
CHECK_PARENT_NAME=Unit
include $(top_srcdir)/check-fragment.am

noinst_LIBRARIES = libstubs.a
libstubs_a_SOURCES=stubs_common.c
LDADD = $(top_srcdir)/test/toolkit/libtoolkit_common.a $(top_srcdir)/common/libcross.a libstubs.a $(LIBDL) @CHECK_LIBS@

$(top_srcdir)/common/libcross.a:
	( cd $(top_srcdir)/common ; make libcross.a )
	
$(top_srcdir)/test/toolkit/libtoolkit_common.a:
	( cd $(top_srcdir)/test/toolkit/; make libtoolkit_common.a)

TESTS= check_anim check_arch check_button check_exp check_friend check_glue check_holy check_image check_info check_init check_item check_links check_living check_loader check_logger check_los check_map check_object check_path check_player check_porting check_quest check_readable check_recipe check_re-cmp check_region check_shstr check_time check_treasure check_utils


check-local: check-html
clean-local: clean-unit