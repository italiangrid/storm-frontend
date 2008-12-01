# $Id: configure.in 4387 2008-03-12 17:05:07Z alb $
#*********************************************************************
#*
#* Authors: Alberto Forti <alberto.forti@cnaf.infn.it>
#*
#* Copyright 2004 INFN
#*
#* Date: 20 November 2008
#*
#*********************************************************************

AC_INIT([storm-frontend], [1.4.0], [luca.magnoni@cnaf.infn.it])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([src/frontend/srmv2.cpp])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE()

# Release number
AC_SUBST([STORM_RELEASE], [00])

# Installation prefix
AC_PREFIX_DEFAULT([/opt/storm/frontend])

if test "x${prefix}" = "xNONE"; then
  prefix=$ac_default_prefix
  ac_configure_args="$ac_configure_args --prefix $prefix"
fi

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_LIB([pthread], [main])
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_FUNCS(getenv)

# ========================================================================
# Get GSOAP installation directory from command line option or environment
# variable GSOAP_DIR.
# ========================================================================
AC_MSG_CHECKING([for gsoap directory])
AC_ARG_WITH([gsoap-dir], [AS_HELP_STRING([--with-gsoap-dir], [gSOAP directory])],
            [gsoap_dir="$withval"], [gsoap_dir="$GSOAP_DIR"])
if test -d "$gsoap_dir" ; then
    AC_MSG_RESULT([$gsoap_dir found])
    AC_SUBST([GSOAP_DIR], [$gsoap_dir])
    AC_SUBST([GSOAP_GENERATED_CODE_DIR_NAME], [gsoap-generated])
else
    AC_MSG_ERROR([no such directory $gsoap_dir])
fi

# ========================================================================
# Get MySQL installation directory from command line option or environment
# variable MYSQL_DIR.
# ========================================================================
AC_MSG_CHECKING([for MySQL directory])
AC_ARG_WITH([mysql-dir], [AS_HELP_STRING([ --with-mysql-dir], [MySQL directory])],
            [mysql_dir="$withval"], [mysql_dir="$MYSQL_DIR"])
if test -d "$mysql_dir" ; then
    AC_MSG_RESULT([$mysql_dir found])
    AC_SUBST([MYSQL_DIR], [$mysql_dir])
    AC_SUBST([MYSQL_CFLAGS], [" -I${MYSQL_DIR}/include/mysql -DUSE_MYSQL" ])
    AC_SUBST([MYSQL_LDFLAGS], [" -L${MYSQL_DIR}/lib/mysql" ])
    AC_SUBST([MYSQL_LDADD], [-lmysqlclient_r])
else
    AC_MSG_RESULT([no such directory $mysql_dir])
fi





##############################################################################






dnl ===================
dnl Do you want GLOBUS?
dnl ===================
AC_ARG_ENABLE(globus,[ --enable-globus=<option> Default is yes],
            enable_globus="$enableval", enable_globus="yes")
