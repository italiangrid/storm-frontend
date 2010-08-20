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

MTC_CFLAGS = -DCTHREAD_LINUX -D_THREAD_SAFE -D_REENTRANT

GENERATED_GSOAP_INCLUDES = $(top_srcdir)/wsdl/@GSOAP_GENERATED_CODE_DIR_NAME@

sbin_PROGRAMS = storm-frontend

#noinst_LTLIBRARIES = libstormfrontend.la

# The prefix for SOURCES is the name of the program or library (in this case
# storm-frontend), including extension, where any dash ("-") and any period
# (".") is replaced by an underscore ("_").

storm_frontend_SOURCES = $(stormfrontend_others) storm-frontend.cpp
storm_frontend_LDFLAGS = \
    @GLITE_LDFLAGS@ 
storm_frontend_LDADD = \
    @VOMS_LIBS@ \
    @MYSQL_LIBS@ \
    @XMLRPC_C_DEVEL_LIBS@ \
    @CURL_DEVEL_LIBS@ \
    @GLOBUS_GSS_THR_LIBS@ \
    @CGSI_VOMS_GSOAP_PTHR_LIBS@ \
    @BOOST_PO_STATIC_LIBS@ \
    @BOOST_THREAD_STATIC_LIBS@ \
    $(top_builddir)/src/db/libstormdb.la \
    -lz -lm -luuid -lidn

#@GSOAP_STATIC_LIBS@ 

noinst_INCLUDES = osdep.h \
  srm_server.h \
  storm_constants.h \
  storm_functions.h \
  storm_limits.h \
  storm_util.h \
  srmlogit.h \
  xmlrpc_decode.h \
  xmlrpc_encode.h \
  $(GENERATED_GSOAP_INCLUDES)/srm.v2.2.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2H.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2Stub.h

stormfrontend_others = Credentials.cpp Credentials.hpp \
    file_request.hpp filerequest_template.hpp \
    status_template.hpp file_status.hpp \
    bol.cpp bol.hpp bol_status.cpp bol_status.hpp \
    ptp.cpp ptp.hpp put_status.cpp put_status.hpp \
    ptg.cpp ptg.hpp get_status.cpp get_status.hpp \
    copy.cpp copy.hpp copy_status.cpp copy_status.hpp \
    asynch.cpp asynch_status.cpp \
    srmv2_GetRequestSummary.cpp srmv2_GetRequestTokens.cpp \
    FrontendConfiguration.cpp FrontendConfiguration.hpp \
    HttpPostClient.cpp HttpPostClient.h \
    DBConnectionPool.cpp DBConnectionPool.hpp \
    ThreadPool.hpp Monitoring.hpp Monitoring.cpp \
    srmlogit.cpp sql_string.hpp soap_util.hpp \
    srmv2_dirreq.c srmv2_xferreq.c \
    srmv2_permreq.c srmv2_spacereq.c \
    synch.cpp \
    xmlrpc_decode.c xmlrpc_encode.c \
    storm_util.c storm_utils.hpp \
    srmv2C.c \
    srmv2Server.c \
    stdsoap2.c \
    $(noinst_INCLUDES)

INCLUDES = \
    -I$(top_srcdir)/src/db \
    -I$(GENERATED_GSOAP_INCLUDES) \
    @GLITE_CFLAGS@/glite/security/voms \
    @GLOBUS_THR_CFLAGS@ \
    @STORM_FRONTEND_CFLAGS@ \
    @XMLRPC_C_DEVEL_CFLAGS@ \
    @CURL_DEVEL_CFLAGS@ \
    @CGSI_GSOAP_CFLAGS@ \
    @GSOAP_CFLAGS@ \
    @MYSQL_CFLAGS@ \
    @BOOST_CFLAGS@ \
    $(MTC_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in \
    stdsoap2.c \
    srmv2C.c \
    srmv2Server.c

stdsoap2.c:
	@echo "Copying file \"stdsoap2.c\" from $(@GSOAP_LOCATION@)"
	/bin/cp @GSOAP_LOCATION@/../src/gSOAP-2.7.6b/soapcpp2/stdsoap2.c .

srmv2C.c:
	@echo "Copying file \"srmv2C.c\" from $(GENERATED_GSOAP_INCLUDES)"
	/bin/cp $(GENERATED_GSOAP_INCLUDES)/srmv2C.c .

srmv2Server.c:
	@echo "Copying file \"srmv2Server.c\" from $(GENERATED_GSOAP_INCLUDES)"
	/bin/cp $(GENERATED_GSOAP_INCLUDES)/srmv2Server.c .

distclean-local:
	rm -f stdsoap2.c srmv2C.c srmv2Server.c
