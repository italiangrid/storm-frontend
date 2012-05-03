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
AC_INIT([storm frontend server], [1.8.0])
AC_CONFIG_AUX_DIR([./project])
#AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/frontend/storm-frontend.cpp])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

# Release number
osdist=`cat /etc/redhat-release | awk 'BEGIN {FS="release "} {print $2}'| awk 'BEGIN {FS="."} {print $1}'`
AC_SUBST([STORM_FRONTEND_SO], [sl$osdist])
AC_SUBST([STORM_FRONTEND_AGE_NUMBER], [9])

AC_SUBST([PACKAGE_PREFIX_NAME], [storm])
AC_SUBST([PACKAGE_LONG_NAME], [storm-frontend-server])
AC_SUBST([PACKAGE_SHORT_NAME], [frontend-server])

# Notices.
AC_COPYRIGHT([See LICENCE file for details])
AC_REVISION([$Revision: $])

#Environment.
WORKDIR=`pwd`
AC_SUBST(WORKDIR)

GSOAP_GENERATED_CODE_DIR_NAME=gsoap-generated
AC_SUBST(GSOAP_GENERATED_CODE_DIR_NAME)

DEFAULT_RPM_DIR=`pwd`

AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_PROG_INSTALL

AC_CHECK_LIB([pthread], [main])

AC_C_CONST
AC_TYPE_SIZE_T

# Set template config.h (required by stdsoap)
# This part should probably go in a macron in gsoap.m4 (something like AC_GSOAP_SRC)
# BEGIN:
AH_TEMPLATE(HAVE_FTIME)
AH_TEMPLATE(HAVE_GMTIME)
AH_TEMPLATE(HAVE_GMTIME_R)
AH_TEMPLATE(HAVE_GETTIMEOFDAY)
AH_TEMPLATE(HAVE_LOCALTIME_R)
AH_TEMPLATE(HAVE_MBTOWC)
AH_TEMPLATE(HAVE_STRTOF)
AH_TEMPLATE(HAVE_STRTOD)
AH_TEMPLATE(HAVE_STRTOL)
AH_TEMPLATE(HAVE_STRTOLL)
AH_TEMPLATE(HAVE_STRTOUL)
AH_TEMPLATE(HAVE_STRTOULL)
AH_TEMPLATE(HAVE_SSCANF)
AH_TEMPLATE(HAVE_WCTOMB)
AH_TEMPLATE(HAVE_TIMEGM)

# For IPv6
AH_TEMPLATE([WITH_IPV6], [For Ipv6 support])
AC_DEFINE(WITH_IPV6)

AC_CHECK_FUNC(ftime,           [AC_DEFINE(HAVE_FTIME)])
AC_CHECK_FUNC(gmtime,          [AC_DEFINE(HAVE_GMTIME)])
AC_CHECK_FUNC(gmtime_r,        [AC_DEFINE(HAVE_GMTIME_R)])
AC_CHECK_FUNC(gettimeofday,    [AC_DEFINE(HAVE_GETTIMEOFDAY)])
AC_CHECK_FUNC(localtime_r,     [AC_DEFINE(HAVE_LOCALTIME_R)])
AC_CHECK_FUNC(mbtowc,          [AC_DEFINE(HAVE_MBTOWC)])
AC_CHECK_FUNC(strtof,          [AC_DEFINE(HAVE_STRTOF)])
AC_CHECK_FUNC(strtod,          [AC_DEFINE(HAVE_STRTOD)])
AC_CHECK_FUNC(strtol,          [AC_DEFINE(HAVE_STRTOL)])
AC_CHECK_FUNC(strtoll,         [AC_DEFINE(HAVE_STRTOLL)])
AC_CHECK_FUNC(strtoul,         [AC_DEFINE(HAVE_STRTOUL)])
AC_CHECK_FUNC(strtoull,        [AC_DEFINE(HAVE_STRTOULL)])
AC_CHECK_FUNC(sscanf,          [AC_DEFINE(HAVE_SSCANF)])
AC_CHECK_FUNC(wctomb,          [AC_DEFINE(HAVE_WCTOMB)])
AC_CHECK_FUNC(timegm,          [AC_DEFINE(HAVE_TIMEGM)])
#:END

