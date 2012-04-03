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

//to be commented out
#include "Monitoring.hpp"
#include "InstrumentedMonitor.hpp"
#include "MonitorNotEnabledException.hpp"
///
#include "srmv2H.h"

#include "MonitoringHelper.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"

#include "srmlogit.h"
#include "get_socket_info.hpp"
#include "Authorization.hpp"
#include <stdsoap2.h>
#include "soap_util.hpp"
#include "synch.hpp"

#include "request/synch/MkdirRequest.hpp"
#include "request/synch/RmdirRequest.hpp"
#include "request/synch/RmRequest.hpp"
#include "request/synch/LsRequest.hpp"
#include "request/synch/StatusLsRequest.hpp"
#include "request/synch/MvRequest.hpp"
#include "request/synch/SetPermissionRequest.hpp"
#include "request/synch/CheckPermissionRequest.hpp"
#include "request/synch/GetPermissionRequest.hpp"
#include "request/synch/ReserveSpaceRequest.hpp"
#include "request/synch/StatusReserveSpaceRequest.hpp"
#include "request/synch/ReleaseSpaceRequest.hpp"
#include "request/synch/UpdateSpaceRequest.hpp"
#include "request/synch/StatusUpdateSpaceRequest.hpp"
#include "request/synch/GetSpaceMetaDataRequest.hpp"
#include "request/synch/GetSpaceTokensRequest.hpp"
#include "request/synch/ChangeSpaceForFilesRequest.hpp"
#include "request/synch/StatusChangeSpaceForFilesRequest.hpp"
#include "request/synch/ExtendFileLifeTimeInSpaceRequest.hpp"
#include "request/synch/PurgeFromSpaceRequest.hpp"
#include "request/synch/ReleaseFilesRequest.hpp"
#include "request/synch/PutDoneRequest.hpp"
#include "request/synch/AbortRequestRequest.hpp"
#include "request/synch/AbortFilesRequest.hpp"
#include "request/synch/SuspendRequestRequest.hpp"
#include "request/synch/ResumeRequestRequest.hpp"
#include "request/synch/ExtendFileLifeTimeRequest.hpp"
#include "request/synch/GetTransferProtocolsRequest.hpp"
#include "request/synch/PingRequest.hpp"

template<typename soap_in_t, typename soap_out_t, typename soap_out_root_t>
int process(storm::SynchRequest<soap_in_t, soap_out_t, soap_out_root_t>* request
		, char* funcName, boost::posix_time::ptime startTime
		, soap_out_root_t* response) {
	if(storm::Authorization::checkBlacklist(request->getSoapRequest()))
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		try
		{
			request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(storm::InvalidResponse& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
			storm::MonitoringHelper::registerOperationError(startTime,
							request->getmonitorName());
			srmLogResponse(request->getName().c_str(), SRM_USCOREFAILURE);
			return(SOAP_FATAL_ERROR);
		}
		storm::MonitoringHelper::registerOperation(startTime,
					request->getmonitorName(), request->getStatus());
		srmLogResponse(request->getName().c_str(), request->getStatus());
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}
	int ret_val = request->performXmlRpcCall(response);
	storm::MonitoringHelper::registerOperation(startTime, ret_val,
				request->getmonitorName(),
				request->getStatus());
	return ret_val;
}

//Directory Functions

