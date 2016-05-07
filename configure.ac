dnl Process this file with autoconf to produce a configure script.
AC_INIT([crossfire], [1.71.dev], [crossfire@metalforge.org])
AC_CONFIG_AUX_DIR([utils])
AC_CONFIG_HEADERS([include/autoconf.h])
AC_CONFIG_MACRO_DIR([macros])
AC_CONFIG_SRCDIR([server/main.c])
AC_PREFIX_DEFAULT([/usr/games/crossfire])
AM_INIT_AUTOMAKE([subdir-objects])

AM_SILENT_RULES([yes])

dnl#############################################################################
dnl Configuration Options
dnl#############################################################################
AC_ARG_ENABLE(mapper, [AS_HELP_STRING([--enable-mapper],
              [Build the crossfire-mapper utility (requires libgd)])],
              [enable_mapper=yes], [enable_mapper=no])

dnl Options for plugins
AC_ARG_WITH(python, [AS_HELP_STRING([--with-python=dir],
            [Directory to look for python headers/library])],
            [PYTHON_HOME="$withval"])

AC_ARG_ENABLE(cfrhg, [AS_HELP_STRING([--enable-cfrhg],
              [Build the random house generator plugin])],
              [], [enable_cfrhg=no])

AC_ARG_ENABLE(logger, [AS_HELP_STRING([--enable-logger],
              [Build the SQLite logging plugin])],
              [enable_logger=yes], [enable_logger=no])

AC_ARG_ENABLE(newspaper, [AS_HELP_STRING([--enable-newspaper],
              [Build the newspaper plugin])],
              [enable_newspaper=yes], [enable_newspaper=no])

dnl Unit test configuration
AC_ARG_ENABLE(check, [AS_HELP_STRING([--disable-check],
              [Disable the unit tests even if check is available])],
              [enable_check=no], [enable_check=yes])

dnl Add support for dmalloc, if the user wants it.
AM_WITH_DMALLOC

dnl#############################################################################
dnl Program Checks
dnl#############################################################################
AC_PROG_AWK
AC_PROG_CC_C99
AC_PROG_LIBTOOL
AC_PROG_LEX

dnl If no lexical parser is found, $LEX is set to ":".
dnl Check for that so we can exit if lex and flex are not found.
dnl
dnl -- Daniel Hawkins 2015-05-25

if test $LEX == ':'; then
    AC_MSG_ERROR([Lexical parser flex is missing.])
fi

AC_PATH_PROG(BASENAME, basename)
AC_PATH_PROG(GZIP, gzip)
AC_PATH_PROG(LATEX, latex)
AC_PATH_PROG(PERL, perl)
AC_PATH_PROG(SVNVERSION, svnversion)
AC_PATH_PROG(TAR, tar)

AC_SUBST(BASENAME)
AC_SUBST(GZIP)
AC_SUBST(PERL)

dnl#############################################################################
AS_BOX([Library Checks])
dnl#############################################################################
AC_SEARCH_LIBS([crypt], [crypt])
AC_SEARCH_LIBS([pow], [m])
AC_SEARCH_LIBS([pthread_create], [pthread])
AC_SEARCH_LIBS([sqrt], [m])
AC_SEARCH_LIBS([clock_gettime], [rt])

dnl Metaserver2
LIBCURL_CHECK_CONFIG()
LIBS="$LIBS $LIBCURL"
CPPFLAGS="$CPPFLAGS $LIBCURL_CPPFLAGS"

dnl Plugins
AC_SEARCH_LIBS([dlopen], [dl], [cf_have_libdl=yes])
AC_SEARCH_LIBS([sqlite3_open], [sqlite3], [cf_have_libsqlite=yes])

dnl For utils/mapper
AS_IF([test "$enable_mapper" = "yes"], [
    PKG_CHECK_MODULES([GD], [gdlib])
])
AM_CONDITIONAL(MAPPER, test "$enable_mapper" = "yes")

dnl Random house generator
AM_CONDITIONAL(CFRHG_PLUGIN, test "$enable_cfrhg" = "yes")

dnl Unit tests
AS_IF([test "$enable_check" = "yes"], [
    PKG_CHECK_MODULES([CHECK], [check], [have_check="yes"], [have_check="no"])
], [
    have_check="no"
])

dnl#############################################################################
dnl Header File, Type, Structure, and Compiler Checks
dnl#############################################################################
dnl Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS([crypt.h])

