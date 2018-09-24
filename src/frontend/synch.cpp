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

#include "srmv2H.h"
#include <boost/format.hpp>

#include "MonitoringHelper.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "srmlogit.h"
#include "get_socket_info.hpp"
#include "Authorization.hpp"
#include <stdsoap2.h>
#include "soap_util.hpp"
#include "synch.hpp"
#include "storm_exception.hpp"
#include "sync_requests.hpp"
#include "request_logger.hpp"
#include "base_request.hpp"

template<typename response_container_t, typename response_t, typename request_t,
        typename soap_request_t>
int handle_request(const char* func_name, struct soap* soap,
        soap_request_t* soap_req, response_container_t* response_container,
        response_t** soap_resp)
{

    storm::logging_traits<request_t> logger;

    using boost::format;
    using boost::str;

    boost::posix_time::ptime const start_time =
            boost::posix_time::microsec_clock::local_time();

    try {

        request_t request(soap, soap_req);

        logger.log_request(func_name, request);

        bool const request_is_blacklisted =
                storm::authz::is_blacklisted(request.getSoapRequest());

        if (request_is_blacklisted) {

            *soap_resp = storm::build_error_message_response<response_t>(soap,
                    SRM_USCOREAUTHORIZATION_USCOREFAILURE,
                    "Request authorization error: user is blacklisted.");

            storm::request::register_request_error<request_t>(func_name,
                    SRM_USCOREAUTHORIZATION_USCOREFAILURE, start_time,
                    "Request authorization error: user is blacklisted.\n");

            return SOAP_OK;
        }

        // Request is authorized, handle it
        int const soap_return_value = request.performXmlRpcCall(response_container);

        storm::MonitoringHelper::registerOperation(
                request.getStartTime(),
                soap_return_value,
                request_t::MONITOR_NAME,
                request.getStatus()
        );

        storm::log_request_outcome(func_name, request);

        return soap_return_value;

    } catch (storm::invalid_request& e) {

        *soap_resp = storm::build_error_message_response<response_t>(soap,
                SRM_USCOREINVALID_USCOREREQUEST, e.what());

        storm::request::register_request_error<request_t>(func_name,
                SRM_USCOREINVALID_USCOREREQUEST, start_time,
                str(format("%s\n") % e.what()));

        return SOAP_OK;

    } catch (storm::authorization_error& e) {

        *soap_resp = storm::build_error_message_response<response_t>(soap,
                SRM_USCOREFAILURE, e.what());

        storm::request::register_request_error<request_t>(func_name, SRM_USCOREFAILURE,
                start_time,
                str(format("Error authorizing request: %s\n") % e.what()));

        return SOAP_OK;

    }
}

//Directory Functions

int ns1__srmMkdir(struct soap* soap, struct ns1__srmMkdirRequest *req,
        struct ns1__srmMkdirResponse_ *rep) {

    return handle_request<ns1__srmMkdirResponse_, ns1__srmMkdirResponse,
            storm::MkdirRequest, ns1__srmMkdirRequest>(__func__, soap, req, rep,
            &(rep->srmMkdirResponse));

}

int ns1__srmRmdir(struct soap* soap, struct ns1__srmRmdirRequest *req,
        struct ns1__srmRmdirResponse_ *rep) {

    return handle_request<ns1__srmRmdirResponse_, ns1__srmRmdirResponse,
            storm::RmdirRequest, ns1__srmRmdirRequest>(__func__, soap, req, rep,
            &(rep->srmRmdirResponse));
}

int ns1__srmRm(struct soap* soap, struct ns1__srmRmRequest *req,
        struct ns1__srmRmResponse_ *rep) {

    return handle_request<ns1__srmRmResponse_, ns1__srmRmResponse,
            storm::RmRequest, ns1__srmRmRequest>(__func__, soap, req, rep,
            &(rep->srmRmResponse));
}

int ns1__srmLs(struct soap* soap, struct ns1__srmLsRequest *req,
        struct ns1__srmLsResponse_ *rep) {

    return handle_request<ns1__srmLsResponse_, ns1__srmLsResponse,
            storm::LsRequest, ns1__srmLsRequest>(__func__, soap, req, rep,
            &(rep->srmLsResponse));
}

