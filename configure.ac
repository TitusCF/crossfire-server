dnl Process this file with autoconf to produce a configure script.

AC_REVISION($Id$)
AC_INIT([crossfire], [1.5.0], [crossfire-devel@listserv.real-time.com])
AC_CONFIG_AUX_DIR(utils)
AC_CONFIG_SRCDIR([server/main.c])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([include/autoconf.h])

AC_PREFIX_DEFAULT(/usr/games/crossfire)

dnl we want a config.h file instead of -D options.

dnl Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC

case "$target" in
  alpha-dec-osf*)
    # If we are not using gcc, we want the ansi version of cc.
    if test -z "$GCC"; then
#      CFLAGS="$CFLAGS -std1 -warnprotos" # Not yet ready for warnprotos...
      CFLAGS="$CFLAGS -std1"
    fi
    ;;
  *)
    ;;
esac


AC_PROG_CPP
AM_PROG_LEX
AC_PROG_AWK

AC_ARG_ENABLE(dmalloc, [ --enable-dmalloc    Use the dmalloc library if available, may prevent plugins from working],
	use_dmalloc=yes, use_dmalloc=no)

dnl check for some other programs
AC_PATH_PROG(LATEX, latex)
AC_PATH_PROG(GZIP, gzip)
AC_PATH_PROG(GUNZIP, gunzip)
AC_PATH_PROG(PERL, perl)
AC_PATH_PROG(BASENAME, basename)

if test -n "$GZIP" -a -z "$GUNZIP" ; then
    echo "Found gzip, but not gunzip - setting GUNZIP to gzip -c";
    GUNZIP="$GZIP -c"
fi;

if test -z "$COMPRESS" ; then
	AC_PATH_PROG(COMPRESS, compress)
	AC_PATH_PROG(UNCOMPRESS, uncompress)
fi

AC_PATH_PROG(BZIP, bzip2)
AC_PATH_PROG(BUNZIP, bunzip2)

if test -n "$BZIP" -a -z "$BUNZIP" ; then
    echo "Found bzip2, but not bunzip2 - setting BUNZIP to bzip2 -c";
    BUNZIP="$BZIP -c"
fi;

if test -z "$COMPRESS" -a -z "$GZIP" -a -z "$BZIP" ; then
	echo "Unable to find either compress, bzip2,  or gzip - hope you don't plan on compressing";
	echo "any files.";
fi;

dnl nsl, socket may be needed for the X-windowing system, so check
dnl for them before before checking for X.
AC_CHECK_LIB(nsl, main)
AC_CHECK_LIB(socket, main)

dnl Checks for libraries.
dnl Start of X11 libraries
AC_PATH_XTRA

dnl Some of these checks are probably excessive.  Unfortunately, on
dnl some systems, to link in Xaw, Xt you need SM, which needs ICE.
dnl the order of the checks below is important
OLD_LD_FLAGS="$LDFLAGS"
LDFLAGS="$X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS"

AC_CHECK_LIB(X11, main, AC_DEFINE(HAVE_LIBX11,[],[Define if libX11 is available]) X11LIBS="$X11LIBS -lX11")

AC_CHECK_LIB(ICE, main, AC_DEFINE(HAVE_LIBICE,[],[Define if libICE is available]) X11LIBS="$X11LIBS -lICE")

AC_CHECK_LIB(SM, main, AC_DEFINE(HAVE_LIBSM,[],[Define if libSM is available]) X11LIBS="$X11LIBS -lSM", , $X11LIBS)

AC_CHECK_LIB(Xext, main,  AC_DEFINE(HAVE_LIBXEXT,[],[Define if libEXT is available]) X11LIBS="$X11LIBS -lXext", , -lX11)

AC_CHECK_LIB(Xt, main,  AC_DEFINE(HAVE_LIBXT,[],[Define if libXT is available]) X11LIBS="$X11LIBS -lXt", , $X11LIBS)

AC_CHECK_LIB(Xmu, main,  AC_DEFINE(HAVE_LIBXMU,[],[Define if libXMU is available]) X11LIBS="$X11LIBS -lXmu", ,$X11LIBS)

AC_CHECK_LIB(Xaw, main, AC_DEFINE(HAVE_LIBXAW,[],[Define if libXAW is available]) X11LIBS="-lXaw $X11LIBS", , $X11LIBS)

LDFLAGS="$OLD_LD_FLAGS"
X11LIBS="$X_LIBS $X11LIBS"

AC_CHECK_LIB(Xpm, main,  AC_DEFINE(HAVE_LIBXPM,[],[Define if libXpm is available]) X11LIBS="$X11LIBS -lXpm", , $X11LIBS )

AC_CHECK_LIB(m, main)

AC_CHECK_LIB(png, main,  AC_DEFINE(HAVE_LIBPNG,[],[Define if libpng is available]) X11LIBS="$X11LIBS -lpng", , $X11LIBS )

dnl Misc libraries.
AC_CHECK_LIB(crypt, main)
AC_CHECK_LIB(des, des_crypt)

