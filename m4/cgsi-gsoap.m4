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
dnl Define CGSI-GSOAP
dnl
AC_DEFUN([AC_CGSI_GSOAP],
[
    AC_ARG_WITH(cgsi-gsoap-location,
	[  --with-cgsi-gsoap-location=PFX     prefix where CGSI GSOAP plugin is installed. (/usr)],
	[],
        with_cgsi_gsoap_location=${CGSI_GSOAP_LOCATION:-/usr})
     
    AC_MSG_RESULT([checking for cgsi-gsoap... ])

    if test -n "with_cgsi_gsoap_location" ; then
        CGSI_GSOAP_LOCATION="$with_cgsi_gsoap_location"
        CGSI_GSOAP_CFLAGS="-I$with_cgsi_gsoap_location/include"
        if test "x$host_cpu" = "xx86_64"; then
            ac_cgsi_gsoap_ldlib="-L$with_cgsi_gsoap_location/lib64"
        else
            ac_cgsi_gsoap_ldlib="-L$with_cgsi_gsoap_location/lib"
        fi
        CGSI_GSOAP_VOMS_CPP_PTHR_LIBS="$ac_cgsi_gsoap_ldlib -lcgsi_plugin_voms_cpp"
    else
		CGSI_GSOAP_LOCATION=""
		CGSI_GSOAP_CFLAGS=""
		CGSI_GSOAP_VOMS_CPP_PTHR_LIBS=""
    fi
       
    AC_MSG_RESULT([yes])
    CGSI_GSOAP_CFLAGS="$CGSI_GSOAP_CFLAGS -DGSI_PLUGINS"

    AC_MSG_RESULT([CGSI_GSOAP_LOCATION set to $CGSI_GSOAP_LOCATION])
    AC_MSG_RESULT([CGSI_GSOAP_CFLAGS set to $CGSI_GSOAP_CFLAGS])
    AC_MSG_RESULT([CGSI_GSOAP_VOMS_CPP_PTHR_LIBS set to $CGSI_GSOAP_VOMS_CPP_PTHR_LIBS])
    
    AC_SUBST(CGSI_GSOAP_LOCATION)
    AC_SUBST(CGSI_GSOAP_CFLAGS)
    AC_SUBST(CGSI_GSOAP_VOMS_CPP_PTHR_LIBS)
])
