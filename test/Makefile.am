# $Id$
#*********************************************************************
#*
#* Authors: Flavia Donno <Flavia.Donno@cern.ch>
#*	    Magnoni Luca <luca.magnoni@cnaf.infn.it>	
#* Copyright 2004 INFN FIRB Project
#*           http://grid-it.cnaf.infn.it/
#*
#*
#* Date: 01 March 2005
#*
#*********************************************************************

MTCCFLAGS = -DCTHREAD_LINUX -D_THREAD_SAFE -D_REENTRANT

MAINTAINERCLEANFILES = Makefile.in $(SOAPFILES)

SOAPFILES = srmv2C.c \
            srmv2Client.c

INCLUDES = -I. -I$(top_srcdir)/srmv2 \
           @MYSQL_CFLAGS@ \
           @GSOAPGSI_CFLAGS@ \
           -I@GSOAP_DIR@ \
           -I@GSOAPGSI_DIR@/include \
           -DSTORMCONFIG=\"$(STORMCONFIG)\" \
           -DLOGFILE=\"$(LOGFILE)\" \
           $(MTCCFLAGS)

bin_PROGRAMS =  srmv2-Mkdir \
                srmv2-Rmdir \
                srmv2-Rm \
                srmv2-Ls \
                srmv2-ReserveSpace \
                srmv2-GetSpaceMetaData \
                srmv2-PutDone \
                srmv2-ReleaseFiles \
                srmv2-PutRequestStatus \
                client_Ping \
                client_PtG \
                client_Bol \
                client_PtP \
                client_status_PtG \
                client_status_Bol \
                client_Copy \
				client_status_Copy \
                client_GetTransferProtocols

#               srmv2-PtG srmv2-put srmv2-PtP \
#               srmv2-GetRequestStatus \
#               srmv2-CopyRequestStatus \
#               srmv2-GetRequestID \
#               srmv2-GetRequestSummary \
#               srmv2-Copy \
#               srmv2-GetSpaceToken \
#	            srmv2-GetSpaceMetaData \
#	            test-get-protocols
#               srmv2-get
#               client_Bol \
#               client_PtP \
#				client_Ls \
#				client_status_PtP \

srmv2C.c : $(top_srcdir)/srmv2/srmv2C.c
	cp $< $@

srmv2Client.c : $(top_srcdir)/srmv2/srmv2Client.c
	cp $< $@

client_Bol_SOURCES = client_Bol.c $(SOAPFILES)
client_PtP_SOURCES = client_PtP.c $(SOAPFILES)
client_PtG_SOURCES = client_PtG.c $(SOAPFILES)
client_Copy_SOURCES = client_Copy.c $(SOAPFILES)
client_Ls_SOURCES = client_Ls.c $(SOAPFILES)
client_status_Copy_SOURCES = client_status_Copy.cpp $(SOAPFILES)
client_status_Bol_SOURCES = client_status_Bol.c $(SOAPFILES)
client_status_PtG_SOURCES = client_status-PtG.c $(SOAPFILES)
client_status_PtP_SOURCES = client_status-PtP.c $(SOAPFILES)
client_Ping_SOURCES = client_Ping.c $(SOAPFILES)
client_GetTransferProtocols_SOURCES = client_GetTransferProtocols.c $(SOAPFILES)
srmv2_PtG_SOURCES = srm2_testGet.c $(SOAPFILES)
srmv2_PtP_SOURCES = srm2_testPut.c $(SOAPFILES)
#srmv2_get_SOURCES = srm2_testget.c $(SOAPFILES)
srmv2_put_SOURCES = srm2_testput.c $(SOAPFILES)
srmv2_GetRequestID_SOURCES = srm2_testGetRequestID.c $(SOAPFILES)
srmv2_GetRequestStatus_SOURCES = srm2_testGetRequestStatus.c $(SOAPFILES)
srmv2_PutRequestStatus_SOURCES = srm2_testPutRequestStatus.c $(SOAPFILES)
srmv2_CopyRequestStatus_SOURCES = srm2_testCopyRequestStatus.c $(SOAPFILES)
srmv2_GetRequestSummary_SOURCES = srm2_testGetRequestSummary.c $(SOAPFILES)
srmv2_AbortFiles_SOURCES = srm2_testAbortFiles.c $(SOAPFILES) 
srmv2_AbortRequest_SOURCES = srm2_testAbortRequest.c $(SOAPFILES)
srmv2_ChangeFileStorageType_SOURCES = srm2_testChangeFileStorageType.c $(SOAPFILES)
srmv2_CheckPermission_SOURCES = srm2_testCheckPermission.c $(SOAPFILES)
srmv2_CompactSpace_SOURCES = srm2_testCompactSpace.c $(SOAPFILES)
srmv2_Copy_SOURCES = srm2_testCopy.c $(SOAPFILES)
srmv2_ExtendFileLifeTime_SOURCES = srm2_testExtendFileLifeTime.c $(SOAPFILES)
srmv2_GetSpaceMetaData_SOURCES = srm2_testGetSpaceMetaData.c $(SOAPFILES)
srmv2_GetSpaceToken_SOURCES = srm2_testGetSpaceToken.c $(SOAPFILES)
srmv2_Ls_SOURCES = srm2_testLs.c $(SOAPFILES)
srmv2_Mkdir_SOURCES = srm2_testMkdir.c $(SOAPFILES)
srmv2_Mv_SOURCES = srm2_testMv.c $(SOAPFILES)
srmv2_PutDone_SOURCES = srm2_testPutDone.c $(SOAPFILES)
srmv2_ReassignToUser_SOURCES = srm2_testReassignToUser.c $(SOAPFILES)
srmv2_ReleaseFiles_SOURCES = srm2_testReleaseFiles.c $(SOAPFILES)
srmv2_ReleaseSpace_SOURCES = srm2_testReleaseSpace.c $(SOAPFILES)
srmv2_RemoveFiles_SOURCES = srm2_testRemoveFiles.c $(SOAPFILES)
srmv2_ReserveSpace_SOURCES = srm2_testReserveSpace.c $(SOAPFILES)
srmv2_GetSpaceMetaData_SOURCES = srm2_testGetSpaceMetaData.c $(SOAPFILES)
srmv2_ResumeRequest_SOURCES = srm2_testResumeRequest.c $(SOAPFILES)
srmv2_Rm_SOURCES = srm2_testRm.c $(SOAPFILES)
srmv2_Rmdir_SOURCES = srm2_testRmdir.c $(SOAPFILES)
srmv2_SetPermission_SOURCES = srm2_testSetPermission.c $(SOAPFILES)
srmv2_SuspendRequest_SOURCES = srm2_testSuspendRequest.c $(SOAPFILES)
srmv2_UpdateSpace_SOURCES = srm2_testUpdateSpace.c $(SOAPFILES)
test_get_protocols_SOURCES = test_get_protocols.c $(SOAPFILES)


