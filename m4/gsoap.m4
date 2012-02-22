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
dnl Define GSOAP
dnl
AC_DEFUN([AC_GSOAP],
[
    dnl
    dnl GSOAP Location
    dnl
    AC_ARG_WITH(gsoap-location,
	[  --with-gsoap-location=PFX     prefix where GSOAP is installed. (/usr)],
	[],
        with_gsoap_location=${GSOAP_LOCATION:-/usr})

    AC_MSG_RESULT([checking for gsoap... ])

    if test -n "$with_gsoap_location" ; then
 	GSOAP_LOCATION="$with_gsoap_location"
        if test "x$host_cpu" = "xx86_64"; then
            ac_gsoap_ldlib="-L$with_gsoap_location/lib64"
        else
            ac_gsoap_ldlib="-L$with_gsoap_location/lib"
        fi
	GSOAP_LIBS="$ac_gsoap_ldlib"
    else
	GSOAP_LOCATION=""
	GSOAP_LIBS=""
    fi

    dnl
    dnl GSOAP include
    dnl
    AC_ARG_WITH(gsoap-include,
        [  --with-gsoap-include=PFX     prefix where GSOAP is installed. (/usr/include)],
        [],
        with_gsoap_include=${GSOAP_INCLUDE_LOCATION:-/usr/include})

    AC_MSG_RESULT([checking for gsoap include... ])

    if test -n "$with_gsoap_include" ; then
        GSOAP_INCLUDE="-I$with_gsoap_include"
    else
        GSOAP_INCLUDE=""
    fi

    if test "x$host_cpu" = "xx86_64"; then
        flag="lib64"
    else
        flag="lib"
    fi
    
    for ll in gsoap++ gsoapck gsoapck++ gsoapssl gsoapssl++ gsoap
    do
        AC_MSG_RESULT([Checking $with_gsoap_location/$flag/lib$ll.so ])

        if test -f "$with_gsoap_location/$flag/lib$ll.so"
        then
            if test "x$ll" == "xgsoap" ; then            
                GSOAP_LIBS="$GSOAP_LIBS -lgsoap"
                GSOAP_CFLAGS="$GSOAP_INCLUDE -DWITH_IPV6 -DWITH_DOM"
                AC_MSG_RESULT([.... $GSOAP_LIBS $GSOAP_CFLAGS])
            fi
            if test "x$ll" == "xgsoap++" ; then
                GSOAP_PP_LIBS="$GSOAP_LIBS -lgsoap++"
                GSOAP_PP_CFLAGS="$GSOAP_INCLUDE -DWITH_IPV6 -DWITH_DOM"
                AC_MSG_RESULT([.... $GSOAP_PP_LIBS $GSOAP_PP_CFLAGS])
            fi
            if test "x$ll" == "xgsoapck" ; then
                GSOAP_CK_LIBS="$GSOAP_LIBS -lgsoapck"
                GSOAP_CK_CFLAGS="$GSOAP_INCLUDE -DWITH_COOKIES -DWITH_DOM"
            fi
            if test "x$ll" == "xgsoapck++" ; then
                GSOAP_CK_PP_LIBS="$GSOAP_LIBS -lgsoapck++"
                GSOAP_CK_PP_CFLAGS="$GSOAP_INCLUDE -DWITH_COOKIES -DWITH_DOM"
            fi
            if test "x$ll" == "xgsoapssl" ; then
                GSOAP_SSL_LIBS="$GSOAP_LIBS -lgsoapssl"
                GSOAP_SSL_CFLAGS="$GSOAP_INCLUDE -DWITH_IPV6 -DWITH_OPENSSL -DWITH_DOM -DWITH_COOKIES -DWITH_GZIP"
            fi
            if test "x$ll" == "xgsoapssl++" ; then
                GSOAP_SSL_PP_LIBS="$GSOAP_LIBS -lgsoapssl++"
                GSOAP_SSL_PP_CFLAGS="$GSOAP_INCLUDE -DWITH_IPV6 -DWITH_OPENSSL -DWITH_DOM -DWITH_COOKIES -DWITH_GZIP"
            fi
        fi
    done

    dnl
    dnl GSOAP WSDL2H Location
    dnl
    AC_ARG_WITH(gsoap-wsdl2h-location,
	[  --with-gsoap-wsdl2h-location=PFX     prefix where GSOAP wsdl2h is installed. (/usr)],
	[],
        with_gsoap_wsdl2h_location=${GSOAP_WSDL2H_LOCATION:-/usr})

    AC_MSG_RESULT([checking for gsoap wsdl2h... ])

    if test -n "$with_gsoap_wsdl2h_location" ; then
	GSOAP_WSDL2H_LOCATION="$with_gsoap_wsdl2h_location"
    elif test -n "$with_gsoap_location" ; then
	GSOAP_WSDL2H_LOCATION=""
    fi

    dnl
    dnl GSOAP Version
    dnl
    AC_ARG_WITH(gsoap-version,
	[  --with-gsoap-version=PFX     GSOAP version (2.7.13)],
	[],
        with_gsoap_version=${GSOAP_VERSION:-2.7.13})

    AC_MSG_RESULT([checking for gsoap version... ])
	
    if test -n "$with_gsoap_version" ; then
	GSOAP_VERSION="$with_gsoap_version"
    else
	GSOAP_VERSION=""
    fi
    
    dnl
    dnl GSOAP WSDL2H Version
    dnl
    AC_ARG_WITH(gsoap-wsdl2h-version,
	[  --with-gsoap-wsdl2h-version=PFX     WSDL2h version (1.2.13)],
	[],
        with_gsoap_wsdl2h_version=${GSOAP_WSDL2H_VERSION:-$GSOAP_VERSION})

    AC_MSG_RESULT([checking for gsoap WSDL2H version... ])
	
    if test -n "$with_gsoap_wsdl2h_version" ; then
 	GSOAP_WSDL2H_VERSION="$with_gsoap_wsdl2h_version"
    else
	GSOAP_WSDL2H_VERSION="$GSOAP_VERSION"
    fi
    
    dnl
    dnl Set GSOAP Version Number as a compiler Definition
    dnl
    if test -n "$GSOAP_VERSION" ; then
	EXPR='foreach $n(split(/\./,"'$GSOAP_VERSION'")){if(int($n)>99){$n=99;}printf "%02d",int($n);} print "\n";'
	GSOAP_VERSION_NUM=`perl -e "$EXPR"`
    else
	GSOAP_VERSION_NUM=000000
    fi
	
    GSOAP_CFLAGS="$GSOAP_CFLAGS -D_GSOAP_VERSION=0x$GSOAP_VERSION_NUM"

    dnl
    dnl Set GSOAP WSDL2H Version Number as a compiler Definition
    dnl
    if test -n "$GSOAP_WSDL2H_VERSION" ; then
	EXPR='foreach $n(split(/\./,"'$GSOAP_WSDL2H_VERSION'")){if(int($n)>99){$n=99;}printf "%02d",int($n);} print "\n";'
	GSOAP_WSDL2H_VERSION_NUM=`perl -e "$EXPR"`
    else
	GSOAP_WSDL2H_VERSION_NUM=000000
    fi;		
    GSOAP_CFLAGS="$GSOAP_CFLAGS -D_GSOAP_WSDL2H_VERSION=0x$GSOAP_WSDL2H_VERSION_NUM"

    AC_MSG_RESULT([GSOAP_LOCATION set to $GSOAP_LOCATION])
    AC_MSG_RESULT([GSOAP_WSDL2H_LOCATION set to $GSOAP_WSDL2H_LOCATION])
    AC_MSG_RESULT([GSOAP_CFLAGS set to $GSOAP_CFLAGS])
    AC_MSG_RESULT([GSOAP_VERSION set to $GSOAP_VERSION])
    AC_MSG_RESULT([GSOAP_WSDL2H_VERSION set to $GSOAP_WSDL2H_VERSION])
    AC_MSG_RESULT([GSOAP_WSDL2H_VERSION_NUM set to $GSOAP_WSDL2H_VERSION_NUM])

    AC_SUBST(GSOAP_LOCATION)
    AC_SUBST(GSOAP_WSDL2H_LOCATION)
    AC_SUBST(GSOAP_VERSION)
    AC_SUBST(GSOAP_VERSION_NUM)
    AC_SUBST(GSOAP_WSDL2H_VERSION)
    AC_SUBST(GSOAP_WSDL2H_VERSION_NUM)
    
    dnl
    dnl Test GSOAP Version
    dnl
    if test [ "$GSOAP_VERSION_NUM" -ge "020700"] ; then
    	AC_MSG_RESULT([GSOAP_VERSION is 2.7 or newer])
	GSOAP_MAIN_VERSION=`expr substr "$GSOAP_VERSION" 1 3`
	SOAPCPP2="$GSOAP_LOCATION/bin/soapcpp2"
	SOAPCPP2_FLAGS="-n -w"
	GSOAP_CFLAGS="$GSOAP_CFLAGS -DUSEGSOAP_2_7"
    else
    	AC_MSG_RESULT([Unsupported GSOAP_VERSION])
	GSOAP_MAIN_VERSION=""
	SOAPCPP2=""
	SOAPCPP2_FLAGS=""
	GSOAP_CFLAGS=""
    fi

    AC_SUBST(SOAPCPP2)
    AC_SUBST(SOAPCPP2_FLAGS)
	
    dnl
    dnl Test GSOAP WSDL2H Version
    dnl
    if test [ "$GSOAP_WSDL2H_VERSION_NUM" -ge "010200"] ; then
    	AC_MSG_RESULT([GSOAP_WSDL2H_VERSION is 1.2 or newer])
	GSOAP_WSDL2H_MAIN_VERSION=`expr substr "$GSOAP_WSDL2H_VERSION" 1 3`
	WSDL2H="$GSOAP_WSDL2H_LOCATION/bin/wsdl2h"
	GSOAP_CFLAGS="$GSOAP_CFLAGS -DUSEGSOAPWSDL2H_1_2"
        WSDL2H_FLAGS="-c -e -y $GSOAP_INCLUDE -I$with_gsoap_location/share/gsoap/import -z"
    else
    	AC_MSG_RESULT([Unsupported GSOAP_WSDL2H_VERSION])
	GSOAP_WSDL2H_MAIN_VERSION=""
	WSDL2H=""
        WSDL2H_FLAGS=""
    fi

    if test [ "$GSOAP_WSDL2H_VERSION_NUM" -ge "010216"] ; then
        AC_MSG_RESULT([GSOAP_WSDL2H_VERSION_NUM is 1.2.16 or newer])
        WSDL2H_BACK_COMPATIBILITY="-z1"
    else
        WSDL2H_BACK_COMPATIBILITY="-z"
    fi

    AC_MSG_RESULT([WSDL2H_BACK_COMPATIBILITY set to $WSDL2H_BACK_COMPATIBILITY])

    AC_SUBST(WSDL2H)
    AC_SUBST(WSDL2H_FLAGS)
    AC_SUBST(WSDL2H_BACK_COMPATIBILITY)

    AC_SUBST(GSOAP_CFLAGS)
    AC_SUBST(GSOAP_LIBS)
    AC_SUBST(GSOAP_PP_CFLAGS)
    AC_SUBST(GSOAP_PP_LIBS)
    AC_SUBST(GSOAP_CK_CFLAGS)
    AC_SUBST(GSOAP_CK_LIBS)
    AC_SUBST(GSOAP_CK_PP_CFLAGS)
    AC_SUBST(GSOAP_CK_PP_LIBS)
    AC_SUBST(GSOAP_SSL_CFLAGS)
    AC_SUBST(GSOAP_SSL_LIBS)
    AC_SUBST(GSOAP_SSL_PP_CFLAGS)
    AC_SUBST(GSOAP_SSL_PP_LIBS)

])

