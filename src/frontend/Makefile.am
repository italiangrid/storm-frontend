# $Id$

MTCCFLAGS = -DCTHREAD_LINUX -D_THREAD_SAFE -D_REENTRANT

GENERATED_GSOAP_INCLUDES = $(top_builddir)/wsdl/@GSOAP_GENERATED_CODE_DIR_NAME@

MAINTAINERCLEANFILES = Makefile.in stdsoap2.c

#stormmandir = $(mandir)/man8
#stormman_DATA = srmv2.man

#EXTRA_DIST = srmv2.man

INCLUDES = -I$(top_srcdir)/src/db \
    -I$(GENERATED_GSOAP_INCLUDES) \
    -I@GSOAPGSI_DIR@/include \
    -I/opt/glite/include/glite/security/voms \
    -I/opt/globus/include/gcc32/ \
    @STORM_CFLAGS@ \
    @XMLRPC_C_CFLAGS@ \
    @GSOAPGSI_CFLAGS@ \
    -I@GSOAP_DIR@ \
    -I@BOOST_INCLUDE_DIR@

sbin_PROGRAMS = storm-frontend
# The prefix for SOURCES is the name of the program or library (in this case
# storm-frontend), including extension, where any dash ("-") and any period
# (".") is replaced by an underscore ("_").
storm_frontend_SOURCES = storm-frontend.cpp
storm_frontend_LDFLAGS = -L../db \
    @MYSQL_LDFLAGS@ \
    @GLITE_LDFLAGS@
storm_frontend_LDADD = -lstormfrontend -lstormdb \
    @GLITE_LDFLAGS@ -lvomsc \
    @MYSQL_LDADD@ \
    @XMLRPC_C_LDADD@ \
    @CURL_LDADD@ \
    @GSOAPGSI_LDADD@ \
    @GLOBUS_PKG_LIBS@ \
    @BOOST_LDADD@ \
    -lz -lm -luuid -lidn

noinst_INCLUDES = osdep.h srm_server.h storm_constants.h storm_functions.h \
  storm_limits.h storm_util.h srmlogit.h \
  xmlrpc_decode.h xmlrpc_encode.h \
  $(GENERATED_GSOAP_INCLUDES)/srm.v2.2.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2H.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2Stub.h

noinst_LTLIBRARIES = libstormfrontend.la

libstormfrontend_la_SOURCES =  Credentials.cpp Credentials.hpp \
    file_request.hpp srmv2_filerequest_template.hpp \
    srmv2_status_template.hpp file_status.hpp \
    bol.cpp bol.hpp bol_status.cpp bol_status.hpp \
    ptp.cpp ptp.hpp put_status.cpp put_status.hpp \
    ptg.cpp ptg.hpp get_status.cpp get_status.hpp \
    copy.cpp copy.hpp copy_status.cpp copy_status.hpp \
    asynch.cpp asynch_status.cpp \
    srmv2_GetRequestSummary.cpp srmv2_GetRequestTokens.cpp \
    FrontendConfiguration.cpp FrontendConfiguration.hpp \
    DBConnectionPool.cpp DBConnectionPool.hpp \
    srmlogit.cpp sql_string.hpp soap_util.hpp \
    srmv2_dirreq.c srmv2_xferreq.c \
    srmv2_permreq.c srmv2_spacereq.c \
    xmlrpc_decode.c xmlrpc_encode.c \
    storm_util.c \
    $(GENERATED_GSOAP_INCLUDES)/srmv2C.c \
    $(GENERATED_GSOAP_INCLUDES)/srmv2Server.c \
    stdsoap2.c \
    $(noinst_INCLUDES)

storm_frontend_DEPENDENCIES = libstormfrontend.la

stdsoap2.c:
	@echo "Copying file \"stdsoap2.c\" from $(@GSOAP_DIR@)"
	/bin/cp @GSOAP_DIR@/stdsoap2.c .

distclean-local:
	rm -f stdsoap2.c
