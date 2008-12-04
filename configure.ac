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

# Release number
AC_SUBST([STORM_FRONTEND_RELEASE_NUMBER], [00])

AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([src/frontend/storm-frontend.cpp])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE()

# Installation prefix
AC_PREFIX_DEFAULT([/opt/storm/frontend])

if test "x${prefix}" = "xNONE"; then
  prefix=$ac_default_prefix
  ac_configure_args="$ac_configure_args --prefix $prefix"
fi

AC_SUBST([PREFIX], [$prefix])

DEFAULT_STORM_DIR=/home/storm
DEFAULT_GSOAP_DIR=/usr/local/gsoap-2.7/bin
DEFAULT_XMLRPC_C_DIR=$DEFAULT_STORM_DIR/xmlrpc-c_new
DEFAULT_CURL_LOCATION=$DEFAULT_STORM_DIR/curl_new
DEFAULT_GLITE_LOCATION=/opt/glite
DEFAULT_MYSQL_DIR=/usr
DEFAULT_RPM_DIR=`pwd`

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
if test -z $STORM_DIR; then
    STORM_DIR=$DEFAULT_STORM_DIR
fi
if test ! \( -d $STORM_DIR \); then
    AC_MSG_ERROR([no such directory "$STORM_DIR"])
fi
AC_MSG_RESULT(["$STORM_DIR" found])

AC_SUBST([STORM_DIR])
AC_SUBST([STORM_CFLAGS], [" -I$STORM_DIR/include "])

# ========================================================================
# Boost
# ========================================================================
AC_SUBST([BOOST_LDADD], ["/usr/lib/libboost_program_options.a"])

# ========================================================================
# Globus libraries
# ========================================================================
AC_SUBST([GLOBUS_PKG_LIBS], [" -L/opt/globus/lib -lglobus_gss_assist_gcc32dbgpthr "])

# ========================================================================
# GSOAP
# ========================================================================
AC_MSG_CHECKING([for gSOAP directory])
if test -z $GSOAP_DIR; then
    GSOAP_DIR=$DEFAULT_GSOAP_DIR
fi
if test ! \( -d $GSOAP_DIR \); then
    AC_MSG_ERROR([no such directory "$GSOAP_DIR"])
fi
AC_MSG_RESULT(["$GSOAP_DIR" found])

AC_SUBST([GSOAP_DIR])
AC_SUBST([GSOAP_GENERATED_CODE_DIR_NAME], [gsoap-generated])

# ========================================================================
# gSOAP GSI plugin
# ========================================================================
AC_SUBST([GSOAPGSI_DIR], [])
AC_SUBST([GSOAPGSI_CFLAGS], [" -DGSI_PLUGINS "])
AC_SUBST([GSOAPGSI_LDADD], [" -lcgsi_plugin_voms_gsoap_2.7_gcc32dbgpthr "])

# ========================================================================
# MySQL 
# ========================================================================
AC_MSG_CHECKING([for MySQL directory])
if test -z $MYSQL_DIR; then
    MYSQL_DIR=$DEFAULT_MYSQL_DIR
fi
if test ! \( -d $MYSQL_DIR \); then
    AC_MSG_ERROR([no such directory "$MYSQL_DIR"])
fi
AC_MSG_RESULT(["$MYSQL_DIR" found])

AC_SUBST([MYSQL_CFLAGS], [" -I$MYSQL_DIR/include/mysql -DUSE_MYSQL" ])
AC_SUBST([MYSQL_LDFLAGS], [" -L$MYSQL_DIR/lib/mysql" ])
AC_SUBST([MYSQL_LDADD], [-lmysqlclient_r])

# ========================================================================
# gLite
# ========================================================================
AC_MSG_CHECKING([gLite directory])
if test -z $GLITE_LOCATION; then
    GLITE_LOCATION=$DEFAULT_GLITE_LOCATION
fi
if test ! \( -d $GLITE_LOCATION \); then
    AC_MSG_ERROR([no such directory "$GLITE_LOCATION"])
fi
AC_MSG_RESULT(["$GLITE_LOCATION" found])
AC_SUBST([GLITE_LDFLAGS], [" -L$GLITE_LOCATION/lib "])

# ========================================================================
# XMLRPC-C
# ========================================================================
AC_MSG_CHECKING([for XMLRPC-C directory])
if test -z $XMLRPC_C_DIR; then
    XMLRPC_C_DIR=$DEFAULT_XMLRPC_C_DIR
fi
if test ! \( -d $XMLRPC_C_DIR \); then
    AC_MSG_ERROR([no such directory "$XMLRPC_C_DIR"])
fi
AC_MSG_RESULT(["$XMLRPC_C_DIR" found])

AC_SUBST([XMLRPC_C_CFLAGS], [" -I$XMLRPC_C_DIR/include "])
AC_SUBST([XMLRPC_C_LIB_LOCATION], ["$XMLRPC_C_DIR/lib"])
# Now it is linked statically
AC_SUBST([XMLRPC_C_LDADD], [" -L$XMLRPC_C_DIR/lib -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok "])
#AC_SUBST([XMLRPC_C_LDADD], [""])

#XMLRPC_C_LDADD="`$XMLRPC_C_DIR/bin/xmlrpc-c-config --ldadd` `$XMLRPC_C_DIR/bin/xmlrpc-c-config client --ldadd`"
#XMLRPC_C_CFLAGS="`$XMLRPC_C_DIR/bin/xmlrpc-c-config --cflags`"
#XMLRPC_C_LDADD="-L/home/storm/curl_new/lib -lcurl -L/home/storm/xmlrpc-c_new/lib -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok"

# ========================================================================
# Curl
# ========================================================================
AC_MSG_CHECKING([for Curl directory])
if test -z $CURL_LOCATION; then
    CURL_LOCATION=$DEFAULT_CURL_LOCATION
fi
if test ! \( -d $CURL_LOCATION \); then
    AC_MSG_ERROR([no such directory "$CURL_LOCATION"])
fi
AC_MSG_RESULT(["$CURL_LOCATION" found])
AC_SUBST([CURL_LIB_LOCATION], [$CURL_LOCATION/lib])
AC_SUBST([CURL_LDADD], [" -L$CURL_LOCATION/lib -lcurl "])
#AC_SUBST([CURL_LDADD], [" -L$CURL_LOCATION/lib -lwwwapp -lwwwcache -lwwwcore -lwwwdir -lwwwfile -lwwwftp -lwwwgopher -lwwwhtml -lwwwinit -lwwwmime -lwwwmux -lwwwnews -lwwwstream -lwwwtelnet -lwwwzip -lwwwxml -lwwwutils -lwwwtrans -lwwwhttp -lmd5"])

# ========================================================================
# RPM directory
# ========================================================================
AC_MSG_CHECKING([for RPM directory])
if test -z $RPMD; then
    RPMD=$DEFAULT_RPM_DIR
fi
if test ! \( -d $RPMD \); then
    AC_MSG_ERROR([no such directory "$RPMD"])
fi
AC_MSG_RESULT(["$RMPD" found])
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
    src/frontend/frontend_version.h
    src/frontend/srm_server.h \
    storm-frontend.spec \
    clean.sh])

AC_OUTPUT

