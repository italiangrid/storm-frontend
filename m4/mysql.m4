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
dnl AC_MYSQL(MINIMUM-VERSION, MAXIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for mysql, and defines
dnl - MYSQL_CFLAGS (compiler flags)
dnl - MYSQL_LIBS (linker flags, stripping and path)
dnl prerequisites:

AC_DEFUN([AC_MYSQL],
[
    AC_ARG_WITH(mysql_prefix, 
       [  --with-mysql-prefix=PFX      prefix where MySQL is installed.],
       , 
       with_mysql_prefix=${MYSQL_INSTALL_PATH:-/usr})

    dnl Temporary checks
    if test "x$host_cpu" = "xi686" ; then
      with_mysql_prefix="$with_mysql_prefix"
    else
      a=`echo $with_mysql_prefix | awk -F '/' '{print $NF}'`
      if test "$a"  != "usr" ; then
        with_mysql_prefix="$with_mysql_prefix/usr"
      fi
    fi

    if test -n "$with_mysql_prefix" -a "$with_mysql_prefix" != "/usr" ; then
        MYSQL_BIN_PATH=$with_mysql_prefix/bin
    else	
        MYSQL_BIN_PATH=/usr/bin
    fi
    
    AC_ARG_WITH(mysql_devel_prefix, 
       [  --with-mysql-devel-prefix=PFX      prefix where MySQL devel is installed.],
       , 
       with_mysql_devel_prefix=${MYSQL_INSTALL_PATH:-/usr})

    dnl Temporary checks
    if test "x$host_cpu" = "xi686" ; then
      with_mysql_devel_prefix="$with_mysql_devel_prefix"
    else
      a=`echo $with_mysql_devel_prefix | awk -F '/' '{print $NF}'`
      if test "$a"  != "usr" ; then
        with_mysql_devel_prefix="$with_mysql_devel_prefix/usr"
      fi
    fi

    ac_cv_mysql_valid=no

    AC_PATH_PROG(PMYSQL,mysql,no,$MYSQL_BIN_PATH)

    if test "$PMYSQL" != "no" ; then
        if test "x$host_cpu" = "xx86_64"; then
            MYSQL_VERSION=`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$with_mysql_prefix/lib64/mysql/; $PMYSQL --version | cut -d' ' -f6 | tr -cd '0-9.'`
        else
            MYSQL_VERSION=`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$with_mysql_prefix/lib/mysql/; $PMYSQL --version | cut -d' ' -f6 | tr -cd '0-9.'`
	fi
    else
        AC_MSG_RESULT([Could not find mysql tool!])
    fi

    ac_save_CFLAGS=$CFLAGS
    ac_save_LIBS=$LIBS

    if test -n "$with_mysql_devel_prefix" ; then
	MYSQL_CFLAGS="-I$with_mysql_devel_prefix/include -I$with_mysql_devel_prefix/include/mysql"
        if test "x$host_cpu" = "xx86_64"; then
            MYSQL_LIBS="-L$with_mysql_devel_prefix/lib64 -L$with_mysql_devel_prefix/lib64/mysql"
        else
            MYSQL_LIBS="-L$with_mysql_devel_prefix/lib -L$with_mysql_devel_prefix/lib/mysql"
        fi
    else
        MYSQL_LIBS=""
        MYSQL_CFLAGS=""
    fi

    if test -n "$with_mysql_prefix" ; then
        if test "x$host_cpu" = "xx86_64"; then
            MYSQL_LIBS="$MYSQL_LIBS -L$with_mysql_prefix/lib64 -L$with_mysql_prefix/lib64/mysql"
        else
            MYSQL_LIBS="$MYSQL_LIBS -L$with_mysql_prefix/lib -L$with_mysql_prefix/lib/mysql"
        fi
    fi

    MYSQL_LIBS="$MYSQL_LIBS -lmysqlclient_r -lz"
        
    CFLAGS="$MYSQL_CFLAGS $CFLAGS"
    LIBS="$MYSQL_LIBS $LIBS"
    AC_MSG_RESULT([$CFLAGS $LIBS])

    if test "x$MYSQL_CFLAGS"="x" ; then
        AC_TRY_COMPILE([
 	      #include <mysql/mysql.h>
        ],[ MYSQL_FIELD mf ],
        [ ac_cv_mysql_valid=yes ], [ ac_cv_mysql_valid=no ])
    else
        AC_TRY_COMPILE([
          #include <mysql/mysql.h>
        ],[ MYSQL_FIELD mf ],
        [ ac_cv_mysql_valid=yes ], [ ac_cv_mysql_valid=no ])
    fi

    CFLAGS=$ac_save_CFLAGS
    LIBS=$ac_save_LIBS

    AC_MSG_RESULT([mysql status:  $ac_cv_mysql_valid])
    if test "x$ac_cv_mysql_valid" = "xno" ; then
        AC_MSG_RESULT([mysql status: **** suitable version NOT FOUND])
    else
        AC_MSG_RESULT([mysql status: **** suitable version FOUND])
    fi

    AC_MSG_RESULT([mysql *required* version between $1 and $2])
    AC_MSG_RESULT([mysql *found* version: $MYSQL_VERSION])

    if test "x$ac_cv_mysql_valid" = "xyes" ; then
        MYSQL_INSTALL_PATH=$with_mysql_prefix
        MYSQL_DEVEL_INSTALL_PATH=$with_mysql_devel_prefix
	ifelse([$3], , :, [$3])
    else
	MYSQL_CFLAGS=""
	MYSQL_LIBS=""
	ifelse([$4], , :, [$4])
    fi

    AC_SUBST(MYSQL_INSTALL_PATH)
    AC_SUBST(MYSQL_DEVEL_INSTALL_PATH)
    AC_SUBST(MYSQL_CFLAGS)
    AC_SUBST(MYSQL_LIBS)
])

