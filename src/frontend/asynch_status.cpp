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

/*
 * asynch_status.cpp
 *
 *  Created on: Dec 12, 2008
 *      Author: alb
 */

#include "srmv2H.h"
#include "srmlogit.h"
#include "StatusTemplate.hpp"
#include "PutStatusRequest.hpp"
#include "GetStatusRequest.hpp"
#include "CopyStatusRequest.hpp"
#include "BolStatusRequest.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include "MonitoringHelper.hpp"
#include "Credentials.hpp"
#include "Authorization.hpp"

#include "get_socket_info.hpp"
#include "base_request.hpp"

int ns1__srmStatusOfPutRequest(struct soap *soap,
		struct ns1__srmStatusOfPutRequestRequest *req,
		struct ns1__srmStatusOfPutRequestResponse_ *rep) {
	static const char* funcName = "srmStatusOfPutRequest";
	boost::posix_time::ptime start_time =
			boost::posix_time::microsec_clock::local_time();
	storm::PutStatusRequest* request = 0;
	try {
		request = new storm::PutStatusRequest(soap, req);
	} catch (storm::invalid_request& e) {
		storm::request::register_request_error<storm::PutStatusRequest>(
				__func__, SRM_USCOREINVALID_USCOREREQUEST, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfPutRequestResponse =
				storm::build_error_message_response<
						ns1__srmStatusOfPutRequestResponse>(soap,
						SRM_USCOREINVALID_USCOREREQUEST, e.what());

		return SOAP_OK;
	}
	if (request->hasSurls()) {
		srmLogRequestWithTokenAndSurls("PTP status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	} else {
		srmLogRequestWithToken("PTP status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str());
	}
	
	bool is_blacklisted;
	
	try {
		is_blacklisted = storm::authz::is_blacklisted(soap);

	} catch ( storm::authorization_error& e ){

		storm::request::register_request_error<storm::PutStatusRequest>(
				__func__, SRM_USCOREAUTHORIZATION_USCOREFAILURE, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfPutRequestResponse =
			storm::build_error_message_response<
			ns1__srmStatusOfPutRequestResponse>(soap,
					SRM_USCOREAUTHORIZATION_USCOREFAILURE,
					e.what());

		return SOAP_OK;	

	}

	if (is_blacklisted) {
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		try {
			rep->srmStatusOfPutRequestResponse = request->buildSpecificResponse(
					SRM_USCOREAUTHORIZATION_USCOREFAILURE,
					"User not authorized");
		} catch (std::runtime_error& exc) {
			srmlogit(STORM_LOG_ERROR, funcName,
					"Unable to build soap response. logic_error: %s\n",
					exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
					storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME);
			srmLogResponse("PTP status", SRM_USCOREFAILURE);
			return soap_sender_fault(soap, exc.what(), 0);
		}
		storm::MonitoringHelper::registerOperation(start_time,
				storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,
				request->getStatus());
		srmLogResponse("PTP status", request->getStatus());
		delete request;
		return (SOAP_OK);
	} else {
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

	int soap_status = processRequestStatus<ns1__srmStatusOfPutRequestRequest,
			ns1__srmStatusOfPutRequestResponse>(soap, funcName, *request,
			&rep->srmStatusOfPutRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
			storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,
			rep->srmStatusOfPutRequestResponse->returnStatus->statusCode);
	srmLogResponse("PTP status", request->getStatus());
	delete request;
	return soap_status;
}

extern "C" int ns1__srmStatusOfGetRequest(struct soap *soap,
		struct ns1__srmStatusOfGetRequestRequest *req,
		struct ns1__srmStatusOfGetRequestResponse_ *rep) {
	static const char* funcName = "srmStatusOfGetRequest";
	boost::posix_time::ptime start_time =
			boost::posix_time::microsec_clock::local_time();
	storm::GetStatusRequest* request = 0;
	try {
		request = new storm::GetStatusRequest(soap, req);
	} catch (storm::invalid_request& e) {
		storm::request::register_request_error<storm::GetStatusRequest>(
				__func__, SRM_USCOREINVALID_USCOREREQUEST, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfGetRequestResponse =
				storm::build_error_message_response<
						ns1__srmStatusOfGetRequestResponse>(soap,
						SRM_USCOREINVALID_USCOREREQUEST, e.what());

		return SOAP_OK;
	}
	if (request->hasSurls()) {
		srmLogRequestWithTokenAndSurls("PTG status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	} else {
		srmLogRequestWithToken("PTG status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str());
	}

	bool is_blacklisted;

	try {
		is_blacklisted = storm::authz::is_blacklisted(soap);

	} catch ( storm::authorization_error& e ){

		storm::request::register_request_error<storm::GetStatusRequest>(
				__func__, SRM_USCOREAUTHORIZATION_USCOREFAILURE, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfGetRequestResponse =
			storm::build_error_message_response<
			ns1__srmStatusOfGetRequestResponse>(soap,
					SRM_USCOREAUTHORIZATION_USCOREFAILURE,
					e.what());

		return SOAP_OK;
	}
  
	if ( is_blacklisted ){
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		try {
			rep->srmStatusOfGetRequestResponse = request->buildSpecificResponse(
					SRM_USCOREAUTHORIZATION_USCOREFAILURE,
					"User not authorized");
		} catch (std::runtime_error& exc) {
			srmlogit(STORM_LOG_ERROR, funcName,
					"Unable to build soap response. logic_error: %s\n",
					exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
					storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME);
			srmLogResponse("PTG status", SRM_USCOREFAILURE);
			return soap_sender_fault(soap, exc.what(), 0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
				storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,
				request->getStatus());
		srmLogResponse("PTG status", request->getStatus());
		delete request;
		return (SOAP_OK);
	} else {
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}
	int soap_status = processRequestStatus<ns1__srmStatusOfGetRequestRequest,
			ns1__srmStatusOfGetRequestResponse>(soap, funcName, *request,
			&rep->srmStatusOfGetRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
			storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,
			rep->srmStatusOfGetRequestResponse->returnStatus->statusCode);
	srmLogResponse("PTG status", request->getStatus());
	delete request;
	return soap_status;
}

extern "C" int ns1__srmStatusOfBringOnlineRequest(struct soap *soap,
		struct ns1__srmStatusOfBringOnlineRequestRequest *req,
		struct ns1__srmStatusOfBringOnlineRequestResponse_ *rep) {
	static const char* funcName = "srmStatusOfBringOnLineRequest";
	boost::posix_time::ptime start_time =
			boost::posix_time::microsec_clock::local_time();
	storm::BolStatusRequest* request = 0;
	try {
		request = new storm::BolStatusRequest(soap, req);
	} catch (storm::invalid_request& e) {
		storm::request::register_request_error<storm::BolStatusRequest>(
				__func__, SRM_USCOREINVALID_USCOREREQUEST, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfBringOnlineRequestResponse =
				storm::build_error_message_response<
						ns1__srmStatusOfBringOnlineRequestResponse>(soap,
						SRM_USCOREINVALID_USCOREREQUEST, e.what());

		return SOAP_OK;
	}
	if (request->hasSurls()) {
		srmLogRequestWithTokenAndSurls("BOL status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	} else {
		srmLogRequestWithToken("BOL status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str());
	}
	
	bool is_blacklisted;

	try {
		is_blacklisted = storm::authz::is_blacklisted(soap);

	} catch ( storm::authorization_error& e ){

		storm::request::register_request_error<storm::BolStatusRequest>(
				__func__, SRM_USCOREAUTHORIZATION_USCOREFAILURE, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfBringOnlineRequestResponse =
			storm::build_error_message_response<
			ns1__srmStatusOfBringOnlineRequestResponse>(soap,
					SRM_USCOREAUTHORIZATION_USCOREFAILURE,
					e.what());

		return SOAP_OK;
	}

	if ( is_blacklisted ) {
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		try {
			rep->srmStatusOfBringOnlineRequestResponse =
					request->buildSpecificResponse(
							SRM_USCOREAUTHORIZATION_USCOREFAILURE,
							"User not authorized");
		} catch (std::runtime_error& exc) {
			srmlogit(STORM_LOG_ERROR, funcName,
					"Unable to build soap response. logic_error: %s\n",
					exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
					storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME);
			srmLogResponse("BOL status", SRM_USCOREFAILURE);
			return soap_sender_fault(soap, exc.what(), 0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
				storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,
				request->getStatus());
		srmLogResponse("BOL status", request->getStatus());
		delete request;
		return (SOAP_OK);
	} else {
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

	int soap_status = processRequestStatus<
			ns1__srmStatusOfBringOnlineRequestRequest,
			ns1__srmStatusOfBringOnlineRequestResponse>(soap, funcName,
			*request, &rep->srmStatusOfBringOnlineRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
			storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,
			rep->srmStatusOfBringOnlineRequestResponse->returnStatus->statusCode);
	srmLogResponse("BOL status", request->getStatus());
	delete request;
	return soap_status;
}

extern "C" int ns1__srmStatusOfCopyRequest(struct soap *soap,
		struct ns1__srmStatusOfCopyRequestRequest *req,
		struct ns1__srmStatusOfCopyRequestResponse_ *rep) {
	static const char* funcName = "srmStatusOfCopyRequest";
	boost::posix_time::ptime start_time =
			boost::posix_time::microsec_clock::local_time();
	storm::CopyStatusRequest* request = 0;
	try {
		request = new storm::CopyStatusRequest(soap, req);
	} catch (storm::invalid_request& e) {
		storm::request::register_request_error<storm::CopyStatusRequest>(
				__func__, SRM_USCOREINVALID_USCOREREQUEST, start_time,
				boost::str(boost::format("%s\n") % e.what()));

		rep->srmStatusOfCopyRequestResponse =
				storm::build_error_message_response<
						ns1__srmStatusOfCopyRequestResponse>(soap,
						SRM_USCOREINVALID_USCOREREQUEST, e.what());

		return SOAP_OK;
		srmlogit(STORM_LOG_ERROR, funcName,
				"Unable to build request from soap. Invalid_request: %s\n",
				e.what());
		storm::MonitoringHelper::registerOperationError(start_time,
				storm::SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME);
		srmLogResponse("CP status", SRM_USCOREFAILURE);
		return soap_sender_fault(soap, e.what(), 0);;
	}
	if (request->hasSurls()) {
		srmLogRequestWithTokenAndSurls("CP status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str(),
				request->getSurlsList().c_str(), request->getSurlsNumber());
	} else {
		srmLogRequestWithToken("CP status", get_ip(soap).c_str(),
				request->getCredentials().getDN().c_str(),
				request->getRequestToken().c_str());
	}

	try {
		rep->srmStatusOfCopyRequestResponse =
				request->buildSpecificResponse(
						SRM_USCORENOT_USCORESUPPORTED,
						"srmStatusOfCopy operation is not supported");
	} catch (std::runtime_error& exc) {
		srmlogit(STORM_LOG_ERROR, funcName,
				"Unable to build soap response. logic_error: %s\n",
				exc.what());
		delete request;
		storm::MonitoringHelper::registerOperationError(start_time,
				storm::SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME);
		srmLogResponse("CP status", SRM_USCOREFAILURE);
		return soap_sender_fault(soap, exc.what(), 0);;
	}
	storm::MonitoringHelper::registerOperation(start_time,
			storm::SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,
			request->getStatus());
	srmLogResponse("CP status", request->getStatus());
	delete request;
	return (SOAP_OK);
}