srmv2_PtG_LDFLAGS = -L$(top_srcdir)/srmv2 -L$(top_srcdir)/db @GLOBUS_LDFLAGS@ \
                     @MYSQL_LDFLAGS@ @GSOAPGSI_LDFLAGS@ @SSL_AND_CRYPTO@
srmv2_PtG_LDADD = -lsrmv2storm -lstormdb @MYSQL_LDADD@ @GLOBUS_PKG_LIBS@ \
                   @GSOAPGSI_LDADD@ -lshift -lz -lm -luuid
srmv2_PtG_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la


test_get_protocols_LDFLAGS = $(srmv2_PtG_LDFLAGS)
test_get_protocols_LDADD = -lsrmv2storm -lstormdb @MYSQL_LDADD@ \
	                   @GSOAPGSI_LDADD@ @GLOBUS_PKG_LIBS@  -lshift                 
test_get_protocols_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
#srmv2_get_LDFLAGS = $(srmv2_PtG_LDFLAGS)
#srmv2_get_LDADD = $(srmv2_PtG_LDADD)
#srmv2_get_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
#                         $(top_srcdir)/db/libstormdb.la
srmv2_put_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_put_LDADD = $(srmv2_PtG_LDADD)
srmv2_put_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_PtP_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_PtP_LDADD = $(srmv2_PtG_LDADD)
srmv2_PtP_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la

#client_Bol_CFLAGS += -DSTRICT_CHECKING 
#client_PtP_CFLAGS += -DSTRICT_CHECKING 
client_Bol_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_PtP_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_Bol_LDADD = $(srmv2_PtG_LDADD)
client_PtP_LDADD = $(srmv2_PtG_LDADD)
client_Bol_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_PtP_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
#client_PtG_CFLAGS += -DSTRICT_CHECKING 
client_Copy_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_Copy_LDADD = $(srmv2_PtG_LDADD)
client_Copy_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_PtG_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_PtG_LDADD = $(srmv2_PtG_LDADD)
client_PtG_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_Ls_CFLAGS += -DSTRICT_CHECKING 
client_Ls_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_Ls_LDADD = $(srmv2_PtG_LDADD)
client_Ls_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_status_Copy_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_status_Copy_LDADD = $(srmv2_PtG_LDADD)
client_status_Copy_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_status_PtG_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_status_PtG_LDADD = $(srmv2_PtG_LDADD)
client_status_PtG_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_status_PtP_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_status_PtP_LDADD = $(srmv2_PtG_LDADD)
client_status_PtP_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_status_Bol_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_status_Bol_LDADD = $(srmv2_PtG_LDADD)
client_status_Bol_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_Ping_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_Ping_LDADD = $(srmv2_PtG_LDADD)
client_Ping_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
client_GetTransferProtocols_LDFLAGS = $(srmv2_PtG_LDFLAGS)
client_GetTransferProtocols_LDADD = $(srmv2_PtG_LDADD)
client_GetTransferProtocols_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_GetRequestStatus_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_GetRequestStatus_LDADD = $(srmv2_PtG_LDADD)
srmv2_Get_RequestStatus_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_PutRequestStatus_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_PutRequestStatus_LDADD = $(srmv2_PtG_LDADD)
srmv2_PutRequestStatus_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_CopyRequestStatus_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_CopyRequestStatus_LDADD = $(srmv2_PtG_LDADD)
srmv2_CopyRequestStatus_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_GetRequestID_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_GetRequestID_LDADD = $(srmv2_PtG_LDADD)
srmv2_GetRequestID_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_GetRequestSummary_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_GetRequestSummary_LDADD = $(srmv2_PtG_LDADD)
srmv2_GetRequestSummary_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_AbortFiles_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_AbortFiles_LDADD = $(srmv2_PtG_LDADD)
srmv2_AbortFiles_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_AbortRequest_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_AbortRequest_LDADD = $(srmv2_PtG_LDADD)
srmv2_AbortRequest_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ChangeFileStorageType_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ChangeFileStorageType_LDADD = $(srmv2_PtG_LDADD)
srmv2_ChangeFileStorageType_DEPENDENCIES = \
                         $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_CheckPermission_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_CheckPermission_LDADD = $(srmv2_PtG_LDADD)
srmv2_CheckPermission_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_CompactSpace_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_CompactSpace_LDADD = $(srmv2_PtG_LDADD)
srmv2_CompactSpace_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_Copy_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_Copy_LDADD = $(srmv2_PtG_LDADD)
srmv2_Copy_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ExtendFileLifeTime_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ExtendFileLifeTime_LDADD = $(srmv2_PtG_LDADD)
srmv2_ExtendFileLifeTime_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_GetSpaceMetaData_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_GetSpaceMetaData_LDADD = $(srmv2_PtG_LDADD)
srmv2_GetSpaceMetaData_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_GetSpaceToken_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_GetSpaceToken_LDADD = $(srmv2_PtG_LDADD)
srmv2_GetSpaceToken_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_Ls_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_Ls_LDADD = $(srmv2_PtG_LDADD)
srmv2_Ls_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_Mkdir_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_Mkdir_LDADD = $(srmv2_PtG_LDADD)
srmv2_Mkdir_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_Mv_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_Mv_LDADD = $(srmv2_PtG_LDADD)
srmv2_Mv_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_PutDone_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_PutDone_LDADD = $(srmv2_PtG_LDADD)
srmv2_PutDone_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ReassignToUser_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ReassignToUser_LDADD = $(srmv2_PtG_LDADD)
srmv2_ReassignToUser_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ReleaseFiles_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ReleaseFiles_LDADD = $(srmv2_PtG_LDADD)
srmv2_ReleaseFiles_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ReleaseSpace_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ReleaseSpace_LDADD = $(srmv2_PtG_LDADD)
srmv2_ReleaseSpace_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_RemoveFiles_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_RemoveFiles_LDADD = $(srmv2_PtG_LDADD)
srmv2_RemoveFiles_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ReserveSpace_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ReserveSpace_LDADD = $(srmv2_PtG_LDADD)
srmv2_ReserveSpace_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_ResumeRequest_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_ResumeRequest_LDADD = $(srmv2_PtG_LDADD)
srmv2_ResumeRequest_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_Rm_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_Rm_LDADD = $(srmv2_PtG_LDADD)
srmv2_Rm_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_Rmdir_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_Rmdir_LDADD = $(srmv2_PtG_LDADD)
srmv2_Rmdir_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_SetPermission_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_SetPermission_LDADD = $(srmv2_PtG_LDADD)
srmv2_SetPermission_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_SuspendRequest_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_SuspendRequest_LDADD = $(srmv2_PtG_LDADD)
srmv2_SuspendRequest_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la
srmv2_UpdateSpace_LDFLAGS = $(srmv2_PtG_LDFLAGS)
srmv2_UpdateSpace_LDADD = $(srmv2_PtG_LDADD)
srmv2_UpdateSpace_DEPENDENCIES = $(top_srcdir)/srmv2/libsrmv2storm.la \
                         $(top_srcdir)/db/libstormdb.la


clean :
	/bin/rm -f $(SOAPFILES)


dist-hook:
	cd $(distdir)
	for file in $(MAINTAINERCLEANFILES); do \
           /bin/rm $(distdir)/$$file; \
        done

