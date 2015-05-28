# Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
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

AC_PREREQ([2.59])
AC_INIT([storm frontend server], [1.8.9], andrea.ceccanti@cnaf.infn.it)

AC_CONFIG_AUX_DIR([./aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/frontend/storm-frontend.cpp])

AM_INIT_AUTOMAKE

AC_COPYRIGHT([Copyright (c) 2004-2013 Istituto Nazionale di Fisica Nucleare 
See LICENSE file for details
])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_PROG_INSTALL
 
BOOST_REQUIRE([1.41.0])
BOOST_SMART_PTR
BOOST_PROGRAM_OPTIONS([mt-])
BOOST_THREADS([mt-])
BOOST_REGEX([mt-])

AC_XMLRPC_C
AC_MYSQL

PKG_CHECK_MODULES([LIBCURL],[libcurl])
PKG_CHECK_MODULES([LIBARGUS_PEP], [libargus-pep])
PKG_CHECK_MODULES([GLOBUS_COMMON],[globus-common])
PKG_CHECK_MODULES([GLOBUS_GRIDMAP_CALLOUT_ERROR],[globus-gridmap-callout-error])
PKG_CHECK_MODULES([GLOBUS_GSI_CREDENTIAL],[globus-gsi-credential])
PKG_CHECK_MODULES([GLOBUS_GSS_ASSIST],[globus-gss-assist])
PKG_CHECK_MODULES([OPENSSL],[openssl])
PKG_CHECK_MODULES([UUID],[uuid])

PKG_CHECK_MODULES([GSOAP],[gsoap >= 2.7])
PKG_CHECK_MODULES([GSOAP_PP],[gsoap++ >= 2.7])
PKG_CHECK_MODULES([GSOAP_SSL],[gsoapssl >= 2.7])
PKG_CHECK_MODULES([GSOAP_SSL_PP],[gsoapssl++ >= 2.7])

AC_CGSI_GSOAP
AC_WSDL2H


## Generating output
AM_CONFIG_HEADER([src/autogen/config.h])
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
