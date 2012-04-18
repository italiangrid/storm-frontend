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

sbin_PROGRAMS = storm-frontend-server

storm_frontend_server_SOURCES = $(stormfrontend_others) storm-frontend.cpp
storm_frontend_server_CFLAGS = $(INCLUDES)
storm_frontend_server_CXXFLAGS = $(INCLUDES)
storm_frontend_server_LDADD = \
    @VOMS_LIBS@ \
    @MYSQL_LIBS@ \
    @STORM_XMLRPC_C_DEVEL_LIBS@ \
    @CURL_DEVEL_LIBS@ \
    -L/usr/lib64 -lglobus_gssapi_gsi -lglobus_gss_assist \
    @CGSI_GSOAP_VOMS_CPP_PTHR_LIBS@ \
    @GSOAP_SSL_PP_LIBS@ \
    @BOOST_PO_LIBS@ \
    @BOOST_THREAD_LIBS@ \
    $(top_builddir)/src/db/libstormdb.la \
    @ARGUS_LIBS@ \
    -lz -lm -luuid -lidn

noinst_INCLUDES = osdep.h \
  srm_server.h \
  storm_constants.h \
  storm_functions.h \
  get_socket_info.h \
  storm_limits.h \
  storm_util.h \
  srmlogit.h \
  ProtocolChecker.hpp \
  xmlrpc_decode.h \
  xmlrpc_encode.h \
  $(GENERATED_GSOAP_INCLUDES)/srm.v2.2.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2H.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2Stub.h

