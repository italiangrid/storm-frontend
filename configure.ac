#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

#*
#* Authors: Alberto Forti <alberto.forti@cnaf.infn.it>
#*
#* Copyright 2004 INFN
#*
#* Date: 20 November 2008
#*

AC_PREREQ([2.59])
AC_INIT([storm-frontend], [1.5.0])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/frontend/storm-frontend.cpp])

# Release number
AC_SUBST([STORM_FRONTEND_RELEASE_NUMBER], [1.sl4])

# Notices.
AC_COPYRIGHT([See LICENCE file for details])
AC_REVISION([$Revision: $])


#Environment.
WORKDIR=`pwd`
AC_SUBST(WORKDIR)

GSOAP_GENERATED_CODE_DIR_NAME=gsoap-generated
AC_SUBST(GSOAP_GENERATED_CODE_DIR_NAME)

# Installation prefix
#AC_PREFIX_DEFAULT([/opt/storm/frontend])

#if test "x${prefix}" = "xNONE"; then
#  prefix=$ac_default_prefix
#  ac_configure_args="$ac_configure_args --prefix $prefix"
#fi

DEFAULT_RPM_DIR=`pwd`

AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

AC_CHECK_LIB([pthread], [main])
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_FUNCS(getenv)

AC_STORM([])
AC_STORM_FRONTEND([])

have_curl=no
have_xmlrpc_c=no
have_curl_devel=no
have_xmlrpc_c_devel=no

AC_CURL([], have_curl=yes, have_curl=no)
AC_XMLRPC_C([], have_xmlrpc_c=yes, have_xmlrpc_c=no)
AC_CURL_DEVEL([], have_curl_devel=yes, have_curl_devel=no)
AC_XMLRPC_C_DEVEL([], have_xmlrpc_c_devel=yes, have_xmlrpc_c_devel=no)

AC_MSG_RESULT(["CURL found $have_curl"])
AC_MSG_RESULT(["XMLRPC_C found $have_xmlrpc_c"])
AC_MSG_RESULT(["CURL_DEVEL found $have_curl_devel"])
AC_MSG_RESULT(["XMLRPC_C_DEVEL found $have_xmlrpc_c_devel"])

have_boost=no

AC_BOOST([], have_boost=yes, have_boost=no)
AC_MSG_RESULT(["BOOST found $have_boost"])

AC_THREADPOOL([])

have_globus=no

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

AC_GLITE([])
AC_VOMS([])

AC_CGSI_GSOAP([2.7])

AC_GSOAP([])

have_mysql=no

AC_MYSQL([4.0.12], [5.0.80], have_mysql=yes, have_mysql=no)
AC_MSG_RESULT(["MYSQL found $have_mysql"])

# ========================================================================
# Boost
# ========================================================================
#AC_SUBST([BOOST_INCLUDE_DIR], ["/usr/include/boost-1_37"])
#AC_SUBST([BOOST_LDADD], [" /usr/${LIB_DIR_NAME}/libboost_program_options-${GCC_VERSION=}-mt-1_37.a /usr/${LIB_DIR_NAME}/libboost_thread-${GCC_VERSION=}-mt-1_37.a "])

# ========================================================================
# Globus libraries
# ========================================================================
#AC_SUBST([GLOBUS_LDFLAGS], [" -L/opt/globus/lib "])
#AC_SUBST([GLOBUS_LDADD], [" -lglobus_gss_assist_$FLAVOUR "])

# ========================================================================
# GSOAP
# ========================================================================
#AC_MSG_CHECKING([for gSOAP directory])
#if test -z $GSOAP_DIR; then
#    GSOAP_DIR=$DEFAULT_GSOAP_DIR
#fi
#if test ! \( -d $GSOAP_DIR \); then
#    AC_MSG_ERROR([no such directory "$GSOAP_DIR"])
#fi
#AC_MSG_RESULT(["$GSOAP_DIR" found])

#AC_SUBST([GSOAP_DIR])
#AC_SUBST([GSOAP_LDADD], [" $GSOAP_DIR/lib/libgsoap.a "])
#AC_SUBST([GSOAP_GENERATED_CODE_DIR_NAME], [gsoap-generated-sources])

# ========================================================================
# gLite
# ========================================================================
#AC_MSG_CHECKING([gLite directory])
#if test -z $GLITE_LOCATION; then
#    GLITE_LOCATION=$DEFAULT_GLITE_LOCATION
#fi
#if test ! \( -d $GLITE_LOCATION \); then
#    AC_MSG_ERROR([no such directory "$GLITE_LOCATION"])
#fi
#AC_MSG_RESULT(["$GLITE_LOCATION" found])
#AC_SUBST([GLITE_LDFLAGS], [" -L$GLITE_LOCATION/${LIB_DIR_NAME} "])

