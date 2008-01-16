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


#include <string.h>
#include <sys/types.h>
#include <uuid/uuid.h>
#include "storm_functions.h"
#include "serrno.h"
#include "srm_server.h"
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"

#include <cgsi_plugin.h>

#include "srmv2_filerequest_template.hpp"
#include "bol.hpp"


extern "C" int ns1__srmBringOnline (struct soap *soap,
                          struct ns1__srmBringOnlineRequest *req,
                          struct ns1__srmBringOnlineResponse_ *rep)
{
    static const char * const func = "ns1_srmBringOnline(C++)";
    struct ns1__srmBringOnlineResponse *repp;
    
    try {	
	   repp = storm::soap_calloc<ns1__srmBringOnlineResponse>(soap);
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
    
	repp->returnStatus->statusCode = SRM_USCORESUCCESS;
	repp->returnStatus->explanation = "This functionality is not meaningful for StoRM.";
	rep->srmBringOnlineResponse = repp;
	srmlogit(STORM_LOG_INFO, func, "Completed. Status: SRM_SUCCESS\n");
	return(SOAP_OK);
	
//    // Fill the bol class
//    storm::bol request(soap);
//    return __process_file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse>
//        (soap, request, req, &rep->srmBringOnlineResponse);
    
}
