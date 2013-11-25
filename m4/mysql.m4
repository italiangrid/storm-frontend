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

AC_DEFUN([AC_MYSQL],
[
    AC_ARG_WITH(mysql_config_location,
        [  --with-mysql_config_location=LOCATION        Location of the mysql_config program.],
            [with_mysql_config_location="$withval"],
        [with_mysql_config_location="/usr/lib64/mysql/mysql_config"])

    if ! test -e "$with_mysql_config_location"; then
        AC_MSG_ERROR("mysql_config not found at $with_mysql_config_location.")
    fi

    MYSQL_CFLAGS=$(${with_mysql_config_location} --cflags)
    MYSQL_LIBS=$(${with_mysql_config_location} --libs_r)

    CPPFLAGS_SAVE=$CPPFLAGS
    CFLAGS_SAVE=$CFLAGS
    CPPFLAGS=$MYSQL_CFLAGS
    CFLAGS=$MYSQL_CFLAGS
    AC_CHECK_HEADER([mysql/mysql.h],,[AC_MSG_ERROR("Header not found: mysql/mysql.h. Please install the mysql development package.")])
    CPPFLAGS=$CPPFLAGS_SAVE
    CFLAGS=$CFLAGS_SAVE

    CPPFLAGS_SAVE=$CPPFLAGS
    CFLAGS_SAVE=$CFLAGS
    LIBS_SAVE=$LIBS
    CFLAGS=$MYSQL_LIBS
    CPPFLAGS=$MYSQL_LIBS
    AC_CHECK_LIB([mysqlclient_r],[mysql_send_query],,[AC_MSG_ERROR("mysql library check failed. Please install the mysql development package.")])
    LIBS=$LIBS_SAVE
    CPPFLAGS=$CPPFLAGS_SAVE
    CFLAGS=$CFLAGS_SAVE

    AC_SUBST(MYSQL_CFLAGS)
    AC_SUBST(MYSQL_LIBS)
])