# ========================================================================
# gSOAP GSI plugin
# ========================================================================
#AC_SUBST([GSOAPGSI_DIR], [])
#AC_SUBST([GSOAPGSI_CFLAGS], [" -DGSI_PLUGINS "])
#AC_SUBST([GSOAPGSI_LDFLAGS], [" $GLITE_LDFLAGS "])
#AC_SUBST([GSOAPGSI_LDADD], [" -lcgsi_plugin_voms_gsoap_2.7_${FLAVOUR} "])
##AC_SUBST([GSOAPGSI_LDADD], [" -lglite_security_gsoap_plugin_276b_gcc32dbgpthr "])

# ========================================================================
# MySQL 
# ========================================================================
#AC_MSG_CHECKING([for MySQL directory])
#if test -z $MYSQL_DIR; then
#    MYSQL_DIR=$DEFAULT_MYSQL_DIR
#fi
#if test ! \( -d $MYSQL_DIR \); then
#    AC_MSG_ERROR([no such directory "$MYSQL_DIR"])
#fi
#AC_MSG_RESULT(["$MYSQL_DIR" found])

#AC_SUBST([MYSQL_CFLAGS], [" -I$MYSQL_DIR -DUSE_MYSQL " ])
#AC_SUBST([MYSQL_LDFLAGS], [" -L$MYSQL_DIR/${LIB_DIR_NAME}/mysql " ])
#AC_SUBST([MYSQL_LDADD], [" -lmysqlclient_r] ")

# ========================================================================
# XMLRPC-C
# ========================================================================
#AC_MSG_CHECKING([for XMLRPC-C directory])
#if test -z $XMLRPC_C_DIR; then
#    XMLRPC_C_DIR=$DEFAULT_XMLRPC_C_DIR
#fi
#if test ! \( -d $XMLRPC_C_DIR \); then
#    AC_MSG_ERROR([no such directory "$XMLRPC_C_DIR"])
#fi
#AC_MSG_RESULT(["$XMLRPC_C_DIR" found])

#AC_SUBST([XMLRPC_C_CFLAGS], [" -I$XMLRPC_C_DIR/include "])
#AC_SUBST([XMLRPC_C_LIB_LOCATION], ["$XMLRPC_C_DIR/${LIB_DIR_NAME}"])
#AC_SUBST([XMLRPC_C_LDFLAGS], [" -L$XMLRPC_C_DIR/${LIB_DIR_NAME} "])
#AC_SUBST([XMLRPC_C_LDADD], [" -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok "])

# ========================================================================
# Curl
# ========================================================================
#AC_MSG_CHECKING([for Curl directory])
#if test -z $CURL_LOCATION; then
#    CURL_LOCATION=$DEFAULT_CURL_LOCATION
#fi
#if test ! \( -d $CURL_LOCATION \); then
#    AC_MSG_ERROR([no such directory "$CURL_LOCATION"])
#fi
#AC_MSG_RESULT(["$CURL_LOCATION" found])
#AC_SUBST([CURL_LIB_LOCATION], ["$CURL_LOCATION/${LIB_DIR_NAME}"])
#AC_SUBST([CURL_LDFLAGS], [" -L$CURL_LOCATION/${LIB_DIR_NAME} "])
#AC_SUBST([CURL_LDADD], [" -lcurl "])

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

# Installation prefix
AC_PREFIX_DEFAULT([/opt/storm/frontend])

# Configuration items
AM_CONFIG_HEADER([src/autogen/config.h])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([etc/Makefile])
AC_CONFIG_FILES([etc/storm-frontend.conf.template])
AC_CONFIG_FILES([etc/logrotate/Makefile])
AC_CONFIG_FILES([etc/cron.d/storm-frontend.cron])
AC_CONFIG_FILES([etc/logrotate/Makefile])
AC_CONFIG_FILES([etc/logrotate/storm-frontend.logrotate])
AC_CONFIG_FILES([etc/init.d/Makefile])
AC_CONFIG_FILES([etc/init.d/storm-frontend])
AC_CONFIG_FILES([wsdl/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/db/Makefile])
AC_CONFIG_FILES([src/frontend/Makefile])
AC_CONFIG_FILES([src/frontend/frontend_version.h])
AC_CONFIG_FILES([storm-frontend.spec])

AC_OUTPUT

