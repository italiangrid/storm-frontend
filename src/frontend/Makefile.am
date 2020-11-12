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

GENERATED_GSOAP_INCLUDES = $(top_builddir)/wsdl/gsoap-autogen

sbin_PROGRAMS = storm-frontend-server
noinst_PROGRAMS = test_surl

test_surl_SOURCES = test_surl.c surl_normalizer.cc
test_surl_CFLAGS  = $(INCLUDES)
test_surl_LDFLAGS = @BOOST_REGEX_LIBS@


storm_frontend_server_SOURCES = $(stormfrontend_others) storm-frontend.cpp
storm_frontend_server_CFLAGS = $(INCLUDES)
storm_frontend_server_CXXFLAGS = $(INCLUDES)
storm_frontend_server_LDADD = \
    $(top_builddir)/src/db/libstormdb.la \
    @MYSQL_LIBS@ \
    @XMLRPC_LIBS@ \
    @LIBCURL_LIBS@ \
    @GLOBUS_COMMON_LIBS@ \
    @GLOBUS_GSI_CREDENTIAL_LIBS@ \
    @GLOBUS_GSS_ASSIST_LIBS@ \
    @GSOAP_SSL_PP_LIBS@ \
    @CGSI_GSOAP_PLUGIN_LIBS@ \
    @BOOST_PROGRAM_OPTIONS_LIBS@ \
    @BOOST_THREAD_LIBS@ \
    @BOOST_REGEX_LIBS@ \
    @LIBARGUS_PEP_LIBS@ \
    @UUID_LIBS@

storm_frontend_server_LDFLAGS= \
	@BOOST_THREAD_LDFLAGS@ \
	@BOOST_PROGRAM_OPTIONS_LDFLAGS@ \
	@BOOST_REGEX_LDFLAGS@

noinst_INCLUDES = osdep.h \
  srm_server.h \
  storm_functions.h \
  storm_util.h \
  srmlogit.h \
  ProtocolChecker.hpp \
  xmlrpc_decode.hpp \
  xmlrpc_encode.hpp \
  $(GENERATED_GSOAP_INCLUDES)/srm.v2.2.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2H.h \
  $(GENERATED_GSOAP_INCLUDES)/srmv2Stub.h

