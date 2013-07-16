#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([crossfire], [1.70.trunk], [crossfire-devel@listserv.real-time.com])
AC_REVISION([$Id$])
AC_CONFIG_SRCDIR([server/main.c])
AC_CONFIG_AUX_DIR([utils])
AC_CONFIG_MACRO_DIR([macros])
AC_CONFIG_HEADERS([include/autoconf.h])
AC_PREFIX_DEFAULT([/usr/games/crossfire])
AM_INIT_AUTOMAKE

# we want a config.h file instead of -D options.

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AM_PROG_LEX

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

CF_CHECK_VISIBILITY()

AC_ARG_ENABLE(dmalloc,
              [AS_HELP_STRING([--enable-dmalloc], [Use the dmalloc library if available, may prevent plugins from working])],,
              enable_dmalloc=no)

AC_ARG_WITH(python,
            [AS_HELP_STRING([--with-python=dir], [Directory to look for python headers/library (default=standard system location)])],
            PYTHON_HOME="$withval")

AC_ARG_ENABLE(mapper,
            [AS_HELP_STRING([--enable-mapper], [Whether or not to build the crossfire-mapper utility (default=no; requires libgd)])],,
            enable_mapper=no)

AC_ARG_ENABLE(cfrhg,
            [AS_HELP_STRING([--enable-cfrhg], [Whether or not to build the random house generator plugin (default=no)])],,
            enable_cfrhg=no)

# check for some other programs
AC_PATH_PROG(LATEX, latex)
AC_PATH_PROG(GZIP, gzip)
AC_PATH_PROG(GUNZIP, gunzip)
AC_PATH_PROG(TAR, tar)
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

# Check if svnversion is avaliable to get the rev number
AC_PATH_PROG(SVNVERSION, svnversion)

# Check for some library functions
AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([pow], [m])
AC_SEARCH_LIBS([sqrt], [m])

# Misc libraries.
AC_SEARCH_LIBS([crypt], [crypt])
AC_CHECK_LIB(des, des_crypt)

AC_CHECK_LIB(z, compress)
AC_CHECK_LIB(pthread, pthread_create)
LIBCURL_CHECK_CONFIG()

# For utils/mapper
cf_make_mapper=no
if eval "test x$enable_mapper = xyes"; then
    AC_PATH_PROG(GDLIB_CONFIG,gdlib-config)
    if test -n "$GDLIB_CONFIG"; then
        GD_LIBS="`$GDLIB_CONFIG --ldflags` `$GDLIB_CONFIG --libs` -lgd"
        AC_SUBST(GD_LIBS, $GD_LIBS)
        cf_make_mapper=yes
    else
        AC_MSG_WARN([Cannot find gdlib-config; will not build crossfire-mapper.])
    fi
fi
AM_CONDITIONAL(MAPPER, test x$cf_make_mapper = xyes)

# Random house generator
AM_CONDITIONAL(CFRHG_PLUGIN, test x$enable_cfrhg = xyes)

LIBS="$LIBS $LIBCURL"
CPPFLAGS="$CPPFLAGS $LIBCURL_CPPFLAGS"

# Gros - Changed this to support some external programs that do not like dmalloc (like Python).
# If you really need to make some memory debugging, uncomment the dmalloc line and comment out the dmalloclp one.

if eval "test x$enable_dmalloc = xyes"; then
    AC_CHECK_LIB(dmalloc, main)
else
    AC_CHECK_LIB(dmalloclp, main)
fi

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h stddef.h stdlib.h strings.h sys/file.h sys/ioctl.h sys/time.h time.h unistd.h sys/ttycom.h termios.h sys/termios.h crypt.h des.h zlib.h pthread.h curl/curl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_TYPE_UID_T

AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_SETPGRP
AC_TYPE_SIGNAL
AC_FUNC_STRERROR_R
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getdtablesize gettimeofday mkdir mktime rmdir select socket strcasecmp strcspn strerror strncasecmp strspn strstr strtol snprintf setsid srandom srand48 srand sysconf scandir getaddrinfo getnameinfo])

