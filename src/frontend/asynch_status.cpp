/*
 * asynch_status.cpp
 *
 *  Created on: Dec 12, 2008
 *      Author: alb
 */

#include "srmv2H.h"
#include "srmlogit.h"
#include "srmv2_status_template.hpp"
#include "put_status.hpp"
#include "get_status.hpp"
#include "copy_status.hpp"
#include "bol_status.hpp"

extern "C"
int ns1__srmStatusOfPutRequest(struct soap *soap,
        struct ns1__srmStatusOfPutRequestRequest *req,
        struct ns1__srmStatusOfPutRequestResponse_ *rep)
{
    storm::put_status status(soap);

    // If the request contains some surl, then fill the put_status object
    if (NULL != req->arrayOfTargetSURLs ) {
        for (int i=0; i<req->arrayOfTargetSURLs->__sizeurlArray; ++i) {
            status.add_requested_surl(req->arrayOfTargetSURLs->urlArray[i]);
        }
    }

    int soap_status = __process_request_status<ns1__srmStatusOfPutRequestResponse>
    (soap, req->requestToken, status, &rep->srmStatusOfPutRequestResponse);

    return soap_status;
}

extern "C"
int ns1__srmStatusOfGetRequest(struct soap *soap,
        struct ns1__srmStatusOfGetRequestRequest *req,
        struct ns1__srmStatusOfGetRequestResponse_ *rep)
{
    storm::get_status status(soap);

    // If the request contains some surl, then fill the get_status object
    if (NULL != req->arrayOfSourceSURLs ) {
        for (int i=0; i<req->arrayOfSourceSURLs->__sizeurlArray; ++i) {
            status.add_requested_surl(req->arrayOfSourceSURLs->urlArray[i]);
        }
    }

    int soap_status = __process_request_status<ns1__srmStatusOfGetRequestResponse>
        (soap, req->requestToken, status, &rep->srmStatusOfGetRequestResponse);

    return soap_status;

}

extern "C"
int ns1__srmStatusOfBringOnlineRequest(struct soap *soap,
        struct ns1__srmStatusOfBringOnlineRequestRequest *req,
        struct ns1__srmStatusOfBringOnlineRequestResponse_ *rep)
{
    static const char * const func = "ns1_srmStatusOfBringOnline(C++)";
    struct ns1__srmStatusOfBringOnlineRequestResponse *repp;

    try {
        repp = storm::soap_calloc<ns1__srmStatusOfBringOnlineRequestResponse>(soap);
        repp->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(soap);
    }
    catch (soap_bad_alloc) {
        srmlogit(STORM_LOG_ERROR, func, "Memory allocation error (response structure)!\n");
        return SOAP_EOM;
    }
    catch (std::invalid_argument) {
        srmlogit(STORM_LOG_ERROR, func, "soap pointer is NULL!\n");
        return SOAP_NULL;
    }

    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    repp->returnStatus->explanation = "srmBringOnLine implementation is synchronous";
    rep->srmStatusOfBringOnlineRequestResponse = repp;
    srmlogit(STORM_LOG_INFO, func, "Returning status: SRM_NOT_SUPPORTED\n");

    return SOAP_OK;
}
