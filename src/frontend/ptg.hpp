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

#ifndef PTG_HPP_
#define PTG_HPP_

// C includes
#include "srm_server.h"
#include "srmv2H.h"
#include "stdsoap2.h"

// STL includes
#include <string>

// storm_db include
#include "sql_string.hpp"

// parent
#include "file_request.hpp"

#include "PtgSurl.hpp"

namespace storm {

class ptg: public storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse> {
public:
    ptg(struct soap *soap) :
        storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse>(soap)
	{
        this->builtResponse = NULL;
    }
    ;
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmPrepareToGetRequest *req);
    
    /*
     * Returns true if the ptg command supports the option for specify the preferred transfer protocol
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
    struct ns1__srmPrepareToGetResponse* response();

private:
    std::vector<PtgSurl> surls;
    struct ns1__srmPrepareToGetResponse * builtResponse;
    std::vector<sql_string> protocols;

};

}
#endif /*PTG_HPP_*/