if test "$enable_globus" = "yes" ; 
then

	dnl ===========================================================
	dnl Get Globus location from command line option or environment
	dnl variable GLOBUS_LOCATION.
	dnl ===========================================================
	AC_MSG_CHECKING([for globus location])
	AC_ARG_WITH(globus-location,[  --with-globus-location=<dir> Default is \$GLOBUS_LOCATION],
	            globus_location="$withval", globus_location="$GLOBUS_LOCATION")
	if test -d "$globus_location" ; then
	    AC_MSG_RESULT([found $globus_location])
	    GLOBUS_LOCATION_PATH=$globus_location
	else
	    AC_MSG_ERROR([no such directory $globus_location])
	fi

	dnl ===================================================
	dnl Determine globus flavor from command line option.
	dnl If command line option is not given, look in
	dnl $GLOBUS_LOCATION/include for a directory with "thr"
	dnl in the name.
	dnl ===================================================
	echo "Attempting to determine globus flavor..."
	AC_ARG_WITH(globus-flavor,[  --with-globus-flavor=flavor Default is gcc32dbgpthr
	],
	    globus_flavor=$withval, globus_flavor="gcc32dbgpthr")

	echo "globus flavor is \"$globus_flavor\""
       
        AC_SUBST([GLOBUS_FLAVOR], ["$globus_flavor"])
        AC_SUBST([GLOBUS_INCLUDES], [" -I/opt/globus/include/gcc32dbgpthr " ])
        AC_SUBST([GLOBUS_PKG_LIBS], [" -L/opt/globus/lib -lglobus_gss_assist_gcc32dbgpthr -lglobus_gssapi_gsi_gcc32dbgpthr -lglobus_gsi_proxy_core_gcc32dbgpthr -lglobus_gsi_credential_gcc32dbgpthr -lglobus_gsi_callback_gcc32dbgpthr -lglobus_oldgaa_gcc32dbgpthr -lglobus_gsi_sysconfig_gcc32dbgpthr -lglobus_gsi_cert_utils_gcc32dbgpthr -lglobus_openssl_gcc32dbgpthr -lglobus_proxy_ssl_gcc32dbgpthr -lglobus_openssl_error_gcc32dbgpthr -lglobus_common_gcc32dbgpthr" ])

        AC_SUBST([SSL_AND_CRYPTO], [" -L/opt/globus/lib -lssl_gcc32dbgpthr -lcrypto_gcc32dbgpthr "])

	dnl ==================================================================
	dnl Get GSI Plugins option
	dnl ==================================================================
	AC_MSG_CHECKING([for gsoap-gsi option])
	AC_ARG_ENABLE(gsoapgsi,[ --enable-gsoapgsi=<option> Default is yes],
	            gsoapgsiopt="$enableval", gsoapgsiopt="yes")
	if test "x$gsoapgsiopt" = "xyes" || test "x$gsoapgsiopt" = "xno"; then
	   AC_MSG_RESULT([$gsoapgsiopt])
	fi
	if test "x$gsoapgsiopt" = "xyes"; then
	   GSOAPGSI_CFLAGS="-DGSI_PLUGINS"
	else
	   GSOAPGSI_CFLAGS=""
	fi

	dnl ==================================================================
	dnl Get gLite location from command line option or
	dnl environment variable GLITE_LOCATION.
	dnl ==================================================================
	AC_MSG_CHECKING([for gLite location])
	AC_ARG_WITH(glite-location,[  --with-glite-location=<dir> Default is \$GLITE_LOCATION],
	            glite_location="$withval", glite_location="$GLITE_LOCATION")
	if test -d "$glite_location" ; then
		AC_MSG_RESULT([found $glite_location])
		GLITE_LOCATION_PATH=$glite_location
	else
		AC_MSG_ERROR([no such directory $glite_location])
	fi
	GLITE_LDFLAGS="-L${GLITE_LOCATION_PATH}/lib"

	dnl ==================================================================
	dnl Get GSI Plugins installation directory from command line option or
	dnl environment variable GSOAPGSI_DIR.
	dnl ==================================================================
	AC_MSG_CHECKING([for gsoap-gsi directory])
	AC_ARG_WITH(gsoapgsi-dir,[ | --with-gsoapgsi-dir=<dir> Default is \$GSOAPGSI_DIR],
	            gsoapgsi_dir="$withval", gsoapgsi_dir="$GSOAPGSI_DIR")
	if test -d "$gsoapgsi_dir"; then
	   AC_MSG_RESULT([$gsoapgsi_dir found])
	   GSOAPGSI_DIR="$gsoapgsi_dir"
	   GSOAPGSI_CFLAGS="-DGSI_PLUGINS"
	   GSOAPGSI_LDFLAGS="-L${GSOAPGSI_DIR}/lib"
	   GSOAPGSI_LDADD="-lcgsi_plugin_voms_gsoap_2.7_$GLOBUS_FLAVOR"
	else
	   AC_MSG_RESULT([no such directory $gsoap_dir])
	fi

else
    AC_SUBST([SSL_AND_CRYPTO], [" -lssl -lcrypto "])
fi

dnl ==================================================================
dnl Get gLite location from command line option or
dnl environment variable GLITE_LOCATION.
dnl ==================================================================
AC_MSG_CHECKING([for gLite location])
AC_ARG_WITH(glite-location,[  --with-glite-location=<dir> Default is \$GLITE_LOCATION],
            glite_location="$withval", glite_location="$GLITE_LOCATION")
if test -d "$glite_location" ; then
    AC_MSG_RESULT([found $glite_location])
    GLITE_LOCATION_PATH=$glite_location
else
    AC_MSG_ERROR([no such directory $glite_location])
fi
GLITE_LDFLAGS="-L${GLITE_LOCATION_PATH}/lib"

dnl ==================================================================
dnl Get XMLRPC-C installation directory from command line option or
dnl environment variable XMLRPC-C_DIR.
dnl ==================================================================
AC_MSG_CHECKING([for xmlrpc-c directory])
AC_ARG_WITH(xmlrpc-c-dir,[ | --with-xmlrpc-c-dir=<dir> Default is \$XMLRPC_C_DIR],
            xmlrpc_c_dir="$withval", xmlrpc_c_dir="$XMLRPC_C_DIR")
if test -d "$xmlrpc_c_dir" ; then
    AC_MSG_RESULT([$xmlrpc_c_dir found])
    XMLRPC_C_DIR="$xmlrpc_c_dir"
    #XMLRPC_C_LDADD="`$XMLRPC_C_DIR/bin/xmlrpc-c-config --ldadd` `$XMLRPC_C_DIR/bin/xmlrpc-c-config client --ldadd`"
    #XMLRPC_C_CFLAGS="`$XMLRPC_C_DIR/bin/xmlrpc-c-config --cflags`"
    #XMLRPC_C_LDADD="-L/home/storm/curl_new/lib -lcurl -L/home/storm/xmlrpc-c_new/lib -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok"
    # Now it is linked statically
    XMLRPC_C_LDADD=""
