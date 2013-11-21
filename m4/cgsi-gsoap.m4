dnl Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2013.
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

AC_DEFUN([AC_CGSI_GSOAP],
[
	AC_CHECK_HEADER([cgsi_plugin.h],,[AC_MSG_ERROR("Header not found: cgsi_plugin.h. Please install the cgsi-gsoap plugin development package.")])		
	
	dnl LIBS_SAVE=$LIBS
	dnl LIBS="$GSOAP_SSL_PP_LIBS"
	dnl AC_CHECK_LIB([cgsi_plugin_voms_cpp],[server_cgsi_plugin],,[AC_MSG_ERROR("cgsi_plugin_voms check failed. Please install the cgsi-gsoap plugin development package.")])
	dnl LIBS=$LIBS_SAVE
	dnl
	dnl
	
	CGSI_GSOAP_PLUGIN_LIBS="-lcgsi_plugin_voms_cpp"
	AC_SUBST(CGSI_GSOAP_PLUGIN_LIBS)
])

