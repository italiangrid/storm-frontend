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
dnl AC_CURL(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for curl, and defines
dnl - CURL_LIBS (linker flags, stripping and path)
dnl - CURL_LOCATION

dnl AC_CURL_DEVEL(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for curl-devel, and defines
dnl - CURL_DEVEL_CFLAGS (compiler flags)
dnl - CURL_DEVEL_LIBS (linker flags, stripping and path)
dnl - CURL_DEVEL_LOCATION

AC_DEFUN([AC_CURL],
[
    AC_ARG_WITH(curl_prefix,
    [  --with-curl-prefix=PFX     prefix where curl is installed.],
    [],
        with_curl_prefix=${CURL_LOCATION:-/usr})

    AC_MSG_CHECKING([for CURL installation at ])

    dnl Temporary checks
    if test "x$host_cpu" = "xi686" ; then
      with_curl_prefix="$with_curl_prefix"
    else
      a=`echo $with_curl_prefix | awk -F '/' '{print $NF}'`
      if test "$a"  != "usr" ; then
        with_curl_prefix="$with_curl_prefix/usr"
      fi
    fi

    if test -n "$with_curl_prefix" ; then
        CURL_LOCATION=$with_curl_prefix
        if test "x$host_cpu" = "xx86_64"; then
            ac_curl_ldlib="-L$with_curl_prefix/lib64"
        else
            ac_curl_ldlib="-L$with_curl_prefix/lib"
        fi
        CURL_LIBS="$ac_curl_ldlib"
        AC_MSG_RESULT([$with_curl_prefix])
    else
        CURL_LOCATION=""
        CURL_LIBS=""
        AC_MSG_ERROR([$with_curl_prefix: no such directory])
    fi

    AC_SUBST(CURL_LOCATION)
    AC_SUBST(CURL_LIBS)

])

AC_DEFUN([AC_CURL_DEVEL],
[
    AC_ARG_WITH(curl_devel_prefix,
                [  --with-curl-devel-prefix=PFX   prefix where 'curl-devel' is installed.],
                [],
                with_curl_devel_prefix=${CURL_DEVEL_LOCATION:-/usr}
                )
    AC_MSG_CHECKING([for CURL DEVEL installation at ])

    dnl Temporary checks
    if test "x$host_cpu" = "xi686" ; then
      with_curl_devel_prefix="$with_curl_devel_prefix"
    else
      a=`echo $with_curl_devel_prefix | awk -F '/' '{print $NF}'`
      if test "$a"  != "usr" ; then
        with_curl_devel_prefix="$with_curl_devel_prefix/usr"
      fi
    fi

    ac_save_CFLAGS=$CFLAGS
    ac_save_LIBS=$LIBS
    if test -n "$with_curl_devel_prefix" ; then
        CURL_DEVEL_CFLAGS="-I$with_curl_devel_prefix/include"
        if test "x$host_cpu" = "xx86_64"; then
            ac_curl_devel_ldlib="-L$with_curl_devel_prefix/lib64"
        else
            ac_curl_devel_ldlib="-L$with_curl_devel_prefix/lib"
        fi
        CURL_DEVEL_LIBS="$ac_curl_devel_ldlib"
        AC_MSG_RESULT([$with_curl_devel_prefix])
    else
        CURL_DEVEL_CFLAGS=""
        CURL_DEVEL_LIBS=""
        AC_MSG_ERROR([$with_curl_devel_prefix: no such directory])
    fi

    CURL_DEVEL_LIBS="$CURL_LIBS $CURL_DEVEL_LIBS -lcurl"
    CFLAGS="$CURL_DEVEL_CFLAGS $CFLAGS"
    LIBS="$CURL_DEVEL_LIBS $LIBS"
    AC_TRY_COMPILE([ #include <curl/curl.h> ],
                   [ CURLINFO c ],
                   [ ac_cv_curl_devel_valid=yes ], [ ac_cv_curl_devel_valid=no ])
    CFLAGS=$ac_save_CFLAGS
    LIBS=$ac_save_LIBS
    AC_MSG_RESULT([$ac_cv_curl_devel_valid])

    if test x$ac_cv_curl_devel_valid = xyes ; then
        CURL_DEVEL_LOCATION=$with_curl_devel_prefix
        ifelse([$2], , :, [$2])
    else
        CURL_DEVEL_CFLAGS=""
        CURL_DEVEL_LIBS=""
        CURL_DEVEL_LOCATION=""
        ifelse([$3], , :, [$3])
    fi

    AC_SUBST(CURL_DEVEL_LOCATION)
    AC_SUBST(CURL_DEVEL_CFLAGS)
    AC_SUBST(CURL_DEVEL_LIBS)
])
