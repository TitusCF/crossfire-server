
AC_DEFUN([CF_CHECK_PYTHON],
[
	PYTHON_LIB=""
	PY_LIBS=""
	PY_INCLUDES=""
	dir=""
	if test "x$PYTHON_HOME" != "x"; then
		for dir in $PYTHON_HOME/include/python{,2.5,2.4,2.3,2.2,2.1,2.0} ; do
			AC_CHECK_HEADERS(["$dir/Python.h"],[cf_have_python_h=yes])
			if test "x$cf_have_python_h" != "x" ; then
				PY_INCLUDES="-I$dir"
				break
			fi
		done
		PYTHON_SEARCH=$PYTHON
	else
		AC_CHECK_HEADERS([Python.h],[cf_have_python_h=yes])
		if test "x$cf_have_python_h" = "x"  ; then
			for dir in  /usr{,/local}/include/python{,2.5,2.4,2.3,2.2,2.1,2.0} ; do
				AC_CHECK_HEADERS(["$dir/Python.h"],[cf_have_python_h=yes])
				if test "x$cf_have_python_h" != "x" ; then
					PY_INCLUDES="-I$dir"
					break
				fi
			done
		else
			PY_INCLUDES=""
		fi
	fi

	if test "x$cf_have_python_h" = "xyes" ; then
		PYTHON_LIB=""
		if test "x$PYTHON_HOME" != "x"; then
			# I am going of how manually compiled python installed on
			# my system.  We can't use AC_CHECK_LIB, because that will
			# find the one in the stanard location, which is what we
			# want to avoid.
			python=`echo $dir | awk -F/ '{print $NF}'`;
			AC_MSG_CHECKING([For python lib in various places])
			if test -f $PYTHON_HOME/lib/lib$python.so ; then
				# Hopefully -R is a universal option
				AC_MSG_RESULT([found in $PYTHON_HOME/lib/])
				if test -n "$hardcode_libdir_flag_spec" ; then
					oldlibdir=$libdir
					libdir="$PYTHON_HOME/lib/"
					rpath=`eval echo $hardcode_libdir_flag_spec`
					PYTHON_LIB="$rpath -L$PYTHON_HOME/lib/ -l$python"
					echo "rpath=$rpath"
					libdir=$oldlibdir
				else
					PYTHON_LIB="-L$PYTHON_HOME/lib/ -l$python"
				fi

			elif test -f $PYTHON_HOME/lib/$python/lib$python.a ; then
				PYTHON_LIB="$PYTHON_HOME/lib/$python/lib$python.a"
				AC_MSG_RESULT([found in $PYTHON_HOME/lib/$python])
			elif test -f $PYTHON_HOME/lib/$python/config/lib$python.a ; then
				PYTHON_LIB="$PYTHON_HOME/lib/$python/config/lib$python.a"
				AC_MSG_RESULT([found in $PYTHON_HOME/lib/$python/config])
			fi

		else
	                for lib in python{,2.5,2.4,2.3,2.2,2.1,2.0} ; do
	                        AC_CHECK_LIB($lib, PyArg_ParseTuple,[PYTHON_LIB="-l$lib"])
	                        if test "x$PYTHON_LIB" != "x" ; then
	                                break
	                        fi
	                done

			# These checks are a bit bogus - would be better to use AC_CHECK_LIB,
			# but it caches the result of the first check, even if we run AC_CHECK_LIB
			# with other options.
			python=`echo $dir | awk -F/ '{print $NF}'`;
			if test "x$PYTHON_LIB" = "x"  ; then
				AC_MSG_CHECKING([For python lib in various places])
				if test -f /usr/lib/$python/lib$python.a ; then
					PYTHON_LIB="/usr/lib/$python/lib$python.a"
					AC_MSG_RESULT([found in /usr/lib/$python])
				elif test -f /usr/lib/$python/config/lib$python.a ; then
					PYTHON_LIB="/usr/lib/$python/config/lib$python.a"
					AC_MSG_RESULT([found in /usr/lib/$python/config])
				fi
			fi
		fi
		if test "x$PYTHON_LIB" != "x"  ; then
			AC_CHECK_LIB(pthread, main,  PY_LIBS="$PY_LIBS -lpthread", , $PY_LIBS )
			AC_CHECK_LIB(util, main,  PY_LIBS="$PY_LIBS -lutil", , $PY_LIBS )
			AC_CHECK_LIB(dl, main,  PY_LIBS="$PY_LIBS -ldl", , $PY_LIBS )

			AC_MSG_CHECKING([whether python supports the "L" format specifier])
			saved_LIBS="$LIBS"
			LIBS="$LIBS $PYTHON_LIB $PY_LIBS"
			saved_CFLAGS="$CFLAGS"
			CFLAGS="$CFLAGS $PY_INCLUDES"
			AC_TRY_RUN([
#include <Python.h>
#include <stdlib.h>

static PyObject *callback(PyObject *self, PyObject *args)
{
    long long val;

    if (!PyArg_ParseTuple(args, "L", &val))
	return NULL;
    if (val != 1)
	exit(1);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef methods[] = {
    {"callback", callback, METH_VARARGS},
    {NULL, NULL, 0, NULL},
};

int main()
{
    Py_Initialize();
    Py_InitModule("test", methods);
    return(PyRun_SimpleString("import test\ntest.callback(1)\n") != 0);
}
				], [
				AC_MSG_RESULT([yes])
				], [
				AC_MSG_RESULT([no])
				PYTHON_LIB=""
				PYLIBS=""
				PY_INCLUDE=""
				],
				[
				AC_MSG_RESULT([skipped because cross compiling])
				])
			LIBS="$saved_LIBS"
			CFLAGS="$saved_CFLAGS"
		fi
	fi

	if test "x$PYTHON_LIB" = "x"  ; then
		$2
	else
		$1
	fi

	AC_SUBST(PYTHON_LIB)
	AC_SUBST(PY_LIBS)
	AC_SUBST(PY_INCLUDES)
])
dnl AM_PATH_CHECK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for check, and define CHECK_CFLAGS and CHECK_LIBS
dnl

AC_DEFUN([AM_PATH_CHECK],
[
  AC_ARG_WITH(check,
  [  --with-check=PATH       prefix where check is installed [default=auto]])

  min_check_version=ifelse([$1], ,0.8.2,$1)

  AC_MSG_CHECKING(for check - version >= $min_check_version)

  if test x$with_check = xno; then
    AC_MSG_RESULT(disabled)
    ifelse([$3], , AC_MSG_ERROR([disabling check is not supported]), [$3])
  else
    if test "x$with_check" != x; then
      CHECK_CFLAGS="-I$with_check/include"
      CHECK_LIBS="-L$with_check/lib -lcheck"
    else
      CHECK_CFLAGS=""
      CHECK_LIBS="-lcheck"
    fi

    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"

    CFLAGS="$CFLAGS $CHECK_CFLAGS"
    LIBS="$CHECK_LIBS $LIBS"

    rm -f conf.check-test
    AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>

#include <check.h>

int main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.check-test");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = strdup("$min_check_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_check_version");
     return 1;
   }

  if ((CHECK_MAJOR_VERSION != check_major_version) ||
      (CHECK_MINOR_VERSION != check_minor_version) ||
      (CHECK_MICRO_VERSION != check_micro_version))
    {
      printf("\n*** The check header file (version %d.%d.%d) does not match\n",
	     CHECK_MAJOR_VERSION, CHECK_MINOR_VERSION, CHECK_MICRO_VERSION);
      printf("*** the check library (version %d.%d.%d).\n",
	     check_major_version, check_minor_version, check_micro_version);
      return 1;
    }

  if ((check_major_version > major) ||
      ((check_major_version == major) && (check_minor_version > minor)) ||
      ((check_major_version == major) && (check_minor_version == minor) && (check_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** An old version of check (%d.%d.%d) was found.\n",
             check_major_version, check_minor_version, check_micro_version);
      printf("*** You need a version of check being at least %d.%d.%d.\n", major, minor, micro);
      printf("***\n");
      printf("*** If you have already installed a sufficiently new version, this error\n");
      printf("*** probably means that the wrong copy of the check library and header\n");
      printf("*** file is being found. Rerun configure with the --with-check=PATH option\n");
      printf("*** to specify the prefix where the correct version was installed.\n");
    }

  return 1;
}
],, no_check=yes, [echo $ac_n "cross compiling; assumed OK... $ac_c"])

    CFLAGS="$ac_save_CFLAGS"
    LIBS="$ac_save_LIBS"

    if test "x$no_check" = x ; then
      AC_MSG_RESULT(yes)
      ifelse([$2], , :, [$2])
    else
      AC_MSG_RESULT(no)
      if test -f conf.check-test ; then
        :
      else
        echo "*** Could not run check test program, checking why..."
        CFLAGS="$CFLAGS $CHECK_CFLAGS"
        LIBS="$CHECK_LIBS $LIBS"
        AC_TRY_LINK([
#include <stdio.h>
#include <stdlib.h>

#include <check.h>
], ,  [ echo "*** The test program compiled, but did not run. This usually means"
        echo "*** that the run-time linker is not finding check. You'll need to set your"
        echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
        echo "*** to the installed location  Also, make sure you have run ldconfig if that"
        echo "*** is required on your system"
	echo "***"
        echo "*** If you have an old version installed, it is best to remove it, although"
        echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
      [ echo "*** The test program failed to compile or link. See the file config.log for"
        echo "*** the exact error that occured." ])

        CFLAGS="$ac_save_CFLAGS"
        LIBS="$ac_save_LIBS"
      fi

      CHECK_CFLAGS=""
      CHECK_LIBS=""

      rm -f conf.check-test
      ifelse([$3], , AC_MSG_ERROR([check not found]), [$3])
    fi

    AC_SUBST(CHECK_CFLAGS)
    AC_SUBST(CHECK_LIBS)

    rm -f conf.check-test

  fi
])


