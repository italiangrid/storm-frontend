# $Id$

#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

#*
#* Authors: Flavia Donno <Flavia.Donno@cern.ch>
#*
#* Copyright 2004 INFN FIRB Project
#*           http://grid-it.cnaf.infn.it/
#*
#*
#* Date: 01 March 2005
#*

noinst_LTLIBRARIES = libstormdb.la

libstormdb_la_SOURCES = \
    storm_mysql_ifce.cpp \
    mysql_query.cpp \
    mysql_query.hpp \
    storm_mysql.h
libstormdb_la_CPPFLAGS = \
    @GSOAP_CFLAGS@ \
    @BOOST_CFLAGS@ \
    @MYSQL_CFLAGS@ \
    $(API_INCLUDES)

GENERATED_GSOAP_INCLUDES = $(top_srcdir)/wsdl/@GSOAP_GENERATED_CODE_DIR_NAME@

API_INCLUDES = \
    -I$(top_srcdir)/src/frontend \
    -I$(GENERATED_GSOAP_INCLUDES) 

MAINTAINERCLEANFILES = Makefile.in