int ns1__srmStatusOfLsRequest(struct soap* soap,
        struct ns1__srmStatusOfLsRequestRequest *req,
        struct ns1__srmStatusOfLsRequestResponse_ *rep) {

    return handle_request<ns1__srmStatusOfLsRequestResponse_,
            ns1__srmStatusOfLsRequestResponse, storm::StatusLsRequest,
            ns1__srmStatusOfLsRequestRequest>(__func__, soap, req, rep,
            &(rep->srmStatusOfLsRequestResponse));
}

int ns1__srmMv(struct soap* soap, struct ns1__srmMvRequest *req,
        struct ns1__srmMvResponse_ *rep) {

    return handle_request<ns1__srmMvResponse_, ns1__srmMvResponse,
            storm::MvRequest, ns1__srmMvRequest>(__func__, soap, req, rep,
            &(rep->srmMvResponse));
}

//Permission Functions

int ns1__srmSetPermission(struct soap* soap,
        struct ns1__srmSetPermissionRequest *req,
        struct ns1__srmSetPermissionResponse_ *rep) {

    return handle_request<ns1__srmSetPermissionResponse_,
            ns1__srmSetPermissionResponse, storm::SetPermissionRequest,
            ns1__srmSetPermissionRequest>(__func__, soap, req, rep,
            &(rep->srmSetPermissionResponse));
}

int ns1__srmCheckPermission(struct soap* soap,
        struct ns1__srmCheckPermissionRequest *req,
        struct ns1__srmCheckPermissionResponse_ *rep) {

    return handle_request<ns1__srmCheckPermissionResponse_,
            ns1__srmCheckPermissionResponse, storm::CheckPermissionRequest,
            ns1__srmCheckPermissionRequest>(__func__, soap, req, rep,
            &(rep->srmCheckPermissionResponse));
}

int ns1__srmGetPermission(struct soap* soap,
        struct ns1__srmGetPermissionRequest *req,
        struct ns1__srmGetPermissionResponse_ *rep) {

    return handle_request<ns1__srmGetPermissionResponse_,
            ns1__srmGetPermissionResponse, storm::GetPermissionRequest,
            ns1__srmGetPermissionRequest>(__func__, soap, req, rep,
            &(rep->srmGetPermissionResponse));
}

//Space Management Functions

int ns1__srmReserveSpace(struct soap* soap,
        struct ns1__srmReserveSpaceRequest *req,
        struct ns1__srmReserveSpaceResponse_ *rep) {

    return handle_request<ns1__srmReserveSpaceResponse_,
            ns1__srmReserveSpaceResponse, storm::ReserveSpaceRequest,
            ns1__srmReserveSpaceRequest>(__func__, soap, req, rep,
            &(rep->srmReserveSpaceResponse));
}

int ns1__srmStatusOfReserveSpaceRequest(struct soap* soap,
        struct ns1__srmStatusOfReserveSpaceRequestRequest *req,
        struct ns1__srmStatusOfReserveSpaceRequestResponse_ *rep) {

    return handle_request<ns1__srmStatusOfReserveSpaceRequestResponse_,
            ns1__srmStatusOfReserveSpaceRequestResponse,
            storm::StatusReserveSpaceRequest,
            ns1__srmStatusOfReserveSpaceRequestRequest>(__func__, soap, req,
            rep, &(rep->srmStatusOfReserveSpaceRequestResponse));

}

int ns1__srmReleaseSpace(struct soap* soap,
        struct ns1__srmReleaseSpaceRequest *req,
        struct ns1__srmReleaseSpaceResponse_ *rep) {

    return handle_request<ns1__srmReleaseSpaceResponse_,
            ns1__srmReleaseSpaceResponse, storm::ReleaseSpaceRequest,
            ns1__srmReleaseSpaceRequest>(__func__, soap, req, rep,
            &(rep->srmReleaseSpaceResponse));
}

int ns1__srmUpdateSpace(struct soap* soap,
        struct ns1__srmUpdateSpaceRequest *req,
        struct ns1__srmUpdateSpaceResponse_ *rep) {

    return handle_request<ns1__srmUpdateSpaceResponse_,
            ns1__srmUpdateSpaceResponse, storm::UpdateSpaceRequest,
            ns1__srmUpdateSpaceRequest>(__func__, soap, req, rep,
            &(rep->srmUpdateSpaceResponse));
}

