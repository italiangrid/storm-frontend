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

AC_DEFUN([AC_WSDL2H],
[
	
	AC_ARG_WITH(gsoap-wsdl2h,
        	[  --with-gsoap-wsdl2h=CMD     the wsdl2h command that should be used. (/usr)],
        	[with_gsoap_wsdl2h="$withval"],
        	[with_gsoap_wsdl2h="/usr/bin/wsdl2h"])
	
	dnl SOAPCPP flags in addition to what is already set by gsoap
	SOAPCPP2="/usr/bin/soapcpp2"
	AC_SUBST(SOAPCPP2)
	
	dnl wsdl2h macros. we try to udnerstand which flags need to be used depending
	dnl on wsdl2h version
	AC_MSG_CHECKING([wsdl2h version])

	WSDL2H="$with_gsoap_wsdl2h"
	
	if ! test -e "$WSDL2H"; then
		AC_MSG_ERROR("wsdl2h executable: $WSDL2H does not exist.")
	fi

	if ! test -x "$WSDL2H"; then
		AC_MSG_ERROR("wsdl2h executable: $WSDL2H cannot be executed.")
	fi
	
	dnl The ridicoulous escaping with quadrigraph is needed as square brakets
	dnl confuse m4. 
	dnl 
	dnl @<:@  becomes [
	dnl @:>@  becomes ]
	wsdl2h_version=$($WSDL2H -help 2>&1 | grep 'wsdl2h release' | grep -o '@<:@0-9@:>@\.@<:@0-9@:>@\.@<:@0-9@:>@*$' | head -1)
	
	normalized_version=$(printf "%02d%02d%02d" $(echo $wsdl2h_version | tr '.' ' '))

	WSDL2H_FLAGS=""

	if test "$normalized_version" -ge "010216"; then
		WSDL2H_FLAGS="-z1"
	elif test "$normalized_version" -ge "010200"; then
		WSDL2H_FLAGS="-z"
	else
		AC_MSG_ERROR([unsupported wsdl2h version: $wsdl2h_version])
	fi

	AC_MSG_RESULT([$wsdl2h_version])
	AC_SUBST(WSDL2H)
	AC_SUBST(WSDL2H_FLAGS)
])