dnl CF_IS_XSLT_COMPLIANT(progpath,ACTION-IF-FOUND, ACTION_IF_NOT_FOUND)
dnl check for xslt compliance of a given prog, prog must be a full executable
dnl execution command, in this command, this substitution will be donne:
dnl %1  = xml file
dnl %2  = xsl file
dnl %3  = html file
dnl
AC_DEFUN([CF_IS_XSLT_COMPLIANT],[
    cat << \EOF > configtest.xml
<?xml version="1.0" encoding="ISO-8859-1"?>
<tool>
  <field id="prodName">
    <value>HAMMER HG2606</value>
  </field>
  <field id="prodNo">
    <value>32456240</value>
  </field>
  <field id="price">
    <value>$30.00</value>
  </field>
</tool>
EOF
    cat << \EOF > configtest.xsl
<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
<html>
<body>
<form method="post" action="edittool.asp">
<h2>Tool Information (edit):</h2>
<table border="0">
<xsl:for-each select="tool/field">
<tr>
<td>
<xsl:value-of select="@id"/>
</td>
<td>
<input type="text">
<xsl:attribute name="id">
  <xsl:value-of select="@id" />
</xsl:attribute>
<xsl:attribute name="name">
  <xsl:value-of select="@id" />
</xsl:attribute>
<xsl:attribute name="value">
  <xsl:value-of select="value" />
</xsl:attribute>
</input>
</td>
</tr>
</xsl:for-each>
</table>
<br />
<input type="submit" id="btn_sub" name="btn_sub" value="Submit" />
<input type="reset" id="btn_res" name="btn_res" value="Reset" />
</form>
</body>
</html>
</xsl:template>
</xsl:stylesheet>
EOF
    AC_MSG_CHECKING([xslt compliance of $1])
    to_run=$1
    to_run=${to_run/\%1/configtest.xml}
    to_run=${to_run/\%2/configtest.xsl}
    to_run=${to_run/\%3/configtest.out}
    if AC_TRY_COMMAND([$to_run]);then
    	AC_MSG_RESULT([yes]);
        [$2]
    else
    	AC_MSG_RESULT([no]);
        [$3]
    fi
])

