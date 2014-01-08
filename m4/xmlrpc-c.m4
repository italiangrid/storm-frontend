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

AC_DEFUN([AC_XMLRPC_C],
[
	AC_ARG_WITH(xmlrpc_includes,
		[  --with-xmlrpc_includes=XMLRPC_INCLUDES	Sets a non-standard location for xmlrpc-c includes],
		[with_xmlrpc_includes="$withval"],
		[with_xmlrpc_includes="/usr/include/storm"])

	AC_ARG_WITH(xmlrpc_libs,
		[  --with-xmlrpc_libs=XMLRPC_LIBS	Sets a non-standard location for xmlrpc-c librarires],
		[with_xmlrpc_libs="$withval"],
		[with_xmlrpc_libs="/usr/lib64/storm"])


	XMLRPC_CFLAGS="-I$with_xmlrpc_includes"
	XMLRPC_LIBS="-L$with_xmlrpc_libs -lxmlrpc_client -lxmlrpc_xmlparse -lxmlrpc -lxmlrpc_util -lxmlrpc_xmltok"

	CPPFLAGS_SAVE=$CPPFLAGS
	CFLAGS_SAVE=$CFLAGS
	CPPFLAGS=$XMLRPC_CFLAGS
	CFLAGS=$XMLRPC_CFLAGS
	AC_CHECK_HEADER([xmlrpc-c/base.h],,[AC_MSG_ERROR("Header not found: xmlrpc-c/base.h. Please install the xmlrpc-c development package.")])
	CPPFLAGS=$CPPFLAGS_SAVE
	CFLAGS=$CFLAGS_SAVE

	CFLAGS_SAVE=$CFLAGS
	CPPFLAGS_SAVE=$CPPFLAGS
	LIBS_SAVE=$LIBS
	CFLAGS=$XMLRPC_LIBS
	AC_CHECK_LIB([xmlrpc],[xmlrpc_env_init],,[AC_MSG_ERROR("xmlrpc library check failed. Please install the xmlrpc-c development package.")])
	LIBS=$LIBS_SAVE
	CFLAGS=$CFLAGS_SAVE

	AC_SUBST(XMLRPC_CFLAGS)
	AC_SUBST(XMLRPC_LIBS)
])
