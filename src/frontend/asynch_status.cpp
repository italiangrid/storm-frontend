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
#include "put_status.hpp"
#include "get_status.hpp"
#include "copy_status.hpp"
#include "bol_status.hpp"

extern "C"
int ns1__srmStatusOfPutRequest(struct soap *soap,
        struct ns1__srmStatusOfPutRequestRequest *req,
        struct ns1__srmStatusOfPutRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfPutRequest";

    storm::put_status status(soap);

    // If the request contains some surl, then fill the put_status object
    if (NULL != req->arrayOfTargetSURLs ) {
        for (int i=0; i<req->arrayOfTargetSURLs->__sizeurlArray; ++i) {
            status.add_requested_surl(req->arrayOfTargetSURLs->urlArray[i]);
        }
    }

    int soap_status = __process_request_status<ns1__srmStatusOfPutRequestResponse>
    (soap, req->requestToken, funcName, status, &rep->srmStatusOfPutRequestResponse);

    return soap_status;
}

extern "C"
int ns1__srmStatusOfGetRequest(struct soap *soap,
        struct ns1__srmStatusOfGetRequestRequest *req,
        struct ns1__srmStatusOfGetRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfGetRequest";

    storm::get_status status(soap);

    // If the request contains some surl, then fill the get_status object
    if (NULL != req->arrayOfSourceSURLs ) {
        for (int i=0; i<req->arrayOfSourceSURLs->__sizeurlArray; ++i) {
            status.add_requested_surl(req->arrayOfSourceSURLs->urlArray[i]);
        }
    }

    int soap_status = __process_request_status<ns1__srmStatusOfGetRequestResponse>
        (soap, req->requestToken, funcName, status, &rep->srmStatusOfGetRequestResponse);

    return soap_status;

}

extern "C"
int ns1__srmStatusOfBringOnlineRequest(struct soap *soap,
        struct ns1__srmStatusOfBringOnlineRequestRequest *req,
        struct ns1__srmStatusOfBringOnlineRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfBringOnLineRequest";

    storm::bol_status status(soap);

    // If the request contains some surl, then fill the get_status object
    if (NULL != req->arrayOfSourceSURLs ) {
        for (int i=0; i<req->arrayOfSourceSURLs->__sizeurlArray; ++i) {
            status.add_requested_surl(req->arrayOfSourceSURLs->urlArray[i]);
        }
    }

    int soap_status = __process_request_status<ns1__srmStatusOfBringOnlineRequestResponse>
        (soap, req->requestToken, funcName, status, &rep->srmStatusOfBringOnlineRequestResponse);

    return soap_status;
}

extern "C"
int ns1__srmStatusOfCopyRequest(struct soap *soap,
        struct ns1__srmStatusOfCopyRequestRequest *req,
        struct ns1__srmStatusOfCopyRequestResponse_ *rep)
{
    static const char* funcName = "srmStatusOfCopyRequest";

    storm::copy_status status(soap);

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
