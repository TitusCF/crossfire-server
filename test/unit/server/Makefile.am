
LDADD = ../../../common/libcross.a ../../../socket/libsocket.a ../../../random_maps/librandom_map.a ../../../server/libserver.a $(LIBDL) @CHECK_LIBS@

../../logs/unit/server:
	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/server)
../../../common/libcross.a:
	( cd ../../../common ; make libcross.a )
../../../socket/libsocket.a:
	( cd ../../../socket ; make libsocket.a )
../../../random_maps/librandom_map.a:
	( cd ../../../random_maps ; make librandom_map.a )
../../../server/libserver.a:
	( cd ../../../server ; make libserver.a )

TESTS=check_alchemy check_apply check_attack check_ban check_build_map check_c_chat check_c_misc check_c_move check_c_new check_c_object check_commands check_c_party check_c_range check_c_wiz check_daemon check_disease check_egoitem check_gods check_hiscore check_init check_login check_main check_monster check_move check_pets check_player check_plugins check_resurrection check_rune check_shop check_skills check_skill_util check_spell_attack check_spell_effect check_spell_util check_swamp check_swap check_time check_timers check_weather check_win32


CHECK_ROOT=../../
CHECK_FOLDER=unit/server
CHECK_PARENT_NAME=Unit
include $(top_srcdir)/check-fragment.am

check-local: check-html
clean-local: clean-unit