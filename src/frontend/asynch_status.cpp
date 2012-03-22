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
#include "status_template.hpp"
#include "StatusTemplate.hpp"
#include "PutStatusRequest.hpp"
#include "GetStatusRequest.hpp"
#include "copy_status.hpp"
#include "BolStatusRequest.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "MonitoringHelper.hpp"
#include "Credentials.hpp"
#include "Authorization.hpp"

#include "get_socket_info.hpp"

extern "C"
int ns1__srmStatusOfPutRequest(struct soap *soap,
        struct ns1__srmStatusOfPutRequestRequest *req,
        struct ns1__srmStatusOfPutRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfPutRequest";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::PutStatusRequest* request;
	try{ request = new storm::PutStatusRequest(soap, req); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
    	storm::MonitoringHelper::registerOperationError(start_time,
    					storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME);
		return(SOAP_FATAL_ERROR);
	}
	if(request->hasSurls())
	{
		srmLogRequestWithTokenAndSurls("PTP status", get_ip(soap).c_str(),
						request->getCredentials().getDN().c_str(), request->getRequestToken().c_str(), request->getSurlsList().c_str(),
						request->getSurlsNumber());
	}
	else
	{
		srmLogRequestWithToken("PTP status", get_ip(soap).c_str(),
						request->getCredentials().getDN().c_str(), request->getRequestToken().c_str());
	}

    if(Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		try{
			rep->srmStatusOfPutRequestResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(std::logic_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
							storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME);
			return(SOAP_FATAL_ERROR);
		}
		storm::MonitoringHelper::registerOperation(start_time,
							storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,
							SRM_USCOREAUTHORIZATION_USCOREFAILURE);
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

    int soap_status = processRequestStatus<ns1__srmStatusOfPutRequestRequest, ns1__srmStatusOfPutRequestResponse>
    (soap, funcName, *request, &rep->srmStatusOfPutRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,
				rep->srmStatusOfPutRequestResponse->returnStatus->statusCode);
	delete request;
    return soap_status;
}

extern "C"
int ns1__srmStatusOfGetRequest(struct soap *soap,
        struct ns1__srmStatusOfGetRequestRequest *req,
        struct ns1__srmStatusOfGetRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfGetRequest";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::GetStatusRequest* request;
	try{ request = new storm::GetStatusRequest(soap, req); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(start_time,
						storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME);
		return(SOAP_FATAL_ERROR);
	}
	if(request->hasSurls())
	{
		srmLogRequestWithTokenAndSurls("PTG status", get_ip(soap).c_str(),
						request->getCredentials().getDN().c_str(), request->getRequestToken().c_str(), request->getSurlsList().c_str(),
						request->getSurlsNumber());
	}
	else
	{
		srmLogRequestWithToken("PTG status", get_ip(soap).c_str(),
						request->getCredentials().getDN().c_str(), request->getRequestToken().c_str());
	}
    if(Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		try {
			rep->srmStatusOfGetRequestResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(std::logic_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
					storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME);
			return(SOAP_FATAL_ERROR);
		}
		storm::MonitoringHelper::registerOperation(start_time,
				storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,
				SRM_USCOREAUTHORIZATION_USCOREFAILURE);
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}
    int soap_status = processRequestStatus<ns1__srmStatusOfGetRequestRequest, ns1__srmStatusOfGetRequestResponse>
        (soap, funcName, *request, &rep->srmStatusOfGetRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,
				rep->srmStatusOfGetRequestResponse->returnStatus->statusCode);
	delete request;
    return soap_status;
}

extern "C"
int ns1__srmStatusOfBringOnlineRequest(struct soap *soap,
        struct ns1__srmStatusOfBringOnlineRequestRequest *req,
        struct ns1__srmStatusOfBringOnlineRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfBringOnLineRequest";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::BolStatusRequest* request;
	try{ request = new storm::BolStatusRequest(soap, req); }
	catch(storm::invalid_request& e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build request from soap. Invalid_request: %s\n" , e.what());
		storm::MonitoringHelper::registerOperationError(start_time,
						storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME);
		return(SOAP_FATAL_ERROR);
	}
	if(request->hasSurls())
	{
		srmLogRequestWithTokenAndSurls("BOL status", get_ip(soap).c_str(),
						request->getCredentials().getDN().c_str(), request->getRequestToken().c_str(), request->getSurlsList().c_str(),
						request->getSurlsNumber());
	}
	else
	{
		srmLogRequestWithToken("BOL status", get_ip(soap).c_str(),
						request->getCredentials().getDN().c_str(), request->getRequestToken().c_str());
	}
    if(Authorization::checkBlacklist(soap))
	{
    	srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
    	try {
			rep->srmStatusOfBringOnlineRequestResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(std::logic_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
					storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME);
			return(SOAP_FATAL_ERROR);
		}
		storm::MonitoringHelper::registerOperation(start_time,
									storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,
									SRM_USCOREAUTHORIZATION_USCOREFAILURE);
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

    int soap_status = processRequestStatus<ns1__srmStatusOfBringOnlineRequestRequest, ns1__srmStatusOfBringOnlineRequestResponse>
            (soap, funcName, *request, &rep->srmStatusOfBringOnlineRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,
				rep->srmStatusOfBringOnlineRequestResponse->returnStatus->statusCode);
    return soap_status;
}

extern "C"
int ns1__srmStatusOfCopyRequest(struct soap *soap,
        struct ns1__srmStatusOfCopyRequestRequest *req,
        struct ns1__srmStatusOfCopyRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfCopyRequest";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::copy_status status(soap);
    srmLogRequest("Cp status",get_ip(soap).c_str(),status.getClientDN().c_str());
    if(Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		rep->srmStatusOfCopyRequestResponse = status.error_response(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		storm::MonitoringHelper::registerOperation(start_time,
									storm::SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,
									SRM_USCOREAUTHORIZATION_USCOREFAILURE);
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}
    // If the request contains some surl, then fill the copy_status object
    if ((NULL != req->arrayOfSourceSURLs) && (NULL != req->arrayOfTargetSURLs)) {
        if (req->arrayOfSourceSURLs->__sizeurlArray == req->arrayOfTargetSURLs->__sizeurlArray) {
            for (int i = 0; i < req->arrayOfSourceSURLs->__sizeurlArray; ++i)
                status.add_requested_surl(req->arrayOfSourceSURLs->urlArray[i],
                        req->arrayOfTargetSURLs->urlArray[i]);
        }
    }

    int soap_status = __process_request_status<ns1__srmStatusOfCopyRequestResponse>
    (soap, req->requestToken, funcName, status, &rep->srmStatusOfCopyRequestResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,
				rep->srmStatusOfCopyRequestResponse->returnStatus->statusCode);
    return soap_status;

}

//extern "C"
//int ns1__srmStatusOfBringOnlineRequest(struct soap *soap,
//        struct ns1__srmStatusOfBringOnlineRequestRequest *req,
//        struct ns1__srmStatusOfBringOnlineRequestResponse_ *rep)
//{
//    static const char* func = "srmStatusOfBringOnLineRequest";
//    struct ns1__srmStatusOfBringOnlineRequestResponse *repp;
//    storm::Credentials credentials(soap);
//
//    srmlogit(STORM_LOG_INFO, func, "%s request from: %s\n", func, credentials.getDN().c_str());
//    srmlogit(STORM_LOG_INFO, func, "Client IP=%d.%d.%d.%d\n", (soap->ip >> 24) & 0xFF,
//             (soap->ip >> 16) & 0xFF, (soap->ip >> 8) & 0xFF, (soap->ip) & 0xFF);
//
//    try {
//        repp = storm::soap_calloc<ns1__srmStatusOfBringOnlineRequestResponse>(soap);
//        repp->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(soap);
//    }
//    catch (soap_bad_alloc) {
//        srmlogit(STORM_LOG_ERROR, func, "Memory allocation error (response structure)!\n");
//        return SOAP_EOM;
//    }
//    catch (std::invalid_argument) {
//        srmlogit(STORM_LOG_ERROR, func, "soap pointer is NULL!\n");
//        return SOAP_NULL;
//    }
//
//    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
//    repp->returnStatus->explanation = "srmBringOnLine implementation is synchronous";
//    rep->srmStatusOfBringOnlineRequestResponse = repp;
//    srmlogit(STORM_LOG_INFO, func, "Returning status: SRM_NOT_SUPPORTED\n");
//
//    return SOAP_OK;
//}
