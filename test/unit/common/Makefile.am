include $(top_srcdir)/check-fragment.am

CHECK_ROOT=../../
CHECK_FOLDER=unit/common
CHECK_PARENT_NAME=Unit

noinst_LIBRARIES = libstubs.a
libstubs_a_SOURCES=stubs_common.c
LDADD = $(top_builddir)/test/toolkit/libtoolkit_common.a $(top_builddir)/common/libcross.a libstubs.a @CHECK_LIBS@

$(top_builddir)/common/libcross.a:
	( cd $(top_builddir)/common ; make libcross.a )

$(top_builddir)/test/toolkit/libtoolkit_common.a:
	( cd $(top_builddir)/test/toolkit/; make libtoolkit_common.a)

# disabled tests, enable them when they have some real content
# check_anim check_button check_exp check_friend check_glue check_holy check_info check_init
# check_links check_logger check_los check_map
# check_player check_porting check_recipe check_re-cmp check_region

check_asset_load_write_SOURCES = check_asset_load_write.cpp
TESTS= check_arch check_image check_item check_living check_loader check_object check_path check_readable check_shstr check_time check_treasure check_utils
TESTS+= check_asset_load_write

DISTCLEANFILES=	.autorun.xml

check-local: check-html
clean-local: clean-unit
