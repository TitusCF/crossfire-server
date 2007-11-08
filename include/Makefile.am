
noinst_HEADERS = \
commands.h     libproto.h     sounds.h	     attack.h       config.h \
living.h       player.h       define.h       loader.h       race.h \
material.h     spells.h       face.h         logger.h       re-cmp.h \
sproto.h       artifact.h     map.h          recipe.h \
treasure.h     global.h       newclient.h    path.h         shstr.h \
tod.h          god.h          newserver.h    xdir.h \
book.h         includes.h     object.h       skills.h	    sockproto.h \
plugin.h       timers.h       typesproto.h   ob_methods.h   ob_types.h \
svnversion.h   version.h

BUILT_SOURCES = svnversion.h

CLEANFILES = svnversion.h

svnversion.h: FORCE
	OUTPUT_DATA='/* Auto-generated at build time. */'; \
	if test -n "$(SVNVERSION)" -a -d .svn; then \
		OUTPUT_DATA=`echo "$$OUTPUT_DATA"; echo -n '#define SVN_REV "'; $(SVNVERSION) -n`'"'; \
	fi; \
	if test ! -e svnversion.h; then \
		echo "$$OUTPUT_DATA" > svnversion.h; \
	elif test "$$OUTPUT_DATA" != "`cat svnversion.h`"; then \
		echo "$$OUTPUT_DATA" > svnversion.h; \
	fi

FORCE:

