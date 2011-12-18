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
 * asynch.cpp
 *
 *  Created on: Dec 12, 2008
 *      Author: alb
 */

#include "srmv2H.h"
#include "srmlogit.h"
#include "filerequest_template.hpp"
#include "ptp.hpp"
#include "ptg.hpp"
#include "copy.hpp"
#include "bol.hpp"
#include "srmlogit.h"

#include "Credentials.hpp"
#include "Authorization.hpp"

extern "C" int ns1__srmPrepareToPut(struct soap *soap, struct ns1__srmPrepareToPutRequest *req,
        struct ns1__srmPrepareToPutResponse_ *rep) {

    static const char* funcName = "srmPrepareToPut";
    storm::ptp request(soap);
    storm::Credentials cred(soap);
	storm::Authorization auth((storm::Credentials*)&cred);
	bool black = auth.isBlacklisted();
	if(black)
	{
		srmlogit(STORM_LOG_INFO, funcName, "The user is blacklisted\n");
		request.r_token("");
		rep->srmPrepareToPutResponse = request.error_response(SRM_USCOREAUTHORIZATION_USCOREFAILURE, "User not authorized");
		return(SOAP_OK);
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "The user is NOT blacklisted\n");
	}


    int soap_status = __process_file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse> (
            soap, request, funcName, req, &rep->srmPrepareToPutResponse);

    return soap_status;
}

extern "C" int ns1__srmPrepareToGet(struct soap *soap, struct ns1__srmPrepareToGetRequest *req,
        struct ns1__srmPrepareToGetResponse_ *rep) {

    static const char* funcName = "srmPrepareToGet";

    storm::ptg request(soap);

    int soap_status = __process_file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse> (
            soap, request, funcName, req, &rep->srmPrepareToGetResponse);

    return soap_status;
}

extern "C" int ns1__srmCopy(struct soap *soap, struct ns1__srmCopyRequest *req,
        struct ns1__srmCopyResponse_ *rep) {

    static const char* funcName = "srmCopy";

    storm::copy request(soap);

    int soap_status = __process_file_request<ns1__srmCopyRequest, ns1__srmCopyResponse> (soap, request,
            funcName, req, &rep->srmCopyResponse);

    return soap_status;
}

extern "C" int ns1__srmBringOnline(struct soap *soap, struct ns1__srmBringOnlineRequest *req,
        struct ns1__srmBringOnlineResponse_ *rep) {

    static const char* funcName = "srmBringOnline";

    storm::bol request(soap);

    int soap_status = __process_file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> (
            soap, request, funcName, req, &rep->srmBringOnlineResponse);

    return soap_status;

    //    static const char* funcName = "srmBringOnline";
    //    struct ns1__srmBringOnlineResponse *repp;
    //    storm::Credentials credentials(soap);
    //
    //    srmlogit(STORM_LOG_INFO, funcName, "%s request from: %s\n", funcName, credentials.getDN().c_str());
    //    srmlogit(STORM_LOG_INFO, funcName, "Client IP=%d.%d.%d.%d\n", (soap->ip >> 24) & 0xFF,
    //             (soap->ip >> 16) & 0xFF, (soap->ip >> 8) & 0xFF, (soap->ip) & 0xFF);
    //
    //    try {
    //        repp = storm::soap_calloc<ns1__srmBringOnlineResponse>(soap);
    //        repp->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(soap);
    //    }
    //    catch (soap_bad_alloc) {
    //        srmlogit(STORM_LOG_ERROR, funcName, "Memory allocation error (response structure)!\n");
    //        return SOAP_EOM;
    //    }
    //    catch (std::invalid_argument) {
    //        srmlogit(STORM_LOG_ERROR, funcName, "soap pointer is NULL!\n");
    //        return SOAP_NULL;
    //    }
    //
    //    repp->returnStatus->statusCode = SRM_USCORESUCCESS;
    //    repp->returnStatus->explanation = "This functionality is not meaningful for StoRM.";
    //    rep->srmBringOnlineResponse = repp;
    //
    //    srmlogit(STORM_LOG_INFO, funcName, "Completed. Status: SRM_SUCCESS\n");
    //
    //    return(SOAP_OK);
}
