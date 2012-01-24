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
dnl - VOMS_LIBS

AC_DEFUN([AC_VOMS],
[

  AC_ARG_WITH(
    voms-prefix,
    AC_HELP_STRING(
      [--with-voms-prefix=DIR],
      [root of the voms installation]
    ),
    [ac_voms_prefix=$withval],
    [ac_voms_prefix=${VOMS_LOCATION:-/usr}]
  )

  AC_ARG_WITH(
    voms-include,
    AC_HELP_STRING(
      [--with-voms-include=DIR],
      [root of the boost installation]
    ),
    [ac_voms_include=$withval],
    [ac_voms_include=${VOMS_INCLUDE_LOCATION:-/usr/include}]
  )

  VOMS_INSTALL_PATH=$ac_voms_prefix
  VOMS_INCLUDE_PATH=$ac_voms_include

  if test "x$host_cpu" = "xx86_64"; then
     library_path="lib64"
  else
     library_path="lib"
  fi

  AC_MSG_CHECKING([for VOMS installation at ${with_voms_prefix}])

  ac_save_CFLAGS=$CFLAGS
  ac_save_CPPFLAGS=$CPPFLAGS
  ac_save_LIBS=$LIBS
  if test -n "$with_voms_include" ; then
     VOMS_CFLAGS="-I$ac_voms_include"
     VOMS_PATH_LIBS="-L$with_voms_prefix/$library_path"
  else
     VOMS_CFLAGS=""
     VOMS_PATH_LIBS=""
     VOMS_LIBS=""
  fi

  VOMS_LIBS="$VOMS_PATH_LIBS -lvomsapi"

  AC_LANG_SAVE
  dnl AC_LANG_CPLUSPLUS
  CPPFLAGS="$GLOBUS_THR_CFLAGS $VOMS_CFLAGS $CPPFLAGS"
  LIBS="$VOMS_LIBS $LIBS"

  AC_TRY_COMPILE([ #include <voms/voms_api.h> ],
                 [ vomsdata vo_data("","") ],
                 [ ac_cv_vomscpp_valid=yes ], [ac_cv_vomscpp_valid=no ])
  CPPFLAGS=$ac_save_CPPFLAGS
  LIBS=$ac_save_LIBS
  AC_LANG_RESTORE
  AC_MSG_RESULT([$ac_cv_vomscpp_valid for cpp api])

  if test "x$ac_cv_vomscpp_valid" = "xyes" ; then
     ifelse([$2], , :, [$2])
  else
     VOMS_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(VOMS_INSTALL_PATH)
  AC_SUBST(VOMS_INCLUDE_PATH)
  AC_SUBST(VOMS_CFLAGS)
  AC_SUBST(VOMS_LIBS)
])

