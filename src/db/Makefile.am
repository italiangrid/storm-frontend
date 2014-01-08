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
#

noinst_LTLIBRARIES = libstormdb.la



libstormdb_la_SOURCES = \
    storm_mysql_ifce.cpp \
    mysql_query.cpp \
    mysql_query.hpp \
    storm_mysql.h

libstormdb_la_CFLAGS= @MYSQL_CFLAGS@
GENERATED_GSOAP_INCLUDES = $(top_srcdir)/wsdl/gsoap-autogen

INCLUDES = \
    -I$(top_srcdir)/src/frontend \
    -I$(GENERATED_GSOAP_INCLUDES) 

MAINTAINERCLEANFILES = Makefile.in
