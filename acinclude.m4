
AC_DEFUN([CF_CHECK_PYTHON],
[
	PYTHON_LIB=""
	PY_LIBS=""
	PY_INCLUDES=""
	dir=""
	AC_CHECK_HEADERS([Python.h],[cf_have_python_h=yes])
	if test "x$cf_have_python_h" = "x"  ; then
		for dir in /usr{,/local}/include/python{,2.3,2.2,2.1,2.0} ; do
			AC_CHECK_HEADERS(["$dir/Python.h"],[cf_have_python_h=yes])
			if test "x$cf_have_python_h" != "x" ; then
				PY_INCLUDES="-I$dir"
				break
			fi
		done
	else
		PY_INCLUDES=""
	fi

	if test "x$cf_have_python_h" = "xyes" ; then
		PYTHON_LIB=""

                for lib in python{,2.3,2.2,2.1,2.0} ; do
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
		if test "x$PYTHON_LIB" != "x"  ; then
			AC_CHECK_LIB(pthread, main,  PY_LIBS="$PY_LIBS -lpthread", , $PY_LIBS )
			AC_CHECK_LIB(util, main,  PY_LIBS="$PY_LIBS -lutil", , $PY_LIBS )
			$1
		fi
	fi

	AC_SUBST(PYTHON_LIB)
	AC_SUBST(PY_LIBS)
	AC_SUBST(PY_INCLUDES)
])
