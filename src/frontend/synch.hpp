/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef SYNCH_HPP
#define SYNCH_HPP

//Directory Functions

extern "C" int ns1__srmMkdir_impl(struct soap *soap, struct ns1__srmMkdirRequest *req,
        struct ns1__srmMkdirResponse_ *rep);

extern "C" int ns1__srmRmdir_impl(struct soap *soap, struct ns1__srmRmdirRequest *req,
        struct ns1__srmRmdirResponse_ *rep);

extern "C" int ns1__srmRm_impl(struct soap *soap, struct ns1__srmRmRequest *req,
        struct ns1__srmRmResponse_ *rep);

extern "C" int ns1__srmLs_impl(struct soap *soap, struct ns1__srmLsRequest *req,
        struct ns1__srmLsResponse_ *rep);

extern "C" int ns1__srmStatusOfLsRequest_impl(struct soap *soap,
        struct ns1__srmStatusOfLsRequestRequest *req,
        struct ns1__srmStatusOfLsRequestResponse_ *rep);

extern "C" int ns1__srmMv_impl(struct soap *soap, struct ns1__srmMvRequest *req,
		struct ns1__srmMvResponse_ *rep);

//Permission Functions

extern "C" int ns1__srmSetPermission_impl(struct soap *soap,
        struct ns1__srmSetPermissionRequest *req,
        struct ns1__srmSetPermissionResponse_ *rep);

extern "C" int ns1__srmCheckPermission_impl(struct soap *soap,
        struct ns1__srmCheckPermissionRequest *req,
        struct ns1__srmCheckPermissionResponse_ *rep);

extern "C" int ns1__srmGetPermission_impl(struct soap *soap,
        struct ns1__srmGetPermissionRequest *req,
        struct ns1__srmGetPermissionResponse_ *rep);

//Space Management Functions

extern "C" int ns1__srmReserveSpace_impl(struct soap *soap,
                         struct ns1__srmReserveSpaceRequest *req,
                         struct ns1__srmReserveSpaceResponse_ *rep);

extern "C" int ns1__srmStatusOfReserveSpaceRequest_impl(struct soap *soap,
        struct ns1__srmStatusOfReserveSpaceRequestRequest *req,
        struct ns1__srmStatusOfReserveSpaceRequestResponse_ *rep);

extern "C" int ns1__srmReleaseSpace_impl(struct soap *soap,
        struct ns1__srmReleaseSpaceRequest *req,
        struct ns1__srmReleaseSpaceResponse_ *rep);

extern "C" int ns1__srmUpdateSpace_impl(struct soap *soap,
        struct ns1__srmUpdateSpaceRequest *req,
        struct ns1__srmUpdateSpaceResponse_ *rep);

extern "C" int ns1__srmStatusOfUpdateSpaceRequest_impl(struct soap *soap,
        struct ns1__srmStatusOfUpdateSpaceRequestRequest *req,
        struct ns1__srmStatusOfUpdateSpaceRequestResponse_ *rep);

extern "C" int ns1__srmGetSpaceMetaData_impl(struct soap *soap,
        struct ns1__srmGetSpaceMetaDataRequest *req,
        struct ns1__srmGetSpaceMetaDataResponse_ *rep);

extern "C" int ns1__srmGetSpaceTokens_impl(struct soap *soap,
        struct ns1__srmGetSpaceTokensRequest *req,
        struct ns1__srmGetSpaceTokensResponse_ *rep);

extern "C" int ns1__srmChangeSpaceForFiles_impl(struct soap *soap,
        struct ns1__srmChangeSpaceForFilesRequest *req,
        struct ns1__srmChangeSpaceForFilesResponse_ *rep);

extern "C" int ns1__srmStatusOfChangeSpaceForFilesRequest_impl(struct soap *soap,
        struct ns1__srmStatusOfChangeSpaceForFilesRequestRequest *req,
        struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse_ *rep);

extern "C" int ns1__srmExtendFileLifeTimeInSpace_impl(struct soap *soap,
        struct ns1__srmExtendFileLifeTimeInSpaceRequest *req,
        struct ns1__srmExtendFileLifeTimeInSpaceResponse_ *rep);

extern "C" int ns1__srmPurgeFromSpace_impl(struct soap *soap,
        struct ns1__srmPurgeFromSpaceRequest *req,
        struct ns1__srmPurgeFromSpaceResponse_ *rep);

//Data Transfer Functions

extern "C" int ns1__srmReleaseFiles_impl (struct soap *soap,
		struct ns1__srmReleaseFilesRequest *req, struct ns1__srmReleaseFilesResponse_ *rep);

extern "C" int ns1__srmPutDone_impl (struct soap *soap,
		struct ns1__srmPutDoneRequest *req, struct ns1__srmPutDoneResponse_ *rep);

extern "C" int ns1__srmAbortRequest_impl (struct soap *soap,
		struct ns1__srmAbortRequestRequest *req, struct ns1__srmAbortRequestResponse_ *rep);

extern "C" int ns1__srmAbortFiles_impl (struct soap *soap,
		struct ns1__srmAbortFilesRequest *req, struct ns1__srmAbortFilesResponse_ *rep);

extern "C" int ns1__srmSuspendRequest_impl (struct soap *soap,
		struct ns1__srmSuspendRequestRequest *req, struct ns1__srmSuspendRequestResponse_ *rep);

extern "C" int ns1__srmResumeRequest_impl (struct soap *soap,
		struct ns1__srmResumeRequestRequest *req, struct ns1__srmResumeRequestResponse_ *rep);

extern "C" int ns1__srmExtendFileLifeTime_impl (struct soap *soap,
		struct ns1__srmExtendFileLifeTimeRequest *req, struct ns1__srmExtendFileLifeTimeResponse_ *rep);

extern "C" int ns1__srmGetTransferProtocols_impl (struct soap *soap,
		struct ns1__srmGetTransferProtocolsRequest *req, struct ns1__srmGetTransferProtocolsResponse_ *rep);

extern "C" int ns1__srmPing_impl(struct soap* soap,
		struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep);

#endif // SYNCH_HPP