stormfrontend_others = Credentials.cpp Credentials.hpp \
    FullCredentials.cpp FullCredentials.hpp \
    CredentialException.hpp \
    get_socket_info.hpp get_socket_info.cpp \
    file_request.hpp filerequest_template.hpp \
    file_status.hpp \
    StatusTemplate.hpp FileStatusRequest.hpp \
    BolRequest.cpp BolRequest.hpp BolStatusRequest.cpp BolStatusRequest.hpp \
    PtpRequest.cpp PtpRequest.hpp PutStatusRequest.cpp PutStatusRequest.hpp \
    PtgRequest.cpp PtgRequest.hpp GetStatusRequest.cpp GetStatusRequest.hpp \
    CopyRequest.cpp CopyRequest.hpp CopyStatusRequest.cpp CopyStatusRequest.hpp \
    request/synch/SynchRequest.hpp \
    request/synch/MkdirRequest.cpp request/synch/MkdirRequest.hpp \
    request/synch/RmdirRequest.cpp request/synch/RmdirRequest.hpp \
    request/synch/RmRequest.cpp request/synch/RmRequest.hpp \
    request/synch/LsRequest.cpp request/synch/LsRequest.hpp \
    request/synch/StatusLsRequest.cpp request/synch/StatusLsRequest.hpp \
    request/synch/MvRequest.cpp request/synch/MvRequest.hpp \
    request/synch/SetPermissionRequest.cpp request/synch/SetPermissionRequest.hpp \
    request/synch/CheckPermissionRequest.cpp request/synch/CheckPermissionRequest.hpp \
    request/synch/GetPermissionRequest.cpp request/synch/GetPermissionRequest.hpp \
    request/synch/ReserveSpaceRequest.cpp request/synch/ReserveSpaceRequest.hpp \
    request/synch/StatusReserveSpaceRequest.cpp request/synch/StatusReserveSpaceRequest.hpp \
    request/synch/ReleaseSpaceRequest.cpp request/synch/ReleaseSpaceRequest.hpp \
    request/synch/UpdateSpaceRequest.cpp request/synch/UpdateSpaceRequest.hpp \
    request/synch/StatusUpdateSpaceRequest.cpp request/synch/StatusUpdateSpaceRequest.hpp \
    request/synch/GetSpaceMetaDataRequest.cpp request/synch/GetSpaceMetaDataRequest.hpp \
    request/synch/GetSpaceTokensRequest.cpp request/synch/GetSpaceTokensRequest.hpp \
    request/synch/ChangeSpaceForFilesRequest.cpp request/synch/ChangeSpaceForFilesRequest.hpp \
    request/synch/StatusChangeSpaceForFilesRequest.cpp request/synch/StatusChangeSpaceForFilesRequest.hpp \
    request/synch/ExtendFileLifeTimeInSpaceRequest.cpp request/synch/ExtendFileLifeTimeInSpaceRequest.hpp \
    request/synch/PurgeFromSpaceRequest.cpp request/synch/PurgeFromSpaceRequest.hpp \
    request/synch/ReleaseFilesRequest.cpp request/synch/ReleaseFilesRequest.hpp \
    request/synch/PutDoneRequest.cpp request/synch/PutDoneRequest.hpp \
    request/synch/AbortRequestRequest.cpp request/synch/AbortRequestRequest.hpp \
    request/synch/AbortFilesRequest.cpp request/synch/AbortFilesRequest.hpp \
    request/synch/SuspendRequestRequest.cpp request/synch/SuspendRequestRequest.hpp \
    request/synch/ResumeRequestRequest.cpp request/synch/ResumeRequestRequest.hpp \
    request/synch/ExtendFileLifeTimeRequest.cpp request/synch/ExtendFileLifeTimeRequest.hpp \
    request/synch/GetTransferProtocolsRequest.cpp request/synch/GetTransferProtocolsRequest.hpp \
    request/synch/PingRequest.cpp request/synch/PingRequest.hpp \
    ProtocolChecker.cpp ProtocolChecker.hpp \
    Authorization.cpp Authorization.hpp \
    AuthorizationException.hpp ArgusException.hpp \
    cgsi_plugin_int.h \
    gssapi_openssl.h globus_gsi_gss_constants.h \
    asynch.cpp asynch_status.cpp \
    Surl.hpp BolSurl.hpp PtgSurl.hpp PtpSurl.hpp CopySurl.hpp \
    Turl.hpp BolTurl.hpp PtgTurl.hpp PtpTurl.hpp CopyTurl.hpp \
    srmv2_GetRequestSummary.cpp srmv2_GetRequestTokens.cpp \
    FrontendConfiguration.cpp FrontendConfiguration.hpp \
    HttpPostClient.cpp HttpPostClient.h \
    DBConnectionPool.cpp DBConnectionPool.hpp \
    ThreadPool.hpp ThreadPool.cpp \
    srmlogit.cpp sql_string.hpp soap_util.hpp \
    srmv2_dirreq.c srmv2_xferreq.c \
    srmv2_permreq.c srmv2_spacereq.c \
    synch.cpp synch.hpp\
    xmlrpc_decode.c xmlrpc_encode.c \
    storm_util.c storm_utils.hpp \
    srmv2C.cc \
    srmv2Server.cc \
    Monitor.hpp InstrumentedMonitor.hpp \
    MonitorStub.hpp InstrumentedMonitorBuilder.hpp \
    MonitorHelper.hpp \
    Monitoring.hpp Monitoring.cpp MonitoringHelper.hpp \
    MonitorNotEnabledException.hpp \
    $(noinst_INCLUDES)

INCLUDES = \
    @BOOST_CFLAGS@ \
    -I$(top_srcdir)/src/db \
    -I$(GENERATED_GSOAP_INCLUDES) \
    @GLOBUS_THR_CFLAGS@ \
    @STORM_XMLRPC_C_DEVEL_CFLAGS@ \
    @CURL_DEVEL_CFLAGS@ \
    @VOMS_CFLAGS@ \
    @GSOAP_SSL_PP_CFLAGS@  \
    @CGSI_GSOAP_CFLAGS@ \
    @MYSQL_CFLAGS@ \
    @ARGUS_CFLAGS@ \
    $(MTC_CFLAGS)

#GSOAP_SSL_CFLAGS 

MAINTAINERCLEANFILES = Makefile.in \
    srmv2C.cc \
    srmv2Server.cc

srmv2C.cc:
	@echo "Copying file \"srmv2C.cc\" from $(GENERATED_GSOAP_INCLUDES)"
	/bin/cp $(GENERATED_GSOAP_INCLUDES)/srmv2C.cc .

srmv2Server.cc:
	@echo "Copying file \"srmv2Server.cc\" from $(GENERATED_GSOAP_INCLUDES)"
	/bin/cp $(GENERATED_GSOAP_INCLUDES)/srmv2Server.cc .

distclean-local:
	rm -f srmv2C.cc srmv2Server.cc
