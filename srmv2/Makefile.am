# $Id$
#*********************************************************************
#*
#* Authors: Flavia Donno <Flavia.Donno@cern.ch>
#*
#* Authors: Antonio Messina <antonio.messina@ictp.it>
#*
#* Copyright 2006 ICTP EGRID Project
#*           http://www.egrid.it/
#*
#* Copyright 2004 INFN FIRB Project
#*           http://grid-it.cnaf.infn.it/
#*
#*********************************************************************

STORM_NSCONFIG = /etc/sysconfig/storm-frontend.nsconfig
LOGFILE = @prefix@/var/log/storm/srmv2log
MTCCFLAGS = -DCTHREAD_LINUX -D_THREAD_SAFE -D_REENTRANT

SOAPFILES = srmv2H.h srmv2Stub.h srmv2C.c srmv2Server.c srmv2Client.c srmSoapBinding.nsmap

MAINTAINERCLEANFILES = Makefile.in $(SOAPFILES) stdsoap2.c srmv2_init_script
TOCLEAN = $(SOAPFILES) srm.v2.2.h *.res.xml *.req.xml stdsoap2.c \
	  srmv2_init_script

stormmandir = $(mandir)/man8
stormman_DATA = srmv2.man
stormetcsysdir = $(sysconfdir)/init.d
stormetcsys_DATA = srmv2_init_script
stormetcsysconfdir = $(sysconfdir)/sysconfig
stormetcsysconf_DATA = storm-frontend.nsconfig
stormdbdir = $(sysconfdir)/db
stormdb_DATA = ../db/storm_mysql_grant.sql ../db/storm_mysql_tbl.sql ../db/storm_mysql_config.sh ../db/README \
  ../db/storm_database_config.sh ../db/storm_mysql_update_from_1.0.0_to_1.3.2000.sql

EXTRA_DIST = srm.v2.2.wsdl typemap.dat srmv2.man srmv2_init_script.in storm-frontend.nsconfig

INCLUDES = -I. -I$(top_srcdir)/db \
    @STORM_CFLAGS@ \
    @MYSQL_CFLAGS@ \
    @GSOAPGSI_CFLAGS@ \
    -I@GSOAP_DIR@ \
    -I@GSOAPGSI_DIR@/include \
    -I/opt/glite/include/glite/security/voms \
    -I/opt/globus/include/gcc32/ \
    @XMLRPC_C_CFLAGS@ \
    -DSTORM_NSCONFIG=\"$(STORM_NSCONFIG)\" \
    -DLOGFILE=\"$(LOGFILE)\" \
    -DUSE_MYSQL $(MTCCFLAGS)

# xmlrpc-c and curl are linked STATICALLY! ()
LIBS= @XMLRPC_C_LIB_LOCATION@/libxmlrpc_client.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc_xmlparse.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc_util.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc_xmltok.a @CURL_LIB_LOCATION@/libcurl.a

sbin_PROGRAMS = storm-frontend
# The prefix for SOURCES is the name of the program or library (in this case
# storm-frontend), including extension, where any dash ("-") and any period
# (".") is replaced by an underscore ("_").
storm_frontend_SOURCES = srmv2.cpp
storm_frontend_LDFLAGS = -L../db @GLOBUS_LDFLAGS@ -L/opt/glite/lib @SSL_AND_CRYPTO@ \
    @MYSQL_LDFLAGS@ @GSOAPGSI_LDFLAGS@ @GLITE_LDFLAGS@
storm_frontend_LDADD = -lstormfrontend -lstormdb -L/opt/glite/lib -lvomsc @MYSQL_LDADD@ \
    @XMLRPC_C_LDADD@ @GLOBUS_PKG_LIBS@ @GSOAPGSI_LDFLAGS@ @GSOAPGSI_LDADD@ \
    -lshift -lz -lm -luuid -lidn -lboost_program_options

noinst_INCLUDES = Cglobals.h Cgrp.h Cinit.h Cnetdb.h Cpool_api.h \
  Cpwd.h Cthread_api.h Cthread_flags.h osdep.h \
  serrno.h srm_server.h srm.v2.2.h srmv2H.h \
  srmv2Stub.h storm_api.h storm_constants.h storm_functions.h \
  storm_limits.h storm_util.h srmlogit.h \
  xmlrpc_decode.h xmlrpc_encode.h