int ns1__srmMkdir(struct soap* soap, struct ns1__srmMkdirRequest *req,
		struct ns1__srmMkdirResponse_ *rep) {

	char *funcName = "ns1__srmMkdir()";
	std::string requestName("Mkdir");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::MkdirRequest* request;
	try{ request = new storm::MkdirRequest(soap, req, requestName, storm::SRM_MKDIR_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_MKDIR_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
	    		request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmMkdirRequest, ns1__srmMkdirResponse,ns1__srmMkdirResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmMkdirResponse);
	//
	rep->srmMkdirResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmRmdir(struct soap* soap, struct ns1__srmRmdirRequest *req,
		struct ns1__srmRmdirResponse_ *rep) {

	char *funcName = "ns1__srmRmdir()";
	std::string requestName("Rmdir");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::RmdirRequest* request;
	try{ request = new storm::RmdirRequest(soap, req, requestName, storm::SRM_RMDIR_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_RMDIR_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmRmdirRequest, ns1__srmRmdirResponse,ns1__srmRmdirResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmRmdirResponse);
	//
	rep->srmRmdirResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmRm(struct soap* soap, struct ns1__srmRmRequest *req,
		struct ns1__srmRmResponse_ *rep) {

	char *funcName = "ns1__srmRm()";
	std::string requestName("Rm");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::RmRequest* request;
	try{ request = new storm::RmRequest(soap, req, requestName, storm::SRM_RM_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_RM_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmRmRequest, ns1__srmRmResponse,ns1__srmRmResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmRmResponse);
	//
	rep->srmRmResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmLs(struct soap* soap, struct ns1__srmLsRequest *req,
		struct ns1__srmLsResponse_ *rep) {

	char *funcName = "ns1__srmLs()";
	std::string requestName("Ls");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::LsRequest* request;
	try{ request = new storm::LsRequest(soap, req, requestName, storm::SRM_LS_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_LS_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmLsRequest, ns1__srmLsResponse,ns1__srmLsResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmLsResponse);
	//
	rep->srmLsResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmStatusOfLsRequest(struct soap* soap, struct ns1__srmStatusOfLsRequestRequest *req,
		struct ns1__srmStatusOfLsRequestResponse_ *rep) {

	char *funcName = "ns1__srmStatusOfLsRequest()";
	std::string requestName("Ls status");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::StatusLsRequest* request;
	try{ request = new storm::StatusLsRequest(soap, req, requestName, storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getRequestToken().c_str());
	int returnValue = process<ns1__srmStatusOfLsRequestRequest, ns1__srmStatusOfLsRequestResponse,ns1__srmStatusOfLsRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmStatusOfLsRequestResponse);
	//
	rep->srmStatusOfLsRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmMv(struct soap* soap, struct ns1__srmMvRequest *req,
		struct ns1__srmMvResponse_ *rep) {

	char *funcName = "ns1__srmMv()";
	std::string requestName("Mv");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::MvRequest* request;
	try{ request = new storm::MvRequest(soap, req, requestName, storm::SRM_MV_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_MV_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmMvRequest, ns1__srmMvResponse,ns1__srmMvResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmMvResponse);
	//
	rep->srmMvResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

//Permission Functions

int ns1__srmSetPermission(struct soap* soap, struct ns1__srmSetPermissionRequest *req,
		struct ns1__srmSetPermissionResponse_ *rep) {

	char *funcName = "ns1__srmSetPermission()";
	std::string requestName("Set permission");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::SetPermissionRequest* request;
	try{ request = new storm::SetPermissionRequest(soap, req, requestName, storm::SRM_SET_PERMISSION_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_SET_PERMISSION_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmSetPermissionRequest,
			ns1__srmSetPermissionResponse, ns1__srmSetPermissionResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmSetPermissionResponse);
	//
	rep->srmSetPermissionResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmCheckPermission(struct soap* soap, struct ns1__srmCheckPermissionRequest *req,
		struct ns1__srmCheckPermissionResponse_ *rep) {

	char *funcName = "ns1__srmCheckPermission()";
	std::string requestName("Check permission");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::CheckPermissionRequest* request;
	try{ request = new storm::CheckPermissionRequest(soap, req, requestName, storm::SRM_CHECK_PERMISSION_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_CHECK_PERMISSION_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmCheckPermissionRequest,
			ns1__srmCheckPermissionResponse, ns1__srmCheckPermissionResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmCheckPermissionResponse);
	//
	rep->srmCheckPermissionResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmGetPermission(struct soap* soap, struct ns1__srmGetPermissionRequest *req,
		struct ns1__srmGetPermissionResponse_ *rep) {

	char *funcName = "ns1__srmGetPermission()";
	std::string requestName("Get permission");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::GetPermissionRequest* request;
	try{ request = new storm::GetPermissionRequest(soap, req, requestName, storm::SRM_GET_PERMISSION_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_GET_PERMISSION_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmGetPermissionRequest,
			ns1__srmGetPermissionResponse, ns1__srmGetPermissionResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmGetPermissionResponse);
	//
	rep->srmGetPermissionResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

//Space Management Functions

int ns1__srmReserveSpace(struct soap* soap, struct ns1__srmReserveSpaceRequest *req,
		struct ns1__srmReserveSpaceResponse_ *rep) {

	char *funcName = "ns1__srmReserveSpace()";
	std::string requestName("Reserve space");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ReserveSpaceRequest* request;
	try{ request = new storm::ReserveSpaceRequest(soap, req, requestName, storm::SRM_RESERVE_SPACE_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_RESERVE_SPACE_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequest(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str());
	int returnValue = process<ns1__srmReserveSpaceRequest,
			ns1__srmReserveSpaceResponse, ns1__srmReserveSpaceResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmReserveSpaceResponse);
	//
	rep->srmReserveSpaceResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmStatusOfReserveSpaceRequest(struct soap* soap, struct ns1__srmStatusOfReserveSpaceRequestRequest *req,
		struct ns1__srmStatusOfReserveSpaceRequestResponse_ *rep) {

	char *funcName = "ns1__srmStatusOfReserveSpaceRequest()";
	std::string requestName("Reserve space status");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::StatusReserveSpaceRequest* request;
	try{ request = new storm::StatusReserveSpaceRequest(soap, req, requestName, storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getRequestToken().c_str());
	int returnValue = process<ns1__srmStatusOfReserveSpaceRequestRequest, ns1__srmStatusOfReserveSpaceRequestResponse,ns1__srmStatusOfReserveSpaceRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmStatusOfReserveSpaceRequestResponse);
	//
	rep->srmStatusOfReserveSpaceRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmReleaseSpace(struct soap* soap, struct ns1__srmReleaseSpaceRequest *req,
		struct ns1__srmReleaseSpaceResponse_ *rep) {

	char *funcName = "ns1__srmReleaseSpace()";
	std::string requestName("Release space");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ReleaseSpaceRequest* request;
	try{ request = new storm::ReleaseSpaceRequest(soap, req, requestName, storm::SRM_RELEASE_SPACE_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_RELEASE_SPACE_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getSpaceToken().c_str());
	int returnValue = process<ns1__srmReleaseSpaceRequest, ns1__srmReleaseSpaceResponse,ns1__srmReleaseSpaceResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmReleaseSpaceResponse);
	//
	rep->srmReleaseSpaceResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmUpdateSpace(struct soap* soap, struct ns1__srmUpdateSpaceRequest *req,
		struct ns1__srmUpdateSpaceResponse_ *rep) {

	char *funcName = "ns1__srmUpdateSpace()";
	std::string requestName("Update space");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::UpdateSpaceRequest* request;
	try{ request = new storm::UpdateSpaceRequest(soap, req, requestName, storm::SRM_UPDATE_SPACE_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_UPDATE_SPACE_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getSpaceToken().c_str());
	int returnValue = process<ns1__srmUpdateSpaceRequest, ns1__srmUpdateSpaceResponse,ns1__srmUpdateSpaceResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmUpdateSpaceResponse);
	//
	rep->srmUpdateSpaceResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmStatusOfUpdateSpaceRequest(struct soap* soap, struct ns1__srmStatusOfUpdateSpaceRequestRequest *req,
		struct ns1__srmStatusOfUpdateSpaceRequestResponse_ *rep) {

	char *funcName = "ns1__srmStatusOfUpdateSpaceRequest()";
	std::string requestName("Update space status");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::StatusUpdateSpaceRequest* request;
	try{ request = new storm::StatusUpdateSpaceRequest(soap, req, requestName, storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getRequestToken().c_str());
	int returnValue = process<ns1__srmStatusOfUpdateSpaceRequestRequest, ns1__srmStatusOfUpdateSpaceRequestResponse,ns1__srmStatusOfUpdateSpaceRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmStatusOfUpdateSpaceRequestResponse);
	//
	rep->srmStatusOfUpdateSpaceRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmGetSpaceMetaData(struct soap* soap, struct ns1__srmGetSpaceMetaDataRequest *req,
		struct ns1__srmGetSpaceMetaDataResponse_ *rep) {

	char *funcName = "ns1__srmGetSpaceMetaData()";
	std::string requestName("Get space metadata");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::GetSpaceMetaDataRequest* request;
	try{ request = new storm::GetSpaceMetaDataRequest(soap, req, requestName, storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithTokenList(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getSpaceTokensList().c_str(), request->getSpaceTokensNumber());
	int returnValue = process<ns1__srmGetSpaceMetaDataRequest, ns1__srmGetSpaceMetaDataResponse,ns1__srmGetSpaceMetaDataResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmGetSpaceMetaDataResponse);
	//
	rep->srmGetSpaceMetaDataResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmGetSpaceTokens(struct soap* soap, struct ns1__srmGetSpaceTokensRequest *req,
		struct ns1__srmGetSpaceTokensResponse_ *rep) {

	char *funcName = "ns1__srmGetSpaceTokens()";
	std::string requestName("Get space tokens");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::GetSpaceTokensRequest* request;
	try{ request = new storm::GetSpaceTokensRequest(soap, req, requestName, storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequest(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str());
	int returnValue = process<ns1__srmGetSpaceTokensRequest, ns1__srmGetSpaceTokensResponse,ns1__srmGetSpaceTokensResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmGetSpaceTokensResponse);
	//
	rep->srmGetSpaceTokensResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmChangeSpaceForFiles(struct soap* soap, struct ns1__srmChangeSpaceForFilesRequest *req,
		struct ns1__srmChangeSpaceForFilesResponse_ *rep) {

	char *funcName = "ns1__srmChangeSpaceForFiles()";
	std::string requestName("Change space for files");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ChangeSpaceForFilesRequest* request;
	try{ request = new storm::ChangeSpaceForFilesRequest(soap, req, requestName, storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithTokenAndSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getTargetSpaceToken().c_str(), request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmChangeSpaceForFilesRequest, ns1__srmChangeSpaceForFilesResponse,ns1__srmChangeSpaceForFilesResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmChangeSpaceForFilesResponse);
	//
	rep->srmChangeSpaceForFilesResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmStatusOfChangeSpaceForFilesRequest(struct soap* soap, struct ns1__srmStatusOfChangeSpaceForFilesRequestRequest *req,
		struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse_ *rep) {

	char *funcName = "ns1__srmStatusOfChangeSpaceForFilesRequest()";
	std::string requestName("Change space for files status");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::StatusChangeSpaceForFilesRequest* request;
	try{ request = new storm::StatusChangeSpaceForFilesRequest(soap, req, requestName, storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getRequestToken().c_str());
	int returnValue = process<ns1__srmStatusOfChangeSpaceForFilesRequestRequest, ns1__srmStatusOfChangeSpaceForFilesRequestResponse,ns1__srmStatusOfChangeSpaceForFilesRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmStatusOfChangeSpaceForFilesRequestResponse);
	//
	rep->srmStatusOfChangeSpaceForFilesRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmExtendFileLifeTimeInSpace(struct soap* soap, struct ns1__srmExtendFileLifeTimeInSpaceRequest *req,
		struct ns1__srmExtendFileLifeTimeInSpaceResponse_ *rep) {

	char *funcName = "ns1__srmExtendFileLifeTimeInSpace()";
	std::string requestName("Extend file life time in space");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ExtendFileLifeTimeInSpaceRequest* request;
	try{ request = new storm::ExtendFileLifeTimeInSpaceRequest(soap, req, requestName, storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithTokenAndSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getSpaceToken().c_str(), request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmExtendFileLifeTimeInSpaceRequest, ns1__srmExtendFileLifeTimeInSpaceResponse,ns1__srmExtendFileLifeTimeInSpaceResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmExtendFileLifeTimeInSpaceResponse);
	//
	rep->srmExtendFileLifeTimeInSpaceResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmPurgeFromSpace(struct soap* soap, struct ns1__srmPurgeFromSpaceRequest *req,
		struct ns1__srmPurgeFromSpaceResponse_ *rep) {

	char *funcName = "ns1__srmPurgeFromSpace()";
	std::string requestName("Purge from space");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::PurgeFromSpaceRequest* request;
	try{ request = new storm::PurgeFromSpaceRequest(soap, req, requestName, storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithTokenAndSurls(requestName.c_str(), get_ip(soap).c_str(), request->getClientDN().c_str(), request->getSpaceToken().c_str(), request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmPurgeFromSpaceRequest, ns1__srmPurgeFromSpaceResponse,ns1__srmPurgeFromSpaceResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmPurgeFromSpaceResponse);
	//
	rep->srmPurgeFromSpaceResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

//Data Transfer Functions

int ns1__srmReleaseFiles(struct soap* soap, struct ns1__srmReleaseFilesRequest *req,
		struct ns1__srmReleaseFilesResponse_ *rep) {

	char *funcName = "ns1__srmReleaseFiles()";
	std::string requestName("Release files");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ReleaseFilesRequest* request;
	try{ request = new storm::ReleaseFilesRequest(soap, req, requestName, storm::SRM_RELEASE_FILES_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_RELEASE_FILES_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	if(request->hasRequestToken() && request->hasSurls())
	{
		srmLogRequestWithTokenAndSurls(requestName.c_str(),
				get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getRequestToken().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	}
	else
	{
		if(request->hasRequestToken())
		{
			srmLogRequestWithToken(requestName.c_str(),
					get_ip(soap).c_str(), request->getClientDN().c_str(),
					request->getRequestToken().c_str());
		}
		else
		{
			if(request->hasSurls())
			{
				srmLogRequestWithSurls(requestName.c_str(),
						get_ip(soap).c_str(), request->getClientDN().c_str(),
						request->getSurlsList().c_str(), request->getSurlsNumber());
			}
		}
	}
	int returnValue = process<ns1__srmReleaseFilesRequest, ns1__srmReleaseFilesResponse,ns1__srmReleaseFilesResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmReleaseFilesResponse);
	//
	rep->srmReleaseFilesResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmPutDone(struct soap* soap, struct ns1__srmPutDoneRequest *req,
		struct ns1__srmPutDoneResponse_ *rep) {

	char *funcName = "ns1__srmPutDone()";
	std::string requestName("Put done");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::PutDoneRequest* request;
	try{ request = new storm::PutDoneRequest(soap, req, requestName, storm::SRM_PUT_DONE_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_PUT_DONE_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithTokenAndSurls(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str(),
			request->getRequestToken().c_str(),
			request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmPutDoneRequest, ns1__srmPutDoneResponse,ns1__srmPutDoneResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmPutDoneResponse);
	//
	rep->srmPutDoneResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmAbortRequest(struct soap* soap, struct ns1__srmAbortRequestRequest *req,
		struct ns1__srmAbortRequestResponse_ *rep) {

	char *funcName = "ns1__srmAbortRequest()";
	std::string requestName("Abort request");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::AbortRequestRequest* request;
	try{ request = new storm::AbortRequestRequest(soap, req, requestName, storm::SRM_ABORT_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_ABORT_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str(),
			request->getRequestToken().c_str());
	int returnValue = process<ns1__srmAbortRequestRequest, ns1__srmAbortRequestResponse,ns1__srmAbortRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmAbortRequestResponse);
	//
	rep->srmAbortRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmAbortFiles(struct soap* soap, struct ns1__srmAbortFilesRequest *req,
		struct ns1__srmAbortFilesResponse_ *rep) {

	char *funcName = "ns1__srmAbortFiles()";
	std::string requestName("Abort files");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::AbortFilesRequest* request;
	try{ request = new storm::AbortFilesRequest(soap, req, requestName, storm::SRM_ABORT_FILES_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_ABORT_FILES_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithTokenAndSurls(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str(),
			request->getRequestToken().c_str(),
			request->getSurlsList().c_str(), request->getSurlsNumber());
	int returnValue = process<ns1__srmAbortFilesRequest, ns1__srmAbortFilesResponse,ns1__srmAbortFilesResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmAbortFilesResponse);
	//
	rep->srmAbortFilesResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmSuspendRequest(struct soap* soap, struct ns1__srmSuspendRequestRequest *req,
		struct ns1__srmSuspendRequestResponse_ *rep) {

	char *funcName = "ns1__srmSuspendRequest()";
	std::string requestName("Suspend request");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::SuspendRequestRequest* request;
	try{ request = new storm::SuspendRequestRequest(soap, req, requestName, storm::SRM_SUSPEND_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_SUSPEND_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str(),
			request->getRequestToken().c_str());
	int returnValue = process<ns1__srmSuspendRequestRequest, ns1__srmSuspendRequestResponse,ns1__srmSuspendRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmSuspendRequestResponse);
	//
	rep->srmSuspendRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmResumeRequest(struct soap* soap, struct ns1__srmResumeRequestRequest *req,
		struct ns1__srmResumeRequestResponse_ *rep) {

	char *funcName = "ns1__srmResumeRequest()";
	std::string requestName("Resume request");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ResumeRequestRequest* request;
	try{ request = new storm::ResumeRequestRequest(soap, req, requestName, storm::SRM_RESUME_REQUEST_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_RESUME_REQUEST_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequestWithToken(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str(),
			request->getRequestToken().c_str());
	int returnValue = process<ns1__srmResumeRequestRequest, ns1__srmResumeRequestResponse,ns1__srmResumeRequestResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmResumeRequestResponse);
	//
	rep->srmResumeRequestResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmExtendFileLifeTime(struct soap* soap, struct ns1__srmExtendFileLifeTimeRequest *req,
		struct ns1__srmExtendFileLifeTimeResponse_ *rep) {

	char *funcName = "ns1__srmExtendFileLifeTime()";
	std::string requestName("Extend file life time");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::ExtendFileLifeTimeRequest* request;
	try{ request = new storm::ExtendFileLifeTimeRequest(soap, req, requestName, storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	if(request->hasRequestToken())
	{
		srmLogRequestWithTokenAndSurls(requestName.c_str(),
				get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getRequestToken().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	}
	else
	{
		srmLogRequestWithSurls(requestName.c_str(),
				get_ip(soap).c_str(), request->getClientDN().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	}
	int returnValue = process<ns1__srmExtendFileLifeTimeRequest, ns1__srmExtendFileLifeTimeResponse,ns1__srmExtendFileLifeTimeResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmExtendFileLifeTimeResponse);
	//
	rep->srmExtendFileLifeTimeResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmGetTransferProtocols(struct soap* soap, struct ns1__srmGetTransferProtocolsRequest *req,
		struct ns1__srmGetTransferProtocolsResponse_ *rep) {

	char *funcName = "ns1__srmGetTransferProtocols()";
	std::string requestName("Get transfer protocols");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::GetTransferProtocolsRequest* request;
	try{ request = new storm::GetTransferProtocolsRequest(soap, req, requestName, storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequest(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str());
	int returnValue = process<ns1__srmGetTransferProtocolsRequest, ns1__srmGetTransferProtocolsResponse,ns1__srmGetTransferProtocolsResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmGetTransferProtocolsResponse);
	//
	rep->srmGetTransferProtocolsResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req,
		struct ns1__srmPingResponse_ *rep) {

	char *funcName = "ns1__srmPing()";
	std::string requestName("Ping");
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::local_time();
	storm::PingRequest* request;
	try{ request = new storm::PingRequest(soap, req, requestName, storm::SRM_PING_MONITOR_NAME); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(startTime,
						storm::SRM_PING_MONITOR_NAME);
		srmLogResponse(requestName.c_str(), SRM_USCOREFAILURE);
		return(SOAP_FATAL_ERROR);
	}
	srmLogRequest(requestName.c_str(),
			get_ip(soap).c_str(), request->getClientDN().c_str());
	int returnValue = process<ns1__srmPingRequest, ns1__srmPingResponse,ns1__srmPingResponse_>(request, funcName, startTime, rep);
	//cheat
	request->setResponse(rep->srmPingResponse);
	//
	rep->srmPingResponse = request->getResponse();
	srmLogResponse(requestName.c_str(), request->getStatus());
	delete request;
	return returnValue;
}
