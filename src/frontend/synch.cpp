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

#include "boost/date_time/posix_time/posix_time.hpp"

#include "srmlogit.h"
#include "get_socket_info.hpp"
#include "Authorization.hpp"
#include <stdsoap2.h>
#include "soap_util.hpp"
#include "synch.hpp"
#include "storm_exception.hpp"
#include "request/synch/sync_requests.hpp"
#include "request_logger.hpp"

typedef ns1__TStatusCode srm_request_status;

template<typename request_t>
void register_request_error(const char* func_name, srm_request_status status,
		boost::posix_time::ptime start_time, std::string const& msg) {
	srmlogit(STORM_LOG_ERROR, func_name, msg.c_str());
	srmLogResponse(request_t::NAME.c_str(), status);
	storm::MonitoringHelper::registerOperationError(start_time,
			request_t::MONITOR_NAME.c_str());
}

template<typename response_container_t, typename response_t, typename request_t,
		typename soap_request_t>
int handle_request(const char* func_name, struct soap* soap,
		soap_request_t* soap_req, response_container_t* response_container,
		response_t** soap_resp) {

	storm::logging_traits<request_t> logger;

	using boost::format;
	using boost::str;

	boost::posix_time::ptime const start_time =
			boost::posix_time::microsec_clock::local_time();

	try {

		request_t request(soap, soap_req);

		logger.log_request(func_name, request);

		bool const request_is_blacklisted =
				storm::Authorization::checkBlacklist(request.getSoapRequest());

		if (request_is_blacklisted) {

			*soap_resp = storm::build_error_message_response<response_t>(soap,
					SRM_USCOREAUTHORIZATION_USCOREFAILURE,
					"Request authorization error: user is blacklisted.");

			register_request_error<request_t>(func_name,
					SRM_USCOREAUTHORIZATION_USCOREFAILURE, start_time,
					"Request authorization error: user is blacklisted.");

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

		return soap_return_value;

	} catch (storm::invalid_request& e) {

		*soap_resp = storm::build_error_message_response<response_t>(soap,
				SRM_USCOREINVALID_USCOREREQUEST, e.what());

		register_request_error<request_t>(func_name,
				SRM_USCOREINVALID_USCOREREQUEST, start_time,
				str(format("%s\n") % e.what()));

		return SOAP_OK;

	} catch (storm::AuthorizationException& e) {

		*soap_resp = storm::build_error_message_response<response_t>(soap,
				SRM_USCOREFAILURE, e.what());

		register_request_error<request_t>(func_name, SRM_USCOREFAILURE,
				start_time,
				str(format("Error authorizing request: %s\n") % e.what()));

		return SOAP_OK;

	} catch (storm::ArgusException& e) {

		*soap_resp = storm::build_error_message_response<response_t>(soap,
				SRM_USCOREFAILURE, e.what());

		register_request_error<request_t>(func_name, SRM_USCOREFAILURE,
				start_time,
				str(
						format(
								"Error authorizing request (Argus exception): %s\n")
								% e.what()));

		return SOAP_OK;
	}
}

//Directory Functions

int ns1__srmMkdir(struct soap* soap, struct ns1__srmMkdirRequest *req,
		struct ns1__srmMkdirResponse_ *rep) {

	static const char *funcName = "ns1__srmMkdir()";
	return handle_request<ns1__srmMkdirResponse_, ns1__srmMkdirResponse,
			storm::MkdirRequest, ns1__srmMkdirRequest>(funcName, soap, req, rep,
			&(rep->srmMkdirResponse));

}

int ns1__srmRmdir(struct soap* soap, struct ns1__srmRmdirRequest *req,
		struct ns1__srmRmdirResponse_ *rep) {

	static const char *funcName = "ns1__srmRmdir()";
	return handle_request<ns1__srmRmdirResponse_, ns1__srmRmdirResponse,
			storm::RmdirRequest, ns1__srmRmdirRequest>(funcName, soap, req, rep,
			&(rep->srmRmdirResponse));
}

int ns1__srmRm(struct soap* soap, struct ns1__srmRmRequest *req,
		struct ns1__srmRmResponse_ *rep) {

	static const char *funcName = "ns1__srmRm()";
	return handle_request<ns1__srmRmResponse_, ns1__srmRmResponse,
			storm::RmRequest, ns1__srmRmRequest>(funcName, soap, req, rep,
			&(rep->srmRmResponse));
}

int ns1__srmLs(struct soap* soap, struct ns1__srmLsRequest *req,
		struct ns1__srmLsResponse_ *rep) {

	static const char *funcName = "ns1__srmLs()";
	return handle_request<ns1__srmLsResponse_, ns1__srmLsResponse,
			storm::LsRequest, ns1__srmLsRequest>(funcName, soap, req, rep,
			&(rep->srmLsResponse));
}

int ns1__srmStatusOfLsRequest(struct soap* soap,
		struct ns1__srmStatusOfLsRequestRequest *req,
		struct ns1__srmStatusOfLsRequestResponse_ *rep) {

	static char *funcName = "ns1__srmStatusOfLsRequest()";
	return handle_request<ns1__srmStatusOfLsRequestResponse_,
			ns1__srmStatusOfLsRequestResponse, storm::StatusLsRequest,
			ns1__srmStatusOfLsRequestRequest>(funcName, soap, req, rep,
			&(rep->srmStatusOfLsRequestResponse));
}

int ns1__srmMv(struct soap* soap, struct ns1__srmMvRequest *req,
		struct ns1__srmMvResponse_ *rep) {

	static const char *funcName = "ns1__srmMv()";
	return handle_request<ns1__srmMvResponse_, ns1__srmMvResponse,
			storm::MvRequest, ns1__srmMvRequest>(funcName, soap, req, rep,
			&(rep->srmMvResponse));
}

//Permission Functions

int ns1__srmSetPermission(struct soap* soap,
		struct ns1__srmSetPermissionRequest *req,
		struct ns1__srmSetPermissionResponse_ *rep) {

	static const char *funcName = "ns1__srmSetPermission()";
	return handle_request<ns1__srmSetPermissionResponse_,
			ns1__srmSetPermissionResponse, storm::SetPermissionRequest,
			ns1__srmSetPermissionRequest>(funcName, soap, req, rep,
			&(rep->srmSetPermissionResponse));
}

int ns1__srmCheckPermission(struct soap* soap,
		struct ns1__srmCheckPermissionRequest *req,
		struct ns1__srmCheckPermissionResponse_ *rep) {

	static const char *funcName = "ns1__srmCheckPermission()";
	return handle_request<ns1__srmCheckPermissionResponse_,
			ns1__srmCheckPermissionResponse, storm::CheckPermissionRequest,
			ns1__srmCheckPermissionRequest>(funcName, soap, req, rep,
			&(rep->srmCheckPermissionResponse));
}

int ns1__srmGetPermission(struct soap* soap,
		struct ns1__srmGetPermissionRequest *req,
		struct ns1__srmGetPermissionResponse_ *rep) {

	static const char *funcName = "ns1__srmGetPermission()";
	return handle_request<ns1__srmGetPermissionResponse_,
			ns1__srmGetPermissionResponse, storm::GetPermissionRequest,
			ns1__srmGetPermissionRequest>(funcName, soap, req, rep,
			&(rep->srmGetPermissionResponse));
}

//Space Management Functions

int ns1__srmReserveSpace(struct soap* soap,
		struct ns1__srmReserveSpaceRequest *req,
		struct ns1__srmReserveSpaceResponse_ *rep) {

	static const char *funcName = "ns1__srmReserveSpace()";
	return handle_request<ns1__srmReserveSpaceResponse_,
			ns1__srmReserveSpaceResponse, storm::ReserveSpaceRequest,
			ns1__srmReserveSpaceRequest>(funcName, soap, req, rep,
			&(rep->srmReserveSpaceResponse));
}

int ns1__srmStatusOfReserveSpaceRequest(struct soap* soap,
		struct ns1__srmStatusOfReserveSpaceRequestRequest *req,
		struct ns1__srmStatusOfReserveSpaceRequestResponse_ *rep) {

	static const char *funcName = "ns1__srmStatusOfReserveSpaceRequest()";
	return handle_request<ns1__srmStatusOfReserveSpaceRequestResponse_,
			ns1__srmStatusOfReserveSpaceRequestResponse,
			storm::StatusReserveSpaceRequest,
			ns1__srmStatusOfReserveSpaceRequestRequest>(funcName, soap, req,
			rep, &(rep->srmStatusOfReserveSpaceRequestResponse));

}

int ns1__srmReleaseSpace(struct soap* soap,
		struct ns1__srmReleaseSpaceRequest *req,
		struct ns1__srmReleaseSpaceResponse_ *rep) {

	static const char *funcName = "ns1__srmReleaseSpace()";
	return handle_request<ns1__srmReleaseSpaceResponse_,
			ns1__srmReleaseSpaceResponse, storm::ReleaseSpaceRequest,
			ns1__srmReleaseSpaceRequest>(funcName, soap, req, rep,
			&(rep->srmReleaseSpaceResponse));
}

int ns1__srmUpdateSpace(struct soap* soap,
		struct ns1__srmUpdateSpaceRequest *req,
		struct ns1__srmUpdateSpaceResponse_ *rep) {

	static const char *funcName = "ns1__srmUpdateSpace()";
	return handle_request<ns1__srmUpdateSpaceResponse_,
			ns1__srmUpdateSpaceResponse, storm::UpdateSpaceRequest,
			ns1__srmUpdateSpaceRequest>(funcName, soap, req, rep,
			&(rep->srmUpdateSpaceResponse));
}

int ns1__srmStatusOfUpdateSpaceRequest(struct soap* soap,
		struct ns1__srmStatusOfUpdateSpaceRequestRequest *req,
		struct ns1__srmStatusOfUpdateSpaceRequestResponse_ *rep) {

	static const char *funcName = "ns1__srmStatusOfUpdateSpaceRequest()";
	return handle_request<ns1__srmStatusOfUpdateSpaceRequestResponse_,
			ns1__srmStatusOfUpdateSpaceRequestResponse,
			storm::StatusUpdateSpaceRequest,
			ns1__srmStatusOfUpdateSpaceRequestRequest>(funcName, soap, req, rep,
			&(rep->srmStatusOfUpdateSpaceRequestResponse));
}

int ns1__srmGetSpaceMetaData(struct soap* soap,
		struct ns1__srmGetSpaceMetaDataRequest *req,
		struct ns1__srmGetSpaceMetaDataResponse_ *rep) {

	static const char *funcName = "ns1__srmGetSpaceMetaData()";
	return handle_request<ns1__srmGetSpaceMetaDataResponse_,
			ns1__srmGetSpaceMetaDataResponse, storm::GetSpaceMetaDataRequest,
			ns1__srmGetSpaceMetaDataRequest>(funcName, soap, req, rep,
			&(rep->srmGetSpaceMetaDataResponse));
}

int ns1__srmGetSpaceTokens(struct soap* soap,
		struct ns1__srmGetSpaceTokensRequest *req,
		struct ns1__srmGetSpaceTokensResponse_ *rep) {

	static const char *funcName = "ns1__srmGetSpaceTokens()";
	return handle_request<ns1__srmGetSpaceTokensResponse_,
			ns1__srmGetSpaceTokensResponse, storm::GetSpaceTokensRequest,
			ns1__srmGetSpaceTokensRequest>(funcName, soap, req, rep,
			&(rep->srmGetSpaceTokensResponse));
}

int ns1__srmChangeSpaceForFiles(struct soap* soap,
		struct ns1__srmChangeSpaceForFilesRequest *req,
		struct ns1__srmChangeSpaceForFilesResponse_ *rep) {

	static const char *funcName = "ns1__srmChangeSpaceForFiles()";
	return handle_request<ns1__srmChangeSpaceForFilesResponse_,
			ns1__srmChangeSpaceForFilesResponse,
			storm::ChangeSpaceForFilesRequest,
			ns1__srmChangeSpaceForFilesRequest>(funcName, soap, req, rep,
			&(rep->srmChangeSpaceForFilesResponse));
}

int ns1__srmStatusOfChangeSpaceForFilesRequest(struct soap* soap,
		struct ns1__srmStatusOfChangeSpaceForFilesRequestRequest *req,
		struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse_ *rep) {

	static const char *funcName = "ns1__srmStatusOfChangeSpaceForFilesRequest()";
	return handle_request<ns1__srmStatusOfChangeSpaceForFilesRequestResponse_,
			ns1__srmStatusOfChangeSpaceForFilesRequestResponse,
			storm::StatusChangeSpaceForFilesRequest,
			ns1__srmStatusOfChangeSpaceForFilesRequestRequest>(funcName, soap,
			req, rep, &(rep->srmStatusOfChangeSpaceForFilesRequestResponse));
}

int ns1__srmExtendFileLifeTimeInSpace(struct soap* soap,
		struct ns1__srmExtendFileLifeTimeInSpaceRequest *req,
		struct ns1__srmExtendFileLifeTimeInSpaceResponse_ *rep) {

	static const char *funcName = "ns1__srmExtendFileLifeTimeInSpace()";
	return handle_request<ns1__srmExtendFileLifeTimeInSpaceResponse_,
			ns1__srmExtendFileLifeTimeInSpaceResponse,
			storm::ExtendFileLifeTimeInSpaceRequest,
			ns1__srmExtendFileLifeTimeInSpaceRequest>(funcName, soap, req, rep,
			&(rep->srmExtendFileLifeTimeInSpaceResponse));
}

int ns1__srmPurgeFromSpace(struct soap* soap,
		struct ns1__srmPurgeFromSpaceRequest *req,
		struct ns1__srmPurgeFromSpaceResponse_ *rep) {

	static const char *funcName = "ns1__srmPurgeFromSpace()";
	return handle_request<ns1__srmPurgeFromSpaceResponse_,
			ns1__srmPurgeFromSpaceResponse, storm::PurgeFromSpaceRequest,
			ns1__srmPurgeFromSpaceRequest>(funcName, soap, req, rep,
			&(rep->srmPurgeFromSpaceResponse));
}

//Data Transfer Functions

int ns1__srmReleaseFiles(struct soap* soap,
		struct ns1__srmReleaseFilesRequest *req,
		struct ns1__srmReleaseFilesResponse_ *rep) {

	static const char *funcName = "ns1__srmReleaseFiles()";
	return handle_request<ns1__srmReleaseFilesResponse_,
			ns1__srmReleaseFilesResponse, storm::ReleaseFilesRequest,
			ns1__srmReleaseFilesRequest>(funcName, soap, req, rep,
			&(rep->srmReleaseFilesResponse));
}

int ns1__srmPutDone(struct soap* soap, struct ns1__srmPutDoneRequest *req,
		struct ns1__srmPutDoneResponse_ *rep) {

	static const char *funcName = "ns1__srmPutDone()";
	return handle_request<ns1__srmPutDoneResponse_, ns1__srmPutDoneResponse,
			storm::PutDoneRequest, ns1__srmPutDoneRequest>(funcName, soap, req,
			rep, &(rep->srmPutDoneResponse));
}

int ns1__srmAbortRequest(struct soap* soap,
		struct ns1__srmAbortRequestRequest *req,
		struct ns1__srmAbortRequestResponse_ *rep) {

	static const char *funcName = "ns1__srmAbortRequest()";
	return handle_request<ns1__srmAbortRequestResponse_,
			ns1__srmAbortRequestResponse, storm::AbortRequestRequest,
			ns1__srmAbortRequestRequest>(funcName, soap, req, rep,
			&(rep->srmAbortRequestResponse));
}

int ns1__srmAbortFiles(struct soap* soap, struct ns1__srmAbortFilesRequest *req,
		struct ns1__srmAbortFilesResponse_ *rep) {

	static const char *funcName = "ns1__srmAbortFiles()";
	return handle_request<ns1__srmAbortFilesResponse_,
			ns1__srmAbortFilesResponse, storm::AbortFilesRequest,
			ns1__srmAbortFilesRequest>(funcName, soap, req, rep,
			&(rep->srmAbortFilesResponse));
}

int ns1__srmSuspendRequest(struct soap* soap,
		struct ns1__srmSuspendRequestRequest *req,
		struct ns1__srmSuspendRequestResponse_ *rep) {

	static const char *funcName = "ns1__srmSuspendRequest()";
	return handle_request<ns1__srmSuspendRequestResponse_,
			ns1__srmSuspendRequestResponse, storm::SuspendRequestRequest,
			ns1__srmSuspendRequestRequest>(funcName, soap, req, rep,
			&(rep->srmSuspendRequestResponse));
}

int ns1__srmResumeRequest(struct soap* soap,
		struct ns1__srmResumeRequestRequest *req,
		struct ns1__srmResumeRequestResponse_ *rep) {

	static const char *funcName = "ns1__srmResumeRequest()";
	return handle_request<ns1__srmResumeRequestResponse_,
			ns1__srmResumeRequestResponse, storm::ResumeRequestRequest,
			ns1__srmResumeRequestRequest>(funcName, soap, req, rep,
			&(rep->srmResumeRequestResponse));
}

int ns1__srmExtendFileLifeTime(struct soap* soap,
		struct ns1__srmExtendFileLifeTimeRequest *req,
		struct ns1__srmExtendFileLifeTimeResponse_ *rep) {

	static const char *funcName = "ns1__srmExtendFileLifeTime()";
	return handle_request<ns1__srmExtendFileLifeTimeResponse_,
			ns1__srmExtendFileLifeTimeResponse,
			storm::ExtendFileLifeTimeRequest, ns1__srmExtendFileLifeTimeRequest>(
			funcName, soap, req, rep, &(rep->srmExtendFileLifeTimeResponse));
}

int ns1__srmGetTransferProtocols(struct soap* soap,
		struct ns1__srmGetTransferProtocolsRequest *req,
		struct ns1__srmGetTransferProtocolsResponse_ *rep) {

	static const char *funcName = "ns1__srmGetTransferProtocols()";
	return handle_request<ns1__srmGetTransferProtocolsResponse_,
			ns1__srmGetTransferProtocolsResponse,
			storm::GetTransferProtocolsRequest,
			ns1__srmGetTransferProtocolsRequest>(funcName, soap, req, rep,
			&(rep->srmGetTransferProtocolsResponse));
}

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req,
		struct ns1__srmPingResponse_ *rep) {

	char *funcName = "ns1__srmPing()";
	return handle_request<ns1__srmPingResponse_, ns1__srmPingResponse,
			storm::PingRequest, ns1__srmPingRequest>(funcName, soap, req, rep,
			&(rep->srmPingResponse));
}
