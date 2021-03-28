#note all .a are here twice for compilation problems
AM_LIBTOOLFLAGS= --preserve-dup-deps
LINK = $(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
        --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) \
       $(LDFLAGS) -o $@

LDADD = $(top_builddir)/server/libserver.a \
        $(top_builddir)/common/libcross.a \
        $(top_builddir)/random_maps/librandom_map.a \
        $(top_builddir)/socket/libsocket.a \
        $(top_builddir)/types/libtypes.a \
        $(top_builddir)/types/libtypes.a \
        $(top_builddir)/test/toolkit/libtoolkit_common.a \
        @CHECK_LIBS@

$(top_builddir)/types/libtypes.a:
	( cd $(top_builddir)/types ; make libtypes.a )

# ../../logs/unit/server:
# 	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/server)

$(top_builddir)/common/libcross.a:
	( cd $(top_builddir)/common ; make libcross.a )

$(top_builddir)/socket/libsocket.a:
	( cd $(top_builddir)/socket ; make libsocket.a )

$(top_builddir)/random_maps/librandom_map.a:
	( cd $(top_builddir)/random_maps ; make librandom_map.a )

$(top_builddir)/server/libserver.a:
	( cd $(top_builddir)/server ; make libserver.a )

$(top_builddir)/test/toolkit/libtoolkit_common.a:
	( cd $(top_builddir)/test/toolkit/; make libtoolkit_common.a)

TESTS=check_1727944 check_2879249 check_free_objects check_weight_reduction check_883 check_duplicate_skills
#check_alchemy check_apply check_attack check_ban check_build_map check_c_chat check_c_misc check_c_move check_c_new check_c_object check_commands check_c_party check_c_range check_c_wiz check_disease check_egoitem check_gods check_hiscore check_init check_login check_main check_monster check_move check_pets check_player check_plugins check_resurrection check_rune check_shop check_skills check_skill_util check_spell_attack check_spell_effect check_spell_util check_swamp check_swap check_time check_timers check_weather check_win32 comet_perf


CHECK_ROOT=../../
CHECK_FOLDER=bugs/bugtrack
CHECK_PARENT_NAME=Bugs
include $(top_srcdir)/check-fragment.am

DISTCLEANFILES=	.autorun.xml

check-local: check-html
clean-local: clean-unit
