dnl Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
dnl
dnl Licensed under the Apache License, Version 2.0 (the "License");
dnl you may not use this file except in compliance with the License.
dnl You may obtain a copy of the License at
dnl
dnl     http://www.apache.org/licenses/LICENSE-2.0
dnl
dnl Unless required by applicable law or agreed to in writing, software
dnl distributed under the License is distributed on an "AS IS" BASIS,
dnl WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl See the License for the specific language governing permissions and
dnl limitations under the License.

dnl Usage:
dnl AC_XMLRPC_C(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for xmlrpc-c, and defines
dnl - XMLRPC_C_LIBS (linker flags, stripping and path)
dnl - XMLRPC_C_LOCATION

dnl AC_XMLRPC_C_DEVEL(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for xmlrpc-c-devel, and defines
dnl - XMLRPC_C_DEVEL_CFLAGS (compiler flags)
dnl - XMLRPC_C_DEVEL_LIBS (linker flags, stripping and path)
dnl - XMLRPC_C_DEVEL_LOCATION

AC_DEFUN([AC_STORM_XMLRPC_C],
[
    AC_ARG_WITH(storm_xmlrpc_c_prefix, 
                [  --with-storm-xmlrpc-c-prefix=PFX   prefix where 'storm-xmlrpc-c' is installed.],
                [], 
                with_storm_xmlrpc_c_prefix=${STORM_XMLRPC_C_LOCATION:-/usr}
                )
    AC_MSG_CHECKING([for STORM_XMLRPC_C installation at ])

    dnl Temporary checks
    if test "x$host_cpu" = "xi686" ; then
      with_storm_xmlrpc_c_prefix="$with_storm_xmlrpc_c_prefix"
    else
      a=`echo $with_storm_xmlrpc_c_prefix | awk -F '/' '{print $NF}'`
      if test "$a"  != "usr" ; then
        with_storm_xmlrpc_c_prefix="$with_storm_xmlrpc_c_prefix/usr"
      fi
    fi

    if test -n "$with_storm_xmlrpc_c_prefix" ; then
        STORM_XMLRPC_C_LOCATION=$with_storm_xmlrpc_c_prefix
        if test "x$host_cpu" = "xx86_64"; then
            ac_storm_xmlrpc_c_ldlib="-L$with_storm_xmlrpc_c_prefix/lib64/storm"
        else
            ac_storm_xmlrpc_c_ldlib="-L$with_storm_xmlrpc_c_prefix/lib/storm"
        fi
        STORM_XMLRPC_C_LIBS="$ac_storm_xmlrpc_c_ldlib"
        AC_MSG_RESULT([$with_storm_xmlrpc_c_prefix])
    else
        STORM_XMLRPC_C_LOCATION=""
        STORM_XMLRPC_C_LIBS=""
        AC_MSG_ERROR([$with_storm_xmlrpc_c_prefix: no such directory])
    fi

    AC_SUBST(STORM_XMLRPC_C_LOCATION)
    AC_SUBST(STORM_XMLRPC_C_LIBS)
])

AC_DEFUN([AC_STORM_XMLRPC_C_DEVEL],
[
    AC_ARG_WITH(storm_xmlrpc_c_devel_prefix,
                [  --with-storm-xmlrpc-c-devel-prefix=PFX   prefix where 'storm-xmlrpc-c-devel' is installed.],
                [],
                with_storm_xmlrpc_c_devel_prefix=${STORM_XMLRPC_C_DEVEL_LOCATION:-/usr}
                )
    AC_MSG_CHECKING([for STORM_XMLRPC_C_DEVEL installation at ])

    dnl Temporary checks
    if test "x$host_cpu" = "xi686" ; then
      with_storm_xmlrpc_c_devel_prefix="$with_storm_xmlrpc_c_devel_prefix"
    else
      a=`echo $with_storm_xmlrpc_c_devel_prefix | awk -F '/' '{print $NF}'`
      if test "$a"  != "usr" ; then
        with_storm_xmlrpc_c_devel_prefix="$with_storm_xmlrpc_c_devel_prefix/usr"
      fi
    fi

    ac_save_CFLAGS=$CFLAGS
    ac_save_LIBS=$LIBS
    if test -n "$with_storm_xmlrpc_c_devel_prefix" ; then
        STORM_XMLRPC_C_DEVEL_CFLAGS="-I$with_storm_xmlrpc_c_devel_prefix/include/storm"
        if test "x$host_cpu" = "xx86_64"; then
            ac_storm_xmlrpc_c_devel_ldlib="-L$with_storm_xmlrpc_c_devel_prefix/lib64/storm"
        else
            ac_storm_xmlrpc_c_devel_ldlib="-L$with_storm_xmlrpc_c_devel_prefix/lib/storm"
        fi
        STORM_XMLRPC_C_DEVEL_LIBS="$ac_storm_xmlrpc_c_devel_ldlib"
        AC_MSG_RESULT([$with_storm_xmlrpc_c_devel_prefix])
    else
        STORM_XMLRPC_C_DEVEL_CFLAGS=""
        STORM_XMLRPC_C_DEVEL_LIBS=""
        AC_MSG_ERROR([$with_storm_xmlrpc_c_devel_prefix: no such directory])
    fi

    STORM_XMLRPC_C_DEVEL_LIBS="$STORM_XMLRPC_C_LIBS $STORM_XMLRPC_C_DEVEL_LIBS -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok"
    CFLAGS="$STORM_XMLRPC_C_DEVEL_CFLAGS $CFLAGS"
    LIBS="$STORM_XMLRPC_C_DEVEL_LIBS $LIBS"
    AC_TRY_COMPILE([ #include <xmlrpc-c/util.h> ],
                   [ xmlrpc_env x ],
                   [ ac_cv_storm_xmlrpc_c_devel_valid=yes ], [ ac_cv_storm_xmlrpc_c_devel_valid=no ])
    CFLAGS=$ac_save_CFLAGS
    LIBS=$ac_save_LIBS
    AC_MSG_RESULT([$ac_cv_storm_xmlrpc_c_devel_valid])

    if test x$ac_cv_storm_xmlrpc_c_devel_valid = xyes ; then
        STORM_XMLRPC_C_DEVEL_LOCATION=$with_storm_xmlrpc_c_devel_prefix
        ifelse([$2], , :, [$2])
    else
        STORM_XMLRPC_C_DEVEL_CFLAGS=""
        STORM_XMLRPC_C_DEVEL_LIBS=""
        ifelse([$3], , :, [$3])
    fi

    AC_SUBST(STORM_XMLRPC_C_DEVEL_LOCATION)
    AC_SUBST(STORM_XMLRPC_C_DEVEL_CFLAGS)
    AC_SUBST(STORM_XMLRPC_C_DEVEL_LIBS)
])