else
    AC_MSG_ERROR([no such directory $xmlrpc_c_dir])
fi
if test -d "$xmlrpc_c_dir/include" ; then
    XMLRPC_C_CFLAGS="-I/home/storm/xmlrpc-c_new/include"
else
    AC_MSG_ERROR([no such directory $xmlrpc_c_dir/include])
fi
if test -d "$xmlrpc_c_dir/lib" ; then
    XMLRPC_C_LIB_LOCATION="$xmlrpc_c_dir/lib"
else
    AC_MSG_ERROR([no such directory $xmlrpc_c_dir/lib])
fi

dnl ==================================================================
dnl Get CURL installation directory from command line option or
dnl environment variable CURL_LOCATION.
dnl ==================================================================
AC_MSG_CHECKING([for curl directory])
AC_ARG_WITH(curl-location,[ | --with-curl-location=<dir> Default is \$CURL_LOCATION],
            curl_location="$withval", curl_location="$CURL_LOCATION")
if test -d "$curl_location" ; then
    AC_MSG_RESULT([$curl_location found])
    if test -d "$curl_location/lib" ; then
        CURL_LIB_LOCATION="$curl_location/lib"
    else
        AC_MSG_ERROR([no such directory $curl_location/lib])
    fi
else
    AC_MSG_ERROR([no such directory $curl_location])
fi

dnl ==================================================================
dnl Get STORM needed stuff directory from command line option or
dnl environment variable STORM_DIR.
dnl ==================================================================
AC_MSG_CHECKING([for StoRM directory])
AC_ARG_WITH(storm-dir,[  --with-storm-dir=<dir> Default is \$STORM_DIR],
            storm_dir="$withval", storm_dir="$STORM_DIR")
if test -d "$storm_dir" ; then
   AC_MSG_RESULT([$storm_dir found])
   STORM_DIR="$storm_dir"
   STORM_CFLAGS="-I$STORM_DIR/include"
else
   AC_MSG_RESULT([no such directory $storm_dir])
fi

dnl ========================================================================
dnl Check if all sub-packages need to be built.
dnl ========================================================================
AC_MSG_CHECKING([for building all])
AC_ARG_ENABLE(all,[  --enable-all=<option> Default is yes],
            allopt="$enableval", allopt="yes")
if test "x$allopt" = "xyes" || test "x$allopt" = "xno"; then
    AC_MSG_RESULT([$allopt])
fi

dnl =============
dnl Check RPM dir
dnl =============
RPMD=`pwd`
AC_MSG_CHECKING([for directory where to create rpms])
AC_ARG_WITH(rpm-dir,[  --with-rpm-dir=<dir> Default is \$RPMD/],
            rpm_dir="$withval", rpm_dir="$RPMD")
if test -d "$rpm_dir" ; then
    AC_MSG_RESULT([found $rpm_dir])
    RPM_DIR=$rpm_dir
else
    AC_MSG_RESULT([no such directory $rpm_dir])
    AC_MSG_RESULT([the directory $rpm_dir will be created for you if possible])
    RPM_DIR=$rpm_dir
fi

PREFIX=$prefix

dnl =========================
dnl Define required variables
dnl =========================
AC_SUBST(RPM_DIR)
AC_SUBST(GLOBUS_LOCATION)
AC_SUBST(GLOBUS_FLAVOR)
AC_SUBST(GLOBUS_INCLUDES)
AC_SUBST(GLOBUS_LDFLAGS)
AC_SUBST(GLOBUS_PKG_LIBS)
AC_SUBST(GLITE_LOCATION)
AC_SUBST(GLITE_LDFLAGS)
AC_SUBST(STORM_DIR)
AC_SUBST(STORM_CFLAGS)
AC_SUBST(GSOAPGSI_DIR)
AC_SUBST(GSOAPGSI_CFLAGS)
AC_SUBST(GSOAPGSI_LDFLAGS)
AC_SUBST(GSOAPGSI_LDADD)
AC_SUBST(XMLRPC_C_DIR)
AC_SUBST(XMLRPC_C_LDADD)
AC_SUBST(XMLRPC_C_CFLAGS)
AC_SUBST(CURL_LIB_LOCATION)
AC_SUBST(XMLRPC_C_LIB_LOCATION)
AC_SUBST(PREFIX)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile \
    etc/Makefile \
    etc/init.d/storm-frontend \
    wsdl/Makefile \
    src/Makefile \
    src/db/Makefile \
    src/frontend/Makefile \
    src/frontend/srm_server.h \
    srmv2-storm-server.spec \
    clean.sh])

AC_OUTPUT

