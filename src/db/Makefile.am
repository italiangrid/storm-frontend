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

EXTRA_DIST = storm_mysql_config.sh \
    storm_mysql_drop.sql \
    storm_mysql_grant.sql \
    storm_mysql_tbl.sql \
    instructions.txt \
    storm_database_config.sh \
    storm_mysql_update_from_1.0.0_to_1.3.2000.sql

stormdbdir = $(sysconfdir)/db
stormdb_DATA = storm_mysql_grant.sql \
               storm_mysql_tbl.sql \
               storm_mysql_config.sh \
               README \
               storm_database_config.sh \
               storm_mysql_update_from_1.0.0_to_1.3.2000.sql

GENERATED_GSOAP_INCLUDES = $(top_builddir)/wsdl/@GSOAP_GENERATED_CODE_DIR_NAME@

INCLUDES = -I$(top_builddir)/src/frontend \
    -I$(top_srcdir)/src/frontend \
    -I$(GENERATED_GSOAP_INCLUDES) \
    -I/usr/include/mysql \
    @MYSQL_CFLAGS@

noinst_LTLIBRARIES = libstormdb.la

libstormdb_la_SOURCES = storm_mysql_ifce.cpp mysql_query.cpp mysql_query.hpp