AC_DEFUN([CF_CHECK_XSLT],[
    AC_ARG_WITH(xsltproc, [  --with-xsltproc=path     specify xslt engine to use for test report generation],
        [check_xslt_forcedprogfound=$withval])
    if test "x$check_xslt_forcedprogfound" != "x";  then
        AC_PATH_PROG([check_xslt_forcedprogfound],[$check_xslt_forcedprogfound],[notfound])
    fi
    AC_PATH_PROG([check_xslt_xsltprocfound],[xsltproc],[notfound])
    AC_PATH_PROG([check_xslt_sablotronfound],[sabcmd],[notfound])
    xslt_prog="notfound"
    if test "$check_xslt_forcedprogfound" != notfound -a "x$check_xslt_forcedprogfound" != x; then
        xslt_prog="$check_xslt_forcedprogfound"
        CF_IS_XSLT_COMPLIANT([$xslt_prog],[$1=$xslt_prog],[xslt_prog="notfound"])
    fi
    xslt_prog="notfound"
    if test "$check_xslt_xsltprocfound" != notfound -a "$xslt_prog" = notfound; then
        xslt_prog="$check_xslt_xsltprocfound -o %3 %2 %1"
        CF_IS_XSLT_COMPLIANT([$xslt_prog],[$1=$xslt_prog],[xslt_prog="notfound"])
    fi
    if test "$check_xslt_sablotronfound" != notfound -a "$xslt_prog" = notfound; then
        xslt_prog="$check_xslt_sablotronfound %2 %1 %3"
        CF_IS_XSLT_COMPLIANT([$xslt_prog],[$1=$xslt_prog],[xslt_prog="notfound"])
    fi
])