# alphasort can not be added to AC_CHECK_FUNCS because
AC_MSG_CHECKING(checking if alphasort is defined)
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <dirent.h>]], [[alphasort;]])],[ac_cv_func_alphasort=yes],[ac_cv_func_alphasort=no])
  if test "$ac_cv_func_alphasort" = yes; then
    AC_DEFINE(HAVE_ALPHASORT, 1, [Define if you have the alphasort() function declaration.])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi



AC_MSG_CHECKING(how many args gettimeofday uses)
AC_CACHE_VAL(ac_cv_gettimeofday_args,
 [AC_TRY_COMPILE([#include <sys/time.h>
	#include <unistd.h>],
	[struct timeval tv; struct timezone tzp;
	gettimeofday(&tv, &tzp);],
	[ac_gettimeofday_args=2],
		[AC_TRY_COMPILE([#include <sys/time.h>
		#include <unistd.h>],
		[struct timeval tv;
		gettimeofday(&tv, NULL);],
		[ac_gettimeofday_args=2],
		[AC_TRY_COMPILE([#include <sys/time.h>
			#include <unistd.h>],
			[struct timeval tv; gettimeofday(&tv);],
			[ac_gettimeofday_args=1],
			[ac_gettimeofday_args=0])])])
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
AC_SEARCH_LIBS(dlopen, [dl], [ cf_have_libdl=yes ])
AM_CONDITIONAL(HAVE_LIBDL,test "x$cf_have_libdl" = "xyes")


# Test for libdl isn't taken into account for many plugins, don't take into
# account for Python. Fixes bug #3353920.
# Besides, one can hope modern systems do have that function.

#if test "x$cf_have_libdl" = "xyes" ; then
    ###############
    # Python plugin
    CF_CHECK_PYTHON(
        [PLUGIN_PYTHON="cfpython.la"]
        [cf_will_build_python="yes"],
        [cf_will_build_python="no"]
        [AC_MSG_NOTICE([No Python found. Python plugin will not be built.])]
    )
    # *** Put other plugins configuration code here ***
#else
#    AC_MSG_NOTICE([No dl library found. Plugins will not be supported.])
#fi
AM_CONDITIONAL(PYTHON_PLUGIN,test "x$PLUGIN_PYTHON" != "x")
AC_SUBST(PLUGIN_PYTHON)

########################
# CFLogger and CFNewsPaper configuration

AC_ARG_ENABLE(logger,
              [AS_HELP_STRING([--disable-logger], [Disable the logger compilation])],,
              enable_logger=yes)
AC_ARG_ENABLE(newspaper,
              [AS_HELP_STRING([--disable-newspaper], [Disable the newspaper plugin compilation])],,
              enable_newspaper=yes)
AC_CHECK_LIB(sqlite3, sqlite3_open,[ cf_have_libsqlite=yes ])
AM_CONDITIONAL(HAVE_CFLOGGER, [test x"$cf_have_libsqlite" = "xyes" -a x"$enable_logger" = "xyes"])
AM_CONDITIONAL(HAVE_CFNEWSPAPER, [test x"$cf_have_libsqlite" = "xyes" -a x"$enable_newspaper" = "xyes"])

########################
# Unit test configuration

AC_ARG_ENABLE(check,
              [AS_HELP_STRING([--disable-check], [Disable the unit tests even if check is available])],,
              enable_check=yes)
AM_PATH_CHECK(,[have_check="yes"],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes" -a x"$enable_check" = "xyes")
CF_CHECK_XSLT(xsltengine)
AM_CONDITIONAL(PROCESS_XSLT,[test x"$have_check" = "xyes" -a x"$enable_check" = "xyes" -a x"$xsltengine" != x])
AC_SUBST(XSLTENGINE,[$xsltengine])

########################
# Generate output
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/Developers/Makefile
                 doc/playbook/Makefile doc/playbook-html/Makefile
                 doc/scripts/Makefile
                 doc/spell-docs/Makefile
                 doc/spoiler/Makefile doc/spoiler-html/Makefile
                 common/Makefile
                 devel/Makefile
                 include/Makefile
                 lib/Makefile
                 random_maps/Makefile
                 server/Makefile
                 socket/Makefile
                 types/Makefile
                 utils/Makefile
                 lib/checkarch.pl lib/collect.pl
                 utils/add_throw.perl utils/metaserver.pl utils/scores.pl utils/player_dl.pl
                 utils/crossloop.tmpl utils/crossloop.pl.tmpl utils/crossloop.web
                 plugins/Makefile
                 plugins/cfanim/Makefile plugins/cfanim/include/Makefile
                 plugins/cflogger/Makefile
                 plugins/cfnewspaper/Makefile
                 plugins/cfnewspaper/include/Makefile
                 plugins/cfpython/Makefile plugins/cfpython/include/Makefile
                 plugins/cfrhg/Makefile
                 plugins/cfrhg/include/Makefile
                 plugins/citylife/Makefile
                 plugins/citylife/include/Makefile
                 plugins/common/Makefile plugins/common/include/Makefile
                 test/Makefile
                 test/bugs/Makefile test/bugs/bugtrack/Makefile
                 test/include/Makefile test/templates/Makefile test/toolkit/Makefile
                 test/unit/Makefile
                 test/unit/common/Makefile test/unit/random_maps/Makefile
                 test/unit/server/Makefile test/unit/socket/Makefile])
AC_OUTPUT
AC_MSG_NOTICE([Configuration Summary ----------------------------------------------])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([  Paths])
AC_MSG_NOTICE([    Installation prefix                  ${prefix}])
AC_MSG_NOTICE([    Will put executables in              $bindir])
AC_MSG_NOTICE([    Will put resources in                $datadir])
AC_MSG_NOTICE([    Will put config in                   $sysconfdir])
AC_MSG_NOTICE([    Will put server runtime data in      $localstatedir])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([  Options])
AC_MSG_NOTICE([    Build metaserver2 support?      $ac_cv_header_curl_curl_h])
AC_MSG_NOTICE([    Build plugins?                  $cf_have_libdl])

if test x"$cf_have_libsqlite" = "xyes" ; then
AC_MSG_NOTICE([    Build cflogger?                 $enable_logger])
AC_MSG_NOTICE([    Build cfnewspaper?              $enable_newspaper])
fi

if test "x$cf_have_libdl" = "xyes" ; then
AC_MSG_NOTICE([    Build python plugin?            $cf_will_build_python])
fi

if test "x$enable_cfrhg" = "xyes" ; then
AC_MSG_NOTICE([    Build random house plugin?      yes])
else
AC_MSG_NOTICE([    Build random house plugin?      no])
fi

if test x"$xsltengine" != "x" ; then
    check_complement=["(will generate report)"]
fi
if test "x$have_check" = "xyes" -a x"$enable_check" = "xyes" ; then
AC_MSG_NOTICE([    Process unit testing?           yes $check_complement])
AC_MSG_NOTICE([      (run make; make -k check)])
else
AC_MSG_NOTICE([    Process unit testing?           no])
AC_MSG_NOTICE([      If you are a developer, you MUST install the check framework])
AC_MSG_NOTICE([      and write automated tests for all your add-ons!])
fi
AC_MSG_NOTICE([])
AC_MSG_NOTICE([  Utilities (not required to build or run the server)])
AC_MSG_NOTICE([    Build crossfire-mapper?         $cf_make_mapper])
AC_MSG_NOTICE([--------------------------------------------------------------------])
AC_MSG_NOTICE([Remember to add a symlink to your 'arch' directory in 'lib/arch'!])
