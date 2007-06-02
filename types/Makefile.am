AUTOMAKE_OPTIONS := subdir-objects

noinst_LIBRARIES = libtypes.a
libtypes_a_SOURCES = altar/altar.c \
armour_improver/armour_improver.c \
arrow/arrow.c \
book/book.c \
button/button.c \
check_inv/check_inv.c \
clock/clock.c \
container/container.c \
converter/converter.c \
deep_swamp/deep_swamp.c \
director/director.c \
exit/exit.c \
hole/hole.c \
identify_altar/identify_altar.c \
lamp/lamp.c \
lighter/lighter.c \
pedestal/pedestal.c \
player_mover/player_mover.c \
power_crystal/power_crystal.c \
rune/rune.c \
shop_inventory/shop_inventory.c \
shop_mat/shop_mat.c \
sign/sign.c \
spell_effect/spell_effect.c \
spinner/spinner.c \
thrown_object/thrown_object.c \
transport/transport.c \
trap/trap.c \
trapdoor/trapdoor.c \
trigger/trigger.c \
trigger_altar/trigger_altar.c \
trigger_button/trigger_button.c \
trigger_pedestal/trigger_pedestal.c \
common/common_apply.c \
common/describe.c \
common/projectile.c \
legacy/apply.c \
legacy/legacy_describe.c \
legacy/process.c

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

proto:
	cproto -D__CEXTRACT__ -o ../include/typesproto.h $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(SOURCES) " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	chmod 644 ../include/typesproto.h
