# $Id$
#*********************************************************************
#*
#* Authors: Flavia Donno <Flavia.Donno@cern.ch>
#*
#* Copyright 2004 INFN FIRB Project
#*           http://grid-it.cnaf.infn.it/
#*
#*
#* Date: 01 March 2005
#*
#*********************************************************************

MAINTAINERCLEANFILES = Makefile.in

GENERATED_GSOAP_INCLUDES = $(top_builddir)/wsdl/@GSOAP_GENERATED_CODE_DIR_NAME@

INCLUDES = -I$(top_builddir)/src/frontend \
    -I$(top_srcdir)/src/frontend \
    -I$(GENERATED_GSOAP_INCLUDES) \
    -I/usr/include/mysql \
    -I@BOOST_INCLUDE_DIR@

#noinst_INCLUDES = storm_mysql.h

noinst_LTLIBRARIES = libstormdb.la

libstormdb_la_SOURCES = storm_mysql_ifce.cpp mysql_query.cpp mysql_query.hpp storm_mysql.h