int ns1__srmStatusOfUpdateSpaceRequest(struct soap* soap,
        struct ns1__srmStatusOfUpdateSpaceRequestRequest *req,
        struct ns1__srmStatusOfUpdateSpaceRequestResponse_ *rep) {

    return handle_request<ns1__srmStatusOfUpdateSpaceRequestResponse_,
            ns1__srmStatusOfUpdateSpaceRequestResponse,
            storm::StatusUpdateSpaceRequest,
            ns1__srmStatusOfUpdateSpaceRequestRequest>(__func__, soap, req, rep,
            &(rep->srmStatusOfUpdateSpaceRequestResponse));
}

int ns1__srmGetSpaceMetaData(struct soap* soap,
        struct ns1__srmGetSpaceMetaDataRequest *req,
        struct ns1__srmGetSpaceMetaDataResponse_ *rep) {

    return handle_request<ns1__srmGetSpaceMetaDataResponse_,
            ns1__srmGetSpaceMetaDataResponse, storm::GetSpaceMetaDataRequest,
            ns1__srmGetSpaceMetaDataRequest>(__func__, soap, req, rep,
            &(rep->srmGetSpaceMetaDataResponse));
}

int ns1__srmGetSpaceTokens(struct soap* soap,
        struct ns1__srmGetSpaceTokensRequest *req,
        struct ns1__srmGetSpaceTokensResponse_ *rep) {

    return handle_request<ns1__srmGetSpaceTokensResponse_,
            ns1__srmGetSpaceTokensResponse, storm::GetSpaceTokensRequest,
            ns1__srmGetSpaceTokensRequest>(__func__, soap, req, rep,
            &(rep->srmGetSpaceTokensResponse));
}

int ns1__srmChangeSpaceForFiles(struct soap* soap,
        struct ns1__srmChangeSpaceForFilesRequest *req,
        struct ns1__srmChangeSpaceForFilesResponse_ *rep) {

    return handle_request<ns1__srmChangeSpaceForFilesResponse_,
            ns1__srmChangeSpaceForFilesResponse,
            storm::ChangeSpaceForFilesRequest,
            ns1__srmChangeSpaceForFilesRequest>(__func__, soap, req, rep,
            &(rep->srmChangeSpaceForFilesResponse));
}

int ns1__srmStatusOfChangeSpaceForFilesRequest(struct soap* soap,
        struct ns1__srmStatusOfChangeSpaceForFilesRequestRequest *req,
        struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse_ *rep) {

    return handle_request<ns1__srmStatusOfChangeSpaceForFilesRequestResponse_,
            ns1__srmStatusOfChangeSpaceForFilesRequestResponse,
            storm::StatusChangeSpaceForFilesRequest,
            ns1__srmStatusOfChangeSpaceForFilesRequestRequest>(__func__, soap,
            req, rep, &(rep->srmStatusOfChangeSpaceForFilesRequestResponse));
}

int ns1__srmExtendFileLifeTimeInSpace(struct soap* soap,
        struct ns1__srmExtendFileLifeTimeInSpaceRequest *req,
        struct ns1__srmExtendFileLifeTimeInSpaceResponse_ *rep) {

    return handle_request<ns1__srmExtendFileLifeTimeInSpaceResponse_,
            ns1__srmExtendFileLifeTimeInSpaceResponse,
            storm::ExtendFileLifeTimeInSpaceRequest,
            ns1__srmExtendFileLifeTimeInSpaceRequest>(__func__, soap, req, rep,
            &(rep->srmExtendFileLifeTimeInSpaceResponse));
}

int ns1__srmPurgeFromSpace(struct soap* soap,
        struct ns1__srmPurgeFromSpaceRequest *req,
        struct ns1__srmPurgeFromSpaceResponse_ *rep) {

    return handle_request<ns1__srmPurgeFromSpaceResponse_,
            ns1__srmPurgeFromSpaceResponse, storm::PurgeFromSpaceRequest,
            ns1__srmPurgeFromSpaceRequest>(__func__, soap, req, rep,
            &(rep->srmPurgeFromSpaceResponse));
}

