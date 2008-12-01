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
#include "get_status.hpp"
#include "srmv2_status_template.hpp"

EXTERN_C int ns1__srmStatusOfGetRequest(struct soap *soap,
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
    
    return  __process_request_status<ns1__srmStatusOfGetRequestResponse> (soap, req->requestToken, status, &rep->srmStatusOfGetRequestResponse);

}
