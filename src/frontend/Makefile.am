# $Id$

MTCCFLAGS = -DCTHREAD_LINUX -D_THREAD_SAFE -D_REENTRANT

GENERATED_GSOAP_INCLUDES = $(top_builddir)/wsdl/@GSOAP_GENERATED_CODE_DIR_NAME@

MAINTAINERCLEANFILES = Makefile.in stdsoap2.c
TOCLEAN = stdsoap2.c

stormmandir = $(mandir)/man8
stormman_DATA = srmv2.man

EXTRA_DIST = srmv2.man

INCLUDES = -I$(top_srcdir)/src/db \
    -I$(GENERATED_GSOAP_INCLUDES) \
    -I@GSOAPGSI_DIR@/include \
    -I/opt/glite/include/glite/security/voms \
    -I/opt/globus/include/gcc32/ \
    @STORM_CFLAGS@ \
    @XMLRPC_C_CFLAGS@ \
    @GSOAPGSI_CFLAGS@ \
    -I@GSOAP_DIR@ \
    @MYSQL_CFLAGS@

# xmlrpc-c and curl are linked STATICALLY! ()
LIBS= @XMLRPC_C_LIB_LOCATION@/libxmlrpc_client.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc_xmlparse.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc_util.a @XMLRPC_C_LIB_LOCATION@/libxmlrpc_xmltok.a @CURL_LIB_LOCATION@/libcurl.a

sbin_PROGRAMS = storm-frontend
# The prefix for SOURCES is the name of the program or library (in this case
# storm-frontend), including extension, where any dash ("-") and any period
# (".") is replaced by an underscore ("_").
storm_frontend_SOURCES = srmv2.cpp
storm_frontend_LDFLAGS = -L../db @SSL_AND_CRYPTO@ \
    @GSOAPGSI_LDFLAGS@ @GLITE_LDFLAGS@ @MYSQL_LDFLAGS@ 
storm_frontend_LDADD = -lstormfrontend -lstormdb -L/opt/glite/lib -lvomsc @MYSQL_LDADD@ \
    @XMLRPC_C_LDADD@ @GSOAPGSI_LDFLAGS@ @GSOAPGSI_LDADD@ \
    -lshift -lz -lm -luuid -lidn -lboost_program_options

noinst_INCLUDES = Cglobals.h Cgrp.h Cinit.h Cnetdb.h Cpool_api.h \
  Cpwd.h Cthread_api.h Cthread_flags.h osdep.h \
  serrno.h srm_server.h storm_api.h storm_constants.h storm_functions.h \
  storm_limits.h storm_util.h srmlogit.h \
  xmlrpc_decode.h xmlrpc_encode.h \
  $(GENERATED_GSOAP_INCLUDES)/srm.v2.2.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2H.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2Stub.h

noinst_LTLIBRARIES = libstormfrontend.la

libstormfrontend_la_SOURCES =  Credentials.cpp Credentials.hpp bol.cpp bol.hpp bol_status.cpp bol_status.hpp \
    srmv2_ptg.cpp ptg.cpp ptg.hpp \
    copy.cpp copy.hpp copy_status.cpp copy_status.hpp file_request.hpp \
    file_status.hpp get_status.cpp get_status.hpp ptp.cpp ptp.hpp put_status.cpp \
    srmv2_GetRequestSummary.cpp srmv2_GetRequestTokens.cpp \
    put_status.hpp soap_util.hpp sql_string.hpp srm_util.c srmlogit.c \
    srmv2_bol.cpp srmv2_copy.cpp srmv2_dirreq.c srmv2_filerequest_template.hpp \
    srmv2_permreq.c srmv2_ptp.cpp srmv2_spacereq.c srmv2_status_bol.cpp \
    srmv2_status_copy.cpp srmv2_status_get.cpp srmv2_status_put.cpp srmv2_status_template.hpp \
    srmv2_xferreq.c storm_util.c xmlrpc_decode.c xmlrpc_encode.c stdsoap2.c \
    FrontendOptions.cpp FrontendOptions.hpp \
    $(GENERATED_GSOAP_INCLUDES)/srmv2C.c \
    $(GENERATED_GSOAP_INCLUDES)/srmv2Server.c \
    $(noinst_INCLUDES)

storm_frontend_DEPENDENCIES = libstormfrontend.la

stdsoap2.c:
	@echo "Copying file \"stdsoap2.c\" from $(@GSOAP_DIR@)"
	/bin/cp @GSOAP_DIR@/stdsoap2.c .

