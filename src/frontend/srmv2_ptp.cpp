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
#include "srmv2_filerequest_template.hpp"
#include "ptp.hpp"

extern "C" int ns1__srmPrepareToPut (struct soap *soap,
                          struct ns1__srmPrepareToPutRequest *req,
                          struct ns1__srmPrepareToPutResponse_ *rep)
{
    static const char * const func = "ns1_srmPrepareToPut(C++)";

    // Fill the ptp class
    storm::ptp request(soap);
    return __process_file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse>
        (soap, request, req, &rep->srmPrepareToPutResponse);
    
}

