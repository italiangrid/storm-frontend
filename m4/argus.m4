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

dnl
dnl Define ARGUS
dnl
AC_DEFUN([AC_ARGUS],
[
    AC_ARG_WITH(argus-location,
	[  --with-argus-location=PFX     prefix where argus is installed. (/usr)],
	[],
        with_argus_location=${ARGUS_LOCATION:-/usr})
     
    AC_MSG_RESULT([checking for argus... ])

    if test -n "with_argus_location" ; then
        ARGUS_LOCATION="$with_argus_location"
        ARGUS_CFLAGS="-I$with_argus_location/include"
        if test "x$host_cpu" = "xx86_64"; then
            ac_argus_ldlib="-L$with_argus_location/lib64"
        else
            ac_argus_ldlib="-L$with_argus_location/lib"
        fi
        ARGUS_LIBS="$ac_argus_ldlib -largus-pep"
    else
	ARGUS_LOCATION=""
	ARGUS_CFLAGS=""
	ARGUS_LIBS=""
    fi
       
    AC_MSG_RESULT([yes])
    ARGUS_CFLAGS="$ARGUS_CFLAGS"

    AC_MSG_RESULT([ARGUS_LOCATION set to $ARGUS_LOCATION])
    AC_MSG_RESULT([ARGUS_CFLAGS set to $ARGUS_CFLAGS])
    AC_MSG_RESULT([ARGUS_LIBS set to $ARGUS_LIBS])
    
    AC_SUBST(ARGUS_LOCATION)
    AC_SUBST(ARGUS_CFLAGS)
    AC_SUBST(ARGUS_LIBS)
])
