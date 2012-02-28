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

#ifndef __BOL_HPP
#define __BOL_HPP
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

// C includes
#include "srmv2H.h"

// STL includes
#include <vector>

// parent
#include "file_request.hpp"

#include "BolSurl.hpp"

namespace storm {
class bol: public file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> {
public:
    bol(struct soap *soap) :
        file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> (soap) {
        this->builtResponse = NULL;
    }
    ;
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmBringOnlineRequest *req);
    /*
     * Returns true if the bol command supports the option for specify the preferred transfer protocol
     * */
    bool supportsProtocolSpecification();
    
    /*
     * Returns the array of requested preferred transfer protocols
     * */
    std::vector<sql_string>* getRequestedProtocols();
    
    /*
     * Sets the array of requested preferred transfer protocols to the provided one
     * */
	void setProtocolVector(std::vector<sql_string>* protocolVector);

	/*
	 * Set the status code at SURL level to SRM_FAILURE to all requested SURLs
	 */
	void setGenericFailureSurls();
    ns1__srmBringOnlineResponse * response();

private:
    int remainingTotalRequestTime;
    int deferredStartTime;
    std::vector<BolSurl> surls;
    struct ns1__srmBringOnlineResponse * builtResponse;
    std::vector<sql_string> protocols;

};
}
#endif // __BOL_HPP