//Data Transfer Functions

int ns1__srmReleaseFiles(struct soap* soap,
        struct ns1__srmReleaseFilesRequest *req,
        struct ns1__srmReleaseFilesResponse_ *rep) {

    return handle_request<ns1__srmReleaseFilesResponse_,
            ns1__srmReleaseFilesResponse, storm::ReleaseFilesRequest,
            ns1__srmReleaseFilesRequest>(__func__, soap, req, rep,
            &(rep->srmReleaseFilesResponse));
}

int ns1__srmPutDone(struct soap* soap, struct ns1__srmPutDoneRequest *req,
        struct ns1__srmPutDoneResponse_ *rep) {

    return handle_request<ns1__srmPutDoneResponse_, ns1__srmPutDoneResponse,
            storm::PutDoneRequest, ns1__srmPutDoneRequest>(__func__, soap, req,
            rep, &(rep->srmPutDoneResponse));
}

int ns1__srmAbortRequest(struct soap* soap,
        struct ns1__srmAbortRequestRequest *req,
        struct ns1__srmAbortRequestResponse_ *rep) {

    return handle_request<ns1__srmAbortRequestResponse_,
            ns1__srmAbortRequestResponse, storm::AbortRequestRequest,
            ns1__srmAbortRequestRequest>(__func__, soap, req, rep,
            &(rep->srmAbortRequestResponse));
}

int ns1__srmAbortFiles(struct soap* soap, struct ns1__srmAbortFilesRequest *req,
        struct ns1__srmAbortFilesResponse_ *rep) {

    return handle_request<ns1__srmAbortFilesResponse_,
            ns1__srmAbortFilesResponse, storm::AbortFilesRequest,
            ns1__srmAbortFilesRequest>(__func__, soap, req, rep,
            &(rep->srmAbortFilesResponse));
}

int ns1__srmSuspendRequest(struct soap* soap,
        struct ns1__srmSuspendRequestRequest *req,
        struct ns1__srmSuspendRequestResponse_ *rep) {

    return handle_request<ns1__srmSuspendRequestResponse_,
            ns1__srmSuspendRequestResponse, storm::SuspendRequestRequest,
            ns1__srmSuspendRequestRequest>(__func__, soap, req, rep,
            &(rep->srmSuspendRequestResponse));
}

int ns1__srmResumeRequest(struct soap* soap,
        struct ns1__srmResumeRequestRequest *req,
        struct ns1__srmResumeRequestResponse_ *rep) {

    return handle_request<ns1__srmResumeRequestResponse_,
            ns1__srmResumeRequestResponse, storm::ResumeRequestRequest,
            ns1__srmResumeRequestRequest>(__func__, soap, req, rep,
            &(rep->srmResumeRequestResponse));
}

int ns1__srmExtendFileLifeTime(struct soap* soap,
        struct ns1__srmExtendFileLifeTimeRequest *req,
        struct ns1__srmExtendFileLifeTimeResponse_ *rep) {

    return handle_request<ns1__srmExtendFileLifeTimeResponse_,
            ns1__srmExtendFileLifeTimeResponse,
            storm::ExtendFileLifeTimeRequest, ns1__srmExtendFileLifeTimeRequest>(
            __func__, soap, req, rep, &(rep->srmExtendFileLifeTimeResponse));
}

int ns1__srmGetTransferProtocols(struct soap* soap,
        struct ns1__srmGetTransferProtocolsRequest *req,
        struct ns1__srmGetTransferProtocolsResponse_ *rep) {

    return handle_request<ns1__srmGetTransferProtocolsResponse_,
            ns1__srmGetTransferProtocolsResponse,
            storm::GetTransferProtocolsRequest,
            ns1__srmGetTransferProtocolsRequest>(__func__, soap, req, rep,
            &(rep->srmGetTransferProtocolsResponse));
}

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req,
        struct ns1__srmPingResponse_ *rep) {

    return handle_request<ns1__srmPingResponse_, ns1__srmPingResponse,
            storm::PingRequest, ns1__srmPingRequest>(__func__, soap, req, rep,
            &(rep->srmPingResponse));
}