dnl Checks for typedefs, structures, and compiler characteristics.
CF_CHECK_VISIBILITY()

dnl#############################################################################
dnl Library Function Checks
dnl#############################################################################
dnl Checks for library functions.
AC_CHECK_FUNCS([getdtablesize strcasecmp strcasestr strdup strncasecmp srandom srand48 sysconf tempnam getaddrinfo getnameinfo])

dnl Check for better but non-portable functions.
AC_CHECK_FUNCS([strlcat strlcpy])

AC_SUBST(pkgstatedir,$localstatedir/$PACKAGE)
AC_SUBST(pkgconfdir,$sysconfdir/$PACKAGE)

dnl############################################
dnl Plugin configuration
dnl############################################
AM_CONDITIONAL(HAVE_LIBDL, test "$cf_have_libdl" = "yes")

    dnl Python plugin
    CF_CHECK_PYTHON(
        [PLUGIN_PYTHON="cfpython.la"]
        [cf_will_build_python="yes"],
        [cf_will_build_python="no"]
        [AC_MSG_NOTICE([No Python found. Python plugin will not be built.])]
    )
AM_CONDITIONAL(PYTHON_PLUGIN,test "x$PLUGIN_PYTHON" != "x")
AC_SUBST(PLUGIN_PYTHON)

AM_CONDITIONAL(HAVE_CFLOGGER, [test "$cf_have_libsqlite" = "yes" -a "$enable_logger" = "yes"])
AM_CONDITIONAL(HAVE_CFNEWSPAPER, [test "$cf_have_libsqlite" = "yes" -a "$enable_newspaper" = "yes"])

dnl Unit tests
AM_CONDITIONAL(HAVE_CHECK, test "$have_check" = "yes" -a "$enable_check" = "yes")

if test "$have_check" = "yes"; then
    CF_CHECK_XSLT(xsltengine)
    AC_SUBST(XSLTENGINE, ["$xsltengine"])
else
    AC_MSG_WARN([Check not found; cannot run unit tests!])
fi

AM_CONDITIONAL(PROCESS_XSLT, [test -n "$xsltengine"])

dnl Make sure pre-built archetypes or an 'arch' directory exists.
AC_CHECK_FILE([lib/archetypes], [], [has_archetypes=no])
AC_CHECK_FILE([lib/arch], [], [has_arch_dir=no])

AS_IF([test "$has_archetypes" == no -a "$has_arch_dir" == no], [
       AC_MSG_ERROR([Could not find pre-built or raw archetypes.])
])


dnl#############################################################################
AS_BOX([Generate Output])
dnl#############################################################################
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/Developers/Makefile
                 doc/playbook/Makefile doc/playbook-html/Makefile
                 doc/scripts/Makefile
                 doc/spell-docs/Makefile
                 doc/spoiler/Makefile doc/spoiler-html/Makefile
                 common/Makefile
                 include/Makefile
                 lib/Makefile
                 random_maps/Makefile
                 server/Makefile
                 socket/Makefile
                 types/Makefile
                 utils/Makefile
                 utils/scores.pl utils/player_dl.pl
                 utils/crossloop
                 utils/crossloop.web
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

dnl#############################################################################
dnl Configuration Summary
dnl#############################################################################
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

if test "$cf_have_libsqlite" = "yes" ; then
AC_MSG_NOTICE([    Build cflogger?                 $enable_logger])
AC_MSG_NOTICE([    Build cfnewspaper?              $enable_newspaper])
fi

if test "$cf_have_libdl" = "yes" ; then
AC_MSG_NOTICE([    Build python plugin?            $cf_will_build_python])
fi

AC_MSG_NOTICE([    Build random house plugin?      $enable_cfrhg])

if test x"$xsltengine" != "x" ; then
    check_complement=["(will generate report)"]
fi
if test "$have_check" = "yes" -a "$enable_check" = "yes" ; then
AC_MSG_NOTICE([    Process unit testing?           yes $check_complement])
AC_MSG_NOTICE([      (run make; make -k check)])
else
AC_MSG_NOTICE([    Process unit testing?           no])
AC_MSG_NOTICE([      If you are a developer, you MUST install the check framework])
AC_MSG_NOTICE([      and write automated tests for all your add-ons!])
fi
AC_MSG_NOTICE([])
AC_MSG_NOTICE([  Utilities (not required to build or run the server)])
AC_MSG_NOTICE([    Build crossfire-mapper?         $enable_mapper])
