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

dnl Usage:
dnl AC_XMLRPC_C(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for xmlrpc-c, and defines
dnl - XMLRPC_C_LIBS (linker flags, stripping and path)
dnl - XMLRPC_C_LOCATION

dnl AC_XMLRPC_C_DEVEL(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for xmlrpc-c-devel, and defines
dnl - XMLRPC_C_DEVEL_CFLAGS (compiler flags)
dnl - XMLRPC_C_DEVEL_LIBS (linker flags, stripping and path)
dnl - XMLRPC_C_DEVEL_LOCATION

AC_DEFUN([AC_LIBUUID_DEVEL],
[
    AC_ARG_WITH(libuuid-devel, 
           		 [  --with-libuuid-devel=Flag to require or not libuuid-devel at build time.],
                [echo mamma ; with_libuuid_devel="BuildRequires: libuuid-devel"], 
                [echo babbo ; with_libuuid_devel=${BUILD_REQUIRES_LIBUUID_DEVEL:-""}]
                )
    AC_MSG_CHECKING([if libuuid-devel is requested at build time])
    if test -n "$with_libuuid_devel" ; then
    	AC_MSG_RESULT(["requested"])
	else
		AC_MSG_RESULT(["not requested"])
    fi
    BUILD_REQUIRES_LIBUUID_DEVEL="$with_libuuid_devel"
    AC_SUBST(BUILD_REQUIRES_LIBUUID_DEVEL)
])