#AC_CHECK_FUNCS([random gettimeofday ftime localtime_r timegm gmtime gmtime_r memset select socket strchr strerror strrchr strstr strtol strtoul strtoll strtoull strtold strtod strtof strtold_l strtod_l strtof_l sscanf sscanf_l sprintf_l wctomb mbtowc getenv])
AC_CHECK_FUNCS([memmove memset mkdir putenv rmdir strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol])

AC_SUBST([STORM_FRONTEND_ARCH], [$host_cpu])

dnl this macro is used to get the arguments supplied
dnl to the configure script (./configure --enable-debug)
dnl Check if we have enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
default="no"
DEBUG=
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging
                       [default=$default]],, enable_debug=$default)
if test "x$enable_debug" = "xyes"; then
   CXXFLAGS="-g -O0"
   CFLAGS="-g -O0"
   AC_MSG_RESULT(yes)
else
   CXXFLAGS="-g -O2"
   CFLAGS="-g -O2"
   AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to enable walling)
wall="no"
DEBUG=
AC_ARG_ENABLE(wall, [  --enable-wall=[no/yes] turn on debugging
                       [wall=$default]],, enable_wall=$default)
if test "x$enable_wall" = "xyes"; then
   CXXFLAGS="$CXXFLAGS -Wall"
   CFLAGS="$CFLAGS -Wall"
   AC_MSG_RESULT(yes)
else
   CXXFLAGS="$CXXFLAGS"
   CFLAGS="$CFLAGS"
   AC_MSG_RESULT(no)
fi

AC_STORM([])

have_curl_devel=no
have_storm_xmlrpc_c_devel=no

AC_CURL([])
AC_STORM_XMLRPC_C([])
AC_CURL_DEVEL([], have_curl_devel=yes, have_curl_devel=no)
AC_STORM_XMLRPC_C_DEVEL([], have_storm_xmlrpc_c_devel=yes, have_storm_xmlrpc_c_devel=no)

AC_MSG_RESULT(["CURL found"])
AC_MSG_RESULT(["STORM_XMLRPC_C found"])
AC_MSG_RESULT(["CURL_DEVEL found $have_curl_devel"])
AC_MSG_RESULT(["STORM_XMLRPC_C_DEVEL found $have_storm_xmlrpc_c_devel"])
have_globus=no

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

have_mysql=no

AC_MYSQL([4.0.12], [5.1.52], have_mysql=yes, have_mysql=no)
AC_MSG_RESULT(["MYSQL found $have_mysql"])

AC_VOMS([])
AC_ARGUS([])
AC_CGSI_GSOAP([])
AC_GSOAP([])

AC_BOOST([sl$osdist])

if test "xsl$osdist" = "xsl6"; then
   BOOST_SUFFIX=""
elif test "xsl$osdist" = "xsl5"; then
   BOOST_SUFFIX="141"
else
   BOOST_SUFFIX=""
fi
AC_MSG_RESULT(["BOOST on sl$osdist $BOOST_SUFFIX"])
AC_SUBST(BOOST_SUFFIX)

AC_LIBUUID_DEVEL([])

# Configuration items
AM_CONFIG_HEADER([src/autogen/config.h])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([etc/Makefile])
AC_CONFIG_FILES([etc/storm-frontend-server.conf.template])
AC_CONFIG_FILES([etc/logrotate/Makefile])
AC_CONFIG_FILES([etc/logrotate/storm-frontend-server])
AC_CONFIG_FILES([etc/init.d/Makefile])
AC_CONFIG_FILES([etc/init.d/storm-frontend-server])
AC_CONFIG_FILES([wsdl/Makefile])

AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/db/Makefile])
AC_CONFIG_FILES([src/frontend/Makefile])
AC_CONFIG_FILES([src/frontend/frontend_version.h])
AC_CONFIG_FILES([storm-frontend.spec])

AC_OUTPUT