stormfrontend_others = storm_exception.hpp \
    Credentials.cpp Credentials.hpp \
    get_socket_info.hpp get_socket_info.cpp \
    base_request.hpp \
    file_request.hpp filerequest_template.hpp \
    surl_normalizer.h surl_normalizer.cc \
    request_id.cpp request_id.hpp \
    xmlrpc_client.cpp xmlrpc_client.hpp \
    StatusTemplate.hpp FileStatusRequest.hpp \
    BolRequest.cpp BolRequest.hpp BolStatusRequest.cpp BolStatusRequest.hpp \
    PtpRequest.cpp PtpRequest.hpp PutStatusRequest.cpp PutStatusRequest.hpp \
    PtgRequest.cpp PtgRequest.hpp GetStatusRequest.cpp GetStatusRequest.hpp \
    CopyRequest.cpp CopyRequest.hpp CopyStatusRequest.cpp CopyStatusRequest.hpp \
    SynchRequest.hpp \
    MkdirRequest.cpp MkdirRequest.hpp \
    RmdirRequest.cpp RmdirRequest.hpp \
    RmRequest.cpp RmRequest.hpp \
    LsRequest.cpp LsRequest.hpp \
    StatusLsRequest.cpp StatusLsRequest.hpp \
    MvRequest.cpp MvRequest.hpp \
    SetPermissionRequest.cpp SetPermissionRequest.hpp \
    CheckPermissionRequest.cpp CheckPermissionRequest.hpp \
    GetPermissionRequest.cpp GetPermissionRequest.hpp \
    ReserveSpaceRequest.cpp ReserveSpaceRequest.hpp \
    StatusReserveSpaceRequest.cpp StatusReserveSpaceRequest.hpp \
    ReleaseSpaceRequest.cpp ReleaseSpaceRequest.hpp \
    UpdateSpaceRequest.cpp UpdateSpaceRequest.hpp \
    StatusUpdateSpaceRequest.cpp StatusUpdateSpaceRequest.hpp \
    GetSpaceMetaDataRequest.cpp GetSpaceMetaDataRequest.hpp \
    GetSpaceTokensRequest.cpp GetSpaceTokensRequest.hpp \
    ChangeSpaceForFilesRequest.cpp ChangeSpaceForFilesRequest.hpp \
    StatusChangeSpaceForFilesRequest.cpp StatusChangeSpaceForFilesRequest.hpp \
    ExtendFileLifeTimeInSpaceRequest.cpp ExtendFileLifeTimeInSpaceRequest.hpp \
    PurgeFromSpaceRequest.cpp PurgeFromSpaceRequest.hpp \
    ReleaseFilesRequest.cpp ReleaseFilesRequest.hpp \
    PutDoneRequest.cpp PutDoneRequest.hpp \
    AbortRequestRequest.cpp AbortRequestRequest.hpp \
    AbortFilesRequest.cpp AbortFilesRequest.hpp \
    SuspendRequestRequest.cpp SuspendRequestRequest.hpp \
    ResumeRequestRequest.cpp ResumeRequestRequest.hpp \
    ExtendFileLifeTimeRequest.cpp ExtendFileLifeTimeRequest.hpp \
    GetTransferProtocolsRequest.cpp GetTransferProtocolsRequest.hpp \
    PingRequest.cpp PingRequest.hpp \
    ProtocolChecker.cpp ProtocolChecker.hpp \
    Authorization.cpp Authorization.hpp \
    cgsi_plugin_int.h \
    gssapi_openssl.h globus_gsi_gss_constants.h \
    asynch.cpp asynch_status.cpp \
    Surl.hpp \
    BolSurl.hpp PtgSurl.hpp PtpSurl.hpp CopySurl.hpp \
    Turl.hpp BolTurl.hpp PtgTurl.hpp PtpTurl.hpp CopyTurl.hpp \
    srmv2_GetRequestSummary.cpp srmv2_GetRequestTokens.cpp \
    FrontendConfiguration.cpp FrontendConfiguration.hpp \
    HttpPostClient.cpp HttpPostClient.h \
    DBConnectionPool.cpp DBConnectionPool.hpp \
    ThreadPool.hpp ThreadPool.cpp \
    srmlogit.cpp sql_string.hpp soap_util.hpp soap_util.cpp \
    srmv2_dirreq.cpp srmv2_xferreq.cpp \
    srmv2_permreq.cpp srmv2_spacereq.cpp \
    synch.cpp synch.hpp\
    token_validator.hpp token_validator.cpp \
    xmlrpc_decode.cpp xmlrpc_encode.cpp \
    storm_util.cpp storm_utils.hpp \
    srmv2C.cc \
    srmv2Server.cc \
    Monitor.hpp InstrumentedMonitor.hpp \
    MonitorStub.hpp InstrumentedMonitorBuilder.hpp \
    MonitorHelper.hpp \
    Monitoring.hpp Monitoring.cpp MonitoringHelper.hpp \
    MonitorNotEnabledException.hpp \
    $(noinst_INCLUDES)

AM_CPPFLAGS = \
    -I$(top_srcdir)/src/db \
    -I$(GENERATED_GSOAP_INCLUDES) \
    @BOOST_CPPFLAGS@ \
    @LIBCURL_CFLAGS@ \
    @XMLRPC_CFLAGS@ \
    @UUID_CFLAGS@ \
    @GLOBUS_GRIDMAP_CALLOUT_ERROR_CFLAGS@ \
    @GLOBUS_GSI_CREDENTIAL_CFLAGS@ \
    @GLOBUS_GSS_ASSIST_CFLAGS@ \
    @GLOBUS_COMMON_CFLAGS@ \
    @GSOAP_SSL_PP_CFLAGS@ \
    @MYSQL_CFLAGS@ \
    @LIBARGUS_PEP_CFLAGS@ \
    $(MTC_CFLAGS)

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
