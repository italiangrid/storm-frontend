# Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2024.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

AC_PREREQ([2.69])
AC_INIT([StoRM Frontend Server],[1.8.16])

AC_CONFIG_AUX_DIR([./aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/frontend/storm-frontend.cpp])

AM_INIT_AUTOMAKE

AC_COPYRIGHT([Copyright (c) 2004-2024 Istituto Nazionale di Fisica Nucleare 
See LICENSE file for details
])

LT_INIT
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_COMPILER

BOOST_REQUIRE
BOOST_SMART_PTR
BOOST_PROGRAM_OPTIONS([mt-])
BOOST_THREAD([mt-])
BOOST_REGEX([mt-])

AC_XMLRPC_C
AX_LIB_MYSQL
if test "x${MYSQL_VERSION}" = x; then
    AC_MSG_ERROR([MySQL is not available])
else
    AC_MSG_NOTICE([using MySQL version ${MYSQL_VERSION}])
fi

PKG_CHECK_MODULES([LIBCURL],[libcurl])
PKG_CHECK_MODULES([GLOBUS_COMMON],[globus-common])
PKG_CHECK_MODULES([OPENSSL],[openssl])
PKG_CHECK_MODULES([UUID],[uuid])

PKG_CHECK_MODULES([GSOAP],[gsoap])
PKG_CHECK_MODULES([GSOAP_PP],[gsoap++])
PKG_CHECK_MODULES([GSOAP_SSL],[gsoapssl])
PKG_CHECK_MODULES([GSOAP_SSL_PP],[gsoapssl++])

AC_CGSI_GSOAP
AC_WSDL2H


## Generating output
AC_CONFIG_HEADERS([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([etc/Makefile])
AC_CONFIG_FILES([etc/storm-frontend-server.conf.template])
AC_CONFIG_FILES([etc/logrotate/Makefile])
AC_CONFIG_FILES([etc/logrotate/storm-frontend-server])
AC_CONFIG_FILES([etc/init.d/Makefile])
AC_CONFIG_FILES([etc/init.d/storm-frontend-server])
AC_CONFIG_FILES([etc/sysconfig/Makefile])
AC_CONFIG_FILES([wsdl/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/db/Makefile])
AC_CONFIG_FILES([src/frontend/Makefile])
AC_CONFIG_FILES([src/frontend/frontend_version.h])

AC_OUTPUT
