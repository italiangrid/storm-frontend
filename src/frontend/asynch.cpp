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
#include "srmlogit.h"
#include "filerequest_template.hpp"
#include "PtpRequest.hpp"
#include "PtgRequest.hpp"
#include "CopyRequest.hpp"
#include "BolRequest.hpp"

#include "Credentials.hpp"
#include "Authorization.hpp"
#include "MonitoringHelper.hpp"

#include "get_socket_info.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "storm_exception.hpp"
#include <boost/format.hpp>
#include "base_request.hpp"



int ns1__srmPrepareToPut(struct soap *soap, struct ns1__srmPrepareToPutRequest *req,
        struct ns1__srmPrepareToPutResponse_ *rep) {

    static const char* funcName = "srmPrepareToPut";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::PtpRequest* request = 0;
    try{ request = new storm::PtpRequest(soap, req); }
    catch(storm::invalid_request& e)
    {
    	storm::request::register_request_error<storm::PtpRequest>(
    			__func__,
    			SRM_USCOREINVALID_USCOREREQUEST,
    			start_time,
				boost::str(boost::format("%s\n") % e.what())
				);

    	rep->srmPrepareToPutResponse =
    			storm::build_error_message_response<ns1__srmPrepareToPutResponse>(
    					soap,
    					SRM_USCOREINVALID_USCOREREQUEST,
    					e.what());


		return SOAP_OK;
    }
    srmLogRequestWithSurls("PTP", get_ip(soap).c_str(),
			request->getClientDN().c_str(), request->getSurlsList().c_str(),
			request->getSurlsNumber());
    bool blacklisted = false;
    try
    {
    	blacklisted = storm::authz::is_blacklisted(soap);
    }catch( storm::authorization_error& e){

    	srmlogit(STORM_LOG_ERROR, funcName, "Unable to check user blacklisting. Error: %s\n" , e.what());
		request->invalidateRequestToken();
		try
		{
			rep->srmPrepareToPutResponse = request->buildSpecificResponse(SRM_USCOREFAILURE, "Unable to check user blacklisting");
		} catch(storm::storm_error &exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
							storm::SRM_PREPARE_TO_PUT_MONITOR_NAME);
			srmLogResponse("PTP", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,e.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_PREPARE_TO_PUT_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("PTP", request->getStatus());
		delete request;
		return(SOAP_OK);
    }

    if(blacklisted)
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		request->invalidateRequestToken();
		request->setAuthorizationFailureSurls();
		try
		{
			rep->srmPrepareToPutResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(storm::storm_error &e)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , e.what());
			delete request;
	    	storm::MonitoringHelper::registerOperationError(start_time,
	    					storm::SRM_PREPARE_TO_PUT_MONITOR_NAME);
	    	srmLogResponse("PTP", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,e.what(),0);
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_PREPARE_TO_PUT_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("PTP", request->getStatus());
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

    int soap_status = __process_file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse> (
            soap, *request, funcName, req, &rep->srmPrepareToPutResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_PREPARE_TO_PUT_MONITOR_NAME,
				rep->srmPrepareToPutResponse->returnStatus->statusCode);
    srmLogResponseWithToken("PTP", request->getRequestToken().c_str(),
    			request->getStatus());
	delete request;
    return soap_status;
}

int ns1__srmPrepareToGet(struct soap *soap, struct ns1__srmPrepareToGetRequest *req,
        struct ns1__srmPrepareToGetResponse_ *rep) {

    static const char* funcName = "srmPrepareToGet";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::PtgRequest* request = 0;
    try{ request = new storm::PtgRequest(soap, req); }
    catch(storm::invalid_request& e)
    {
    	storm::request::register_request_error<storm::PtgRequest>(
    	    			__func__,
    	    			SRM_USCOREINVALID_USCOREREQUEST,
    	    			start_time,
    					boost::str(boost::format("%s\n") % e.what())
    					);

    	rep->srmPrepareToGetResponse =
    	    			storm::build_error_message_response<ns1__srmPrepareToGetResponse>(
    	    					soap,
    	    					SRM_USCOREINVALID_USCOREREQUEST,
    	    					e.what());


    	return SOAP_OK;
    }
	srmLogRequestWithSurls("PTG", get_ip(soap).c_str(),
			request->getClientDN().c_str(), request->getSurlsList().c_str(),
			request->getSurlsNumber());
    bool blacklisted = false;
    try
    {
    	blacklisted = storm::authz::is_blacklisted(soap);
    }catch( storm::authorization_error& e)
    {
    	srmlogit(STORM_LOG_ERROR, funcName, "Unable to check user blacklisting. Error: %s\n" , e.what());
		request->invalidateRequestToken();
		try
		{
			rep->srmPrepareToGetResponse = request->buildSpecificResponse(SRM_USCOREFAILURE, "Unable to check user blacklisting");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
							storm::SRM_PREPARE_TO_GET_MONITOR_NAME);
			srmLogResponse("PTG", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,e.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_PREPARE_TO_GET_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("PTG", request->getStatus());
		delete request;
		return(SOAP_OK);
    }

    if(blacklisted)
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		request->invalidateRequestToken();
		request->setAuthorizationFailureSurls();
		try
		{
			rep->srmPrepareToGetResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
	    	storm::MonitoringHelper::registerOperationError(start_time,
	    					storm::SRM_PREPARE_TO_GET_MONITOR_NAME);
	    	srmLogResponse("PTG", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,exc.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_PREPARE_TO_GET_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("PTG", request->getStatus());
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

    int soap_status = __process_file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse> (
            soap, *request, funcName, req, &rep->srmPrepareToGetResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_PREPARE_TO_GET_MONITOR_NAME,
				rep->srmPrepareToGetResponse->returnStatus->statusCode);
    srmLogResponseWithToken("PTG", request->getRequestToken().c_str(),
    			request->getStatus());
	delete request;
    return soap_status;
}

int ns1__srmCopy(struct soap *soap, struct ns1__srmCopyRequest *req,
        struct ns1__srmCopyResponse_ *rep) {

    static const char* funcName = "srmCopy";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::CopyRequest* request = 0;
    try{ request = new storm::CopyRequest(soap, req); }
    catch(storm::invalid_request& e)
    {
    	storm::request::register_request_error<storm::CopyRequest>(
    	    			__func__,
    	    			SRM_USCOREINVALID_USCOREREQUEST,
    	    			start_time,
    					boost::str(boost::format("%s\n") % e.what())
    					);

    	rep->srmCopyResponse =
    	    			storm::build_error_message_response<ns1__srmCopyResponse>(
    	    					soap,
    	    					SRM_USCOREINVALID_USCOREREQUEST,
    	    					e.what());


    	return SOAP_OK;
    }
	srmLogRequestWithSurls("CP", get_ip(soap).c_str(),
			request->getClientDN().c_str(), request->getSurlsList().c_str(),
			request->getSurlsNumber());
    bool blacklisted = false;
    try
    {
    	blacklisted = storm::authz::is_blacklisted(soap);
    }catch(storm::authorization_error& e)
    {
    	srmlogit(STORM_LOG_ERROR, funcName, "Unable to check user blacklisting. Error: %s\n" , e.what());
		request->invalidateRequestToken();
		try
		{
			rep->srmCopyResponse = request->buildSpecificResponse(SRM_USCOREFAILURE, "Unable to check user blacklisting");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
							storm::SRM_COPY_MONITOR_NAME);
			srmLogResponse("CP", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,e.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_COPY_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("CP", request->getStatus());
		delete request;
		return(SOAP_OK);
    }

    if(blacklisted)
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		request->invalidateRequestToken();
		try
		{
			rep->srmCopyResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
	    	storm::MonitoringHelper::registerOperationError(start_time,
	    					storm::SRM_COPY_MONITOR_NAME);
	    	srmLogResponse("CP", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,exc.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_COPY_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("CP", request->getStatus());
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

    int soap_status = __process_file_request<ns1__srmCopyRequest, ns1__srmCopyResponse> (soap, *request,
            funcName, req, &rep->srmCopyResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_COPY_MONITOR_NAME,
				rep->srmCopyResponse->returnStatus->statusCode);
	srmLogResponseWithToken("CP", request->getRequestToken().c_str(),
	    			request->getStatus());
	delete request;
    return soap_status;
}

int ns1__srmBringOnline(struct soap *soap, struct ns1__srmBringOnlineRequest *req,
        struct ns1__srmBringOnlineResponse_ *rep) {

    static const char* funcName = "srmBringOnline";
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    storm::BolRequest* request;
    try{ request = new storm::BolRequest(soap, req); }
    catch(storm::invalid_request& e)
    {
    	storm::request::register_request_error<storm::BolRequest>(
    	    	    			__func__,
    	    	    			SRM_USCOREINVALID_USCOREREQUEST,
    	    	    			start_time,
    	    					boost::str(boost::format("%s\n") % e.what())
    	    					);

    	rep->srmBringOnlineResponse =
    			storm::build_error_message_response<ns1__srmBringOnlineResponse>(
    					soap,
    					SRM_USCOREINVALID_USCOREREQUEST,
    					e.what());

    	return SOAP_OK;
    }
	srmLogRequestWithSurls("BOL", get_ip(soap).c_str(),
			request->getClientDN().c_str(), request->getSurlsList().c_str(),
			request->getSurlsNumber());
    bool blacklisted = false;
    try
    {
    	blacklisted = storm::authz::is_blacklisted(soap);
    }catch( storm::authorization_error& e)
    {
    	srmlogit(STORM_LOG_ERROR, funcName, "Unable to check user blacklisting. Error: %s\n" , e.what());
		request->invalidateRequestToken();
		request->setAuthorizationFailureSurls();
		try
		{
			rep->srmBringOnlineResponse = request->buildSpecificResponse(SRM_USCOREFAILURE, "Unable to check user blacklisting");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
			storm::MonitoringHelper::registerOperationError(start_time,
							storm::SRM_BRING_ONLINE_MONITOR_NAME);
			srmLogResponse("BOL", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,e.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_BRING_ONLINE_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("BOL", request->getStatus());
		delete request;
		return(SOAP_OK);
    }

    if(blacklisted)
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		request->invalidateRequestToken();
		try
		{
			rep->srmBringOnlineResponse = request->buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response.  %s\n" , exc.what());
			delete request;
	    	storm::MonitoringHelper::registerOperationError(start_time,
	    					storm::SRM_BRING_ONLINE_MONITOR_NAME);
	    	srmLogResponse("BOL", SRM_USCOREFAILURE);
			return soap_sender_fault(soap,exc.what(),0);;
		}
		storm::MonitoringHelper::registerOperation(start_time,
					storm::SRM_BRING_ONLINE_MONITOR_NAME,
					request->getStatus());
		srmLogResponse("BOL", request->getStatus());
		delete request;
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is not blacklisted\n");
	}

    int soap_status = __process_file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> (
            soap, *request, funcName, req, &rep->srmBringOnlineResponse);
	storm::MonitoringHelper::registerOperation(start_time, soap_status,
				storm::SRM_BRING_ONLINE_MONITOR_NAME,
				rep->srmBringOnlineResponse->returnStatus->statusCode);
    srmLogResponseWithToken("BOL", request->getRequestToken().c_str(),
    			request->getStatus());
	delete request;
    return soap_status;
}