dnl Gros - Changed this to support some external programs that do not like dmalloc (like Python).
dnl If you really need to make some memory debugging, uncomment the dmalloc line and comment out the dmalloclp one.

if eval "test x$use_dmalloc = xyes"; then
    AC_CHECK_LIB(dmalloc, main)
else
    AC_CHECK_LIB(dmalloclp, main)
fi

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h limits.h malloc.h strings.h sys/file.h sys/ioctl.h sys/time.h time.h unistd.h stddef.h stdlib.h sys/ttycom.h sys/termios.h crypt.h arpa/inet.h des.h)


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UID_T

AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_SETPGRP
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(gettimeofday mkdir mktime rmdir select socket strcspn strerror strspn strstr strtol strcasecmp strncasecmp snprintf setsid srandom getdtablesize srand48 srand sysconf scandir)

AC_MSG_CHECKING(how many args gettimeofday uses)
AC_CACHE_VAL(ac_cv_gettimeofday_args,
 [AC_TRY_COMPILE([#include <sys/time.h>
	#include <unistd.h>],
	[struct timeval tv; struct timezone tzp;
	gettimeofday(&tv, &tzp);],
	[ac_gettimeofday_args=2],
	[AC_TRY_COMPILE([#include <sys/time.h>
		#include <unistd.h>],
		[struct timeval tv; gettimeofday(&tv);],
		[ac_gettimeofday_args=1],
		[ac_gettimeofday_args=0])])
  ac_cv_gettimeofday_args=$ac_gettimeofday_args])

ac_gettimeofday_args=$ac_cv_gettimeofday_args
if test $ac_gettimeofday_args = 1 ; then
  AC_DEFINE(HAVE_GETTIMEOFDAY,[],[Define if gettimeofday is available])
  AC_MSG_RESULT(one argument)
elif test $ac_gettimeofday_args = 2 ; then
  AC_DEFINE(HAVE_GETTIMEOFDAY,[],[Define if gettimeofday is available])
  AC_DEFINE(GETTIMEOFDAY_TWO_ARGS,[],[Define if gettimeofday takes two arguments])
  AC_MSG_RESULT(two arguments)
else
  AC_MSG_RESULT(unknown)
fi

AC_SUBST(x_includes)
AC_SUBST(x_libraries)
AC_SUBST(no_x)
AM_CONDITIONAL(HAVE_X,test "x$no_x" = "x")
AC_SUBST(X11LIBS)
AC_SUBST(PERL)
AC_SUBST(BASENAME)

AC_SUBST(pkgstatedir,$localstatedir/$PACKAGE)
AC_SUBST(pkgconfdir,$sysconfdir/$PACKAGE)

AC_DEFINE_UNQUOTED(COMPRESS,"${COMPRESS}",[Path to the compress binary])
AC_DEFINE_UNQUOTED(UNCOMPRESS,"${UNCOMPRESS}",[Path to the uncompress binary])
AC_DEFINE_UNQUOTED(GZIP,"${GZIP}",[Path to the gzip binary])
AC_DEFINE_UNQUOTED(GUNZIP,"${GUNZIP}",[Path to the gunzip binary])
AC_DEFINE_UNQUOTED(BZIP,"${BZIP}",[Path to the bzip binary])
AC_DEFINE_UNQUOTED(BUNZIP,"${BUNZIP}",[Path to the bunzip binary])

#############################################
# Plugin configuration
AC_CHECK_LIB(dl, dlopen,[ cf_have_libdl=yes ])
AM_CONDITIONAL(HAVE_LIBDL,test "x$cf_have_libdl" = "xyes")

if test "x$cf_have_libdl" = "xyes" ; then
	###############
	# Python plugin
	CF_CHECK_PYTHON(
		[PLUGIN_PYTHON="plugin_python.la"],
		[AC_MSG_NOTICE([No Python found. Python plugin will not be built.])]
	)
	dnl *** Put other plugins configuration code here ***
else
	AC_MSG_NOTICE([No dl library found. Plugins will not be supported.])
fi
AM_CONDITIONAL(PYTHON_PLUGIN,test "x$PLUGIN_PYTHON" != "x")
AC_SUBST(PLUGIN_PYTHON)

AC_OUTPUT([Makefile
	crossedit/Makefile crossedit/doc/Makefile crossedit/include/Makefile
	crossedit/Cnv/Makefile crossedit/bitmaps/Makefile
	doc/Makefile doc/Developers/Makefile doc/spell-docs/Makefile
	doc/spoiler/Makefile doc/spoiler-html/Makefile
	doc/playbook/Makefile doc/playbook-html/Makefile
	doc/scripts/Makefile
	lib/Makefile random_maps/Makefile socket/Makefile server/Makefile 
	include/Makefile utils/Makefile lib/checkarch.pl
	lib/collect.pl utils/add_throw.perl utils/crossloop.tmpl utils/crossloop.pl.tmpl
	utils/metaserver.pl utils/crossloop.web utils/scores.pl
	common/Makefile plugin/Makefile devel/Makefile
	 ])
