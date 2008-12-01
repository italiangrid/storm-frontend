// Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID.
/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "srmv2H.h"
#include "srmlogit.h"
#include "bol_status.hpp"
#include "srmv2_status_template.hpp"

EXTERN_C int ns1__srmStatusOfBringOnlineRequest(struct soap *soap,
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
    return(SOAP_OK);
    
//    storm::bol_status status(soap);
//
//    // If the request contains some surl, then fill the bol_status
//    // object
//    if(NULL != req->arrayOfSourceSURLs )
//        {
//            for(int i=0; 
//                i<req->arrayOfSourceSURLs->__sizeurlArray;              
//                ++i)
//                {
//                    status.add_requested_surl(req->arrayOfSourceSURLs->urlArray[i]);
//                }
//        }
//    
//    return  __process_request_status<ns1__srmStatusOfBringOnlineRequestResponse>
//        (soap, req->requestToken, status, &rep->srmStatusOfBringOnlineRequestResponse);
}
