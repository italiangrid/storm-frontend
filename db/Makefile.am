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

INCLUDES = -I. -I$(top_srcdir)/srmv2 -I$(top_srcdir)/build/srmv2 \
		-I@GSOAP_DIR@ \
		-I@GSOAPGSI_DIR@/include \
        @MYSQL_CFLAGS@

noinst_LTLIBRARIES = libstormdb.la

$(top_srcdir)/srmv2/srmv2H.h:
	make -C ../srmv2 srmv2H.h

storm_mysql_ifce.cpp: $(top_srcdir)/srmv2/srmv2H.h

libstormdb_la_SOURCES = storm_mysql_ifce.cpp mysql_query.cpp mysql_query.hpp

dist-hook:
	cd $(distdir)
	for file in $(MAINTAINERCLEANFILES); do \
           /bin/rm $(distdir)/$$file; \
        done

