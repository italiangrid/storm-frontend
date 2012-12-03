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
dnl AC_BOOST(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl - BOOST_CFLAGS (compiler flags)
dnl - BOOST_THREAD_LIBS (linker flags, stripping and path)
dnl - BOOST_PO_LIBS (linker flags, stripping and path)
dnl - BOOST_INSTALL_PATH
dnl - BOOST_INCLUDE_PATH

AC_DEFUN([AC_BOOST],
[
  AC_ARG_WITH(
    boost-prefix, 
    AC_HELP_STRING(
      [--with-boost-prefix=DIR],
      [root of the boost installation]
    ),
    [ac_boost_prefix=$withval],
    [ac_boost_prefix="/usr"]
  )

  dnl Temporary checks
  if test "x$host_cpu" = "xi686" ; then
    ac_boost_prefix="$ac_boost_prefix"
  else 
    a=`echo $with_boost_prefix | awk -F '/' '{print $NF}'`
    if test "$a"  != "usr" ; then
      ac_boost_prefix="$ac_boost_prefix"
    fi
  fi

  AC_ARG_WITH(
    boost-devel-prefix,
    AC_HELP_STRING(
      [--with-boost-devel-prefix=DIR],
      [root of the boost installation]
    ),
    [ac_boost_devel_prefix=$withval],
    [ac_boost_devel_prefix="/usr"]
  )

  dnl Temporary checks
  if test "x$host_cpu" = "xi686" ; then
    ac_boost_devel_prefix="$ac_boost_devel_prefix"
  else
    a=`echo $with_boost_devel_prefix | awk -F '/' '{print $NF}'`
    if test "$a"  != "usr" ; then
      ac_boost_devel_prefix="$ac_boost_devel_prefix"
    fi
  fi

  AC_ARG_ENABLE(
    boost-debug,
    AC_HELP_STRING(
      [--enable-boost-debug],
      [use debug boost libraries]
    ),
    [ac_boost_debug=$enableval],
    [ac_boost_debug="no"]
  )

  AC_MSG_RESULT([using boost debug: $ac_boost_debug])

  AC_ARG_ENABLE(
    boost-mt,
    AC_HELP_STRING(
      [--enable-boost-mt],
      [use multithreaded boost libraries (default = yes)]
    ),
    [ac_boost_mt=$enableval],
    [ac_boost_mt="yes"]
  )

  AC_MSG_RESULT([using boost mt: $ac_boost_mt])

  AC_ARG_WITH(
    boost-include,
    AC_HELP_STRING(
      [--with-boost-include=DIR],
      [root of the boost installation]
    ),
    [ac_boost_include=$withval],
    [ac_boost_include="/usr/include/boost141"]
  )

  if test "x$1" = "xsl6" ; then
    ac_boost_include="/usr/include/boost"
    ac_boost_version_file="$ac_boost_include/version.hpp"
  elif test "x$1" = "xsl5" ; then
    ac_boost_include="/usr/include/boost141"
    ac_boost_version_file="$ac_boost_include/boost/version.hpp"
  else
    ac_boost_include="/usr/include/boost141"
    ac_boost_version_file="$ac_boost_include/boost/version.hpp"
  fi

  AC_MSG_RESULT([boost path version: $ac_boost_include])
  AC_MSG_RESULT([boost version: $ac_boost_version_file])

  if test -r "$ac_boost_version_file" ; then
    ac_boost_version=`cat $ac_boost_version_file | grep '^# *define  *BOOST_VERSION  *[0-9]\+$' | sed 's,^# *define  *BOOST_VERSION  *\([0-9]\+\)$,\1,'`
    AC_MSG_RESULT([boost version 11111: $ac_boost_version])
  fi
  AC_MSG_RESULT([boost version 2222: $ac_boost_version])
  if test -z "$ac_boost_version" ; then
    AC_MSG_ERROR(unknow version of boost, [1])
  fi

  AC_MSG_CHECKING([for boost installation])

  if test "x$host_cpu" = "xia64" ; then
    libdir_local="lib64"
  elif test "x$host_cpu" = "xx86_64" ; then
    libdir_local="lib64"
  else
    libdir_local="lib"
  fi

  runtime=
  if test $ac_boost_debug = yes; then
    runtime="d"
  fi
  threading=
  mt_cflags=
  if test x$ac_boost_mt = xyes; then
    threading="mt"
    if test "x$host_cpu" = "xia64" ; then
      mt_cflags="-pthread -D_REENTRANT"
    elif test "x$host_cpu" = "xx86_64" ; then
      mt_cflags="-pthread -D_REENTRANT"
    else
      mt_cflags="-pthread"
    fi
  fi

  dnl Temporary checks
  if test "x$host_cpu" = "xi686" ; then
    toolset="-gcc34"
  else
    toolset=""
  fi
  if test x${threading} != x ; then
    threading="-${threading}"
  fi
  if test x${runtime} != x ; then
    runtime="-${runtime}"
  fi
  ext="${toolset}${threading}${runtime}"

  dnl Test for existance of Boost-style library tags.
  if test ! -r $ac_boost_devel_prefix/$libdir_local/boost141/libboost_regex$ext.so  -a \
            -r $ac_boost_devel_prefix/$libdir_local/boost141/libboost_regex.so ; then
    AC_MSG_WARN([*** Cannot find Boost libraries tagged with $ext. Building with no library tag.])
    ext=
    static_ext=
    ac_have_boost=no
  else
    ac_have_boost=yes
  fi
 
  unset runtime static_runtime threading toolset

  BOOST_CFLAGS="$mt_cflags -I$ac_boost_include"

  BOOST_THREAD_LIBS="-L$ac_boost_devel_prefix/$libdir_local/boost141 -lboost_thread$ext -lpthread"
  BOOST_PO_LIBS="-L$ac_boost_devel_prefix/$libdir_local/boost141 -lboost_program_options$ext"

  BOOST_INSTALL_PATH=$ac_boost_prefix
  BOOST_INCLUDE_PATH=$ac_boost_include
  BOOST_DEVEL_INSTALL_PATH=$ac_boost_devel_prefix

  unset mt_cflags ext 

  dnl AC_LANG_SAVE
  dnl dnl AC_LANG_CPLUSPLUS
  dnl ac_save_cppflags=$CPPFLAGS
  dnl ac_save_libs=$LIBS
  dnl CPPFLAGS="$BOOST_CFLAGS $CPPFLAGS"
  dnl LIBS="$BOOST_THREAD_LIBS $BOOST_PO_LIBS $LIBS"
  dnl AC_MSG_WARN([$LIBS $CPPFLAGS])
  dnl AC_TRY_LINK([#include <boost/thread/mutex.hpp>],
  dnl             [ ],
  dnl             [ac_have_boost=yes ], [ ac_have_boost=no])
  dnl AC_MSG_RESULT([$ac_have_boost])
  dnl CPPFLAGS=$ac_save_cppflags
  dnl LIBS=$ac_save_libs
  dnl AC_LANG_RESTORE

  dnl assume sstream available
  AC_DEFINE([HAVE_STRINGSTREAM], 1, [Define when we are sure to have the right strstream header])

  if test x$ac_have_boost = xyes ; then
      ifelse([$2], , :, [$2])
  else    
      ifelse([$3], , :, [$3])
  fi

  AC_SUBST(BOOST_INSTALL_PATH)
  AC_SUBST(BOOST_INCLUDE_PATH)
  AC_SUBST(BOOST_DEVEL_INSTALL_PATH)
  AC_SUBST(BOOST_CFLAGS)
  AC_SUBST(BOOST_THREAD_LIBS)
  AC_SUBST(BOOST_PO_LIBS)
])