noinst_LTLIBRARIES = libstormfrontend.la

libstormfrontend_la_SOURCES =  Credentials.cpp Credentials.hpp bol.cpp bol.hpp bol_status.cpp bol_status.hpp \
    srmv2_ptg.cpp ptg.cpp ptg.hpp \
    copy.cpp copy.hpp copy_status.cpp copy_status.hpp file_request.hpp \
    file_status.hpp get_status.cpp get_status.hpp ptp.cpp ptp.hpp put_status.cpp \
    srmv2_GetRequestSummary.cpp srmv2_GetRequestTokens.cpp \
    put_status.hpp soap_util.hpp sql_string.hpp srm_util.c srmlogit.c srmv2C.c \
    srmv2Server.c srmv2_bol.cpp srmv2_copy.cpp srmv2_dirreq.c srmv2_filerequest_template.hpp \
    srmv2_permreq.c srmv2_ptp.cpp srmv2_spacereq.c srmv2_status_bol.cpp \
    srmv2_status_copy.cpp srmv2_status_get.cpp srmv2_status_put.cpp srmv2_status_template.hpp \
    srmv2_xferreq.c storm_util.c xmlrpc_decode.c xmlrpc_encode.c stdsoap2.c $(noinst_INCLUDES) \
    FrontendOptions.cpp FrontendOptions.hpp

storm_frontend_DEPENDENCIES = libstormfrontend.la

$(SOAPFILES): srm.v2.2.h
	@GSOAP_DIR@/soapcpp2 -c -p srmv2 srm.v2.2.h

srm.v2.2.h: $(top_srcdir)/srmv2/srm.v2.2.wsdl
	@GSOAP_DIR@/wsdl2h -e -z -c -t $(top_srcdir)/srmv2/typemap.dat $(top_srcdir)/srmv2/srm.v2.2.wsdl

stdsoap2.c:
	@echo
	/bin/cp @GSOAP_DIR@/stdsoap2.c .

clean :
	/bin/rm -f $(TOCLEAN)

install-data-hook: $(stormman_DATA) $(stormetcsys_DATA) $(storetcsysconf_DATA)
	if test -d $(DESTDIR)$(stormmandir); then \
		$(mkinstalldirs) $(DESTDIR)$(stormmandir); \
	fi
	for file in $(stormman_DATA); do \
	   fname=`echo $$file | cut -d. -f1`; \
	   /bin/cp $$file $(DESTDIR)$(stormmandir)/$$fname.8; \
	   /bin/rm $(DESTDIR)$(stormmandir)/$$file; \
	done
	if [ ! -d $(DESTDIR)$(stormetcsysdir) ]; then \
		$(mkinstalldirs) $(DESTDIR)$(stormetcsysdir); \
	fi
	for file in $(stormetcsys_DATA); do \
	    /bin/cp $$file $(DESTDIR)$(stormetcsysdir)/storm-frontend; \
	    /bin/rm -f $(DESTDIR)$(stormetcsysdir)/$$file; \
	    /bin/chmod a+rx $(DESTDIR)$(stormetcsysdir)/storm-frontend; \
	done

	if [ ! -d $(DESTDIR)$(stormetcsysconfdir) ]; then \
		$(mkinstalldirs) $(DESTDIR)$(stormetcsysconfdir); \
	fi
	for file in $(stormetcsysconf_DATA); do \
		/bin/cp $$file $(DESTDIR)$(stormetcsysconfdir)/`basename $$file` ; \
	done
	if [ ! -d $(DESTDIR)$(stormdbdir) ]; then \
		$(mkinstalldirs) $(DESTDIR)$(stormdbdir); \
	fi
	for file in $(stormdb_DATA); do \
	    /bin/cp $$file $(DESTDIR)$(stormdbdir)/`basename $$file`; \
	done
	if ! [ -d $(DESTDIR)/var/log ]; then \
		$(mkinstalldirs) $(DESTDIR)$(prefix)/var/log ; \
	fi




dist-hook:
	cd $(distdir)
	for file in $(MAINTAINERCLEANFILES); do \
           /bin/rm -f $(distdir)/$$file; \
        done

srmv2.lo srmv2_dirreq.lo : $(SOAPFILES)

