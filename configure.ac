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
AC_SUBST([STORM_RELEASE_NUMBER], [00])

# Installation prefix
AC_PREFIX_DEFAULT([/opt/storm/frontend])

if test "x${prefix}" = "xNONE"; then
  prefix=$ac_default_prefix
  ac_configure_args="$ac_configure_args --prefix $prefix"
fi

AC_SUBST([PREFIX], [$prefix])

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_LIB([pthread], [main])
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_FUNCS(getenv)

# ========================================================================
# StoRM directory
# ========================================================================
AC_MSG_CHECKING([for StoRM directory])
if test ! \( -d $STORM_DIR \); then
    STORM_DIR=/home/storm
fi
if test ! \( -d $STORM_DIR \); then
    AC_MSG_ERROR([no such directory "${STORM_DIR}"])
fi
AC_MSG_RESULT(["${STORM_DIR}" found])

AC_SUBST([STORM_DIR])
AC_SUBST([STORM_CFLAGS], [" -I$STORM_DIR/include "])

# ========================================================================
# Globus libraries
# ========================================================================
AC_SUBST([GLOBUS_LOCATION], ["/opt/globus"])
AC_SUBST([GLOBUS_FLAVOR], ["gcc32dbgpthr"])
AC_SUBST([GLOBUS_INCLUDES], [" -I/opt/globus/include/gcc32dbgpthr " ])
#AC_SUBST([GLOBUS_PKG_LIBS], [" -L/opt/globus/lib -lglobus_gssapi_gsi_gcc32dbgpthr -lglobus_gss_assist_gcc32dbgpthr " ])
AC_SUBST([GLOBUS_PKG_LIBS], ["" ])

AC_SUBST([SSL_AND_CRYPTO], [" -L/opt/globus/lib -lssl_gcc32dbgpthr -lcrypto_gcc32dbgpthr "])

# ========================================================================
# GSOAP
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
# gSOAP GSI plugin
# ========================================================================
AC_SUBST([GSOAPGSI_DIR], [])
AC_SUBST([GSOAPGSI_CFLAGS], [" -DGSI_PLUGINS "])
AC_SUBST([GSOAPGSI_LDFLAGS], [" -L$GSOAPGSI_DIR/lib "])
AC_SUBST([GSOAPGSI_LDADD], [" -lcgsi_plugin_voms_gsoap_2.7_$GLOBUS_FLAVOR "])

# ========================================================================
# MySQL 
# ========================================================================
AC_MSG_CHECKING([for MySQL directory])
AC_ARG_WITH([mysql-dir], [AS_HELP_STRING([ --with-mysql-dir], [MySQL directory])],
            [mysql_dir="$withval"], [mysql_dir="$MYSQL_DIR"])
if test -d "$mysql_dir" ; then
    AC_MSG_RESULT(["$mysql_dir" found])
    AC_SUBST([MYSQL_DIR], [$mysql_dir])
    AC_SUBST([MYSQL_CFLAGS], [" -I$MYSQL_DIR/include/mysql -DUSE_MYSQL" ])
    AC_SUBST([MYSQL_LDFLAGS], [" -L$MYSQL_DIR/lib/mysql" ])
    AC_SUBST([MYSQL_LDADD], [-lmysqlclient_r])
else
    AC_MSG_RESULT([no such directory "$mysql_dir"])
fi

# ========================================================================
# gLite
# ========================================================================
AC_SUBST([GLITE_LOCATION], ["/opt/glite"])
AC_SUBST([GLITE_LDFLAGS], [" -L$GLITE_LOCATION_PATH/lib "])

# ========================================================================
# XMLRPC-C
# ========================================================================
AC_MSG_CHECKING([for XMLRPC-C directory])
if test -z $XMLRPC_C_DIR; then
    XMLRPC_C_DIR=/home/storm/xmlrpc-c_new
fi
if test ! \( -d $XMLRPC_C_DIR \); then
    AC_MSG_ERROR([no such directory "${XMLRPC_C_DIR}"])
fi
AC_MSG_RESULT(["${XMLRPC_C_DIR}" found])

AC_SUBST([XMLRPC_C_CFLAGS], [" -I$XMLRPC_C_DIR/include "])
AC_SUBST([XMLRPC_C_LIB_LOCATION], ["$XMLRPC_C_DIR/lib"])
# Now it is linked statically
AC_SUBST([XMLRPC_C_LDADD], [""])

#XMLRPC_C_LDADD="`$XMLRPC_C_DIR/bin/xmlrpc-c-config --ldadd` `$XMLRPC_C_DIR/bin/xmlrpc-c-config client --ldadd`"
#XMLRPC_C_CFLAGS="`$XMLRPC_C_DIR/bin/xmlrpc-c-config --cflags`"
#XMLRPC_C_LDADD="-L/home/storm/curl_new/lib -lcurl -L/home/storm/xmlrpc-c_new/lib -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok"

# ========================================================================
# Curl
# ========================================================================
AC_MSG_CHECKING([for Curl directory])
if test -z $CURL_LOCATION; then
    CURL_LOCATION=$STORM_DIR/curl_new
fi
if test ! \( -d $CURL_LOCATION \); then
    AC_MSG_ERROR([no such directory "${CURL_LOCATION}"])
fi
AC_MSG_RESULT(["${CURL_LOCATION}" found])
AC_SUBST([CURL_LIB_LOCATION], [$CURL_LOCATION/lib])

# ========================================================================
# RPM directory
# ========================================================================
AC_MSG_CHECKING([for RPM directory])
if test -z $RPMD; then
    RPMD=`pwd`
fi
if test ! \( -d $RPMD \); then
    AC_MSG_ERROR([no such directory "${RPMD}"])
fi
AC_MSG_RESULT(["${RMPD}" found])
AC_SUBST([RPM_DIR], [$RPMD])


AC_CONFIG_HEADERS([config.h])

# Files ".in" and ".am" to be processed
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

