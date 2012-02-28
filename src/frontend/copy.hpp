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

#ifndef __COPY_HPP
#define __COPY_HPP

// C includes
#include "srmv2H.h"
#include "srm_server.h"
#include "stdsoap2.h"

// STL includes
#include <vector>

#include "CopySurl.hpp"

// parent
#include "file_request.hpp"

namespace storm {

class copy : public file_request<ns1__srmCopyRequest, ns1__srmCopyResponse> {
public:
    copy(struct soap *soap) :
			file_request<ns1__srmCopyRequest, ns1__srmCopyResponse>(soap) {
		builtResponse = NULL;
	};

    void insert(struct srm_dbfd *dbfd);

    void load(ns1__srmCopyRequest *req);

    /*
     * Returns true if the copy command supports the option for specify the preferred transfer protocol
     */
    bool supportsProtocolSpecification();
    
    /*
     * Returns the array of requested preferred transfer protocols
     */
    std::vector<sql_string>* getRequestedProtocols();
    
    /*
     * Sets the array of requested preferred transfer protocols to the provided one
     */
	void setProtocolVector(std::vector<sql_string>* protocolVector);

	/*
	 * Set the status code at SURL level to SRM_FAILURE to all requested SURLs
	 */
	void setGenericFailureSurls();

    ns1__srmCopyResponse * response();

private:
    std::vector<CopySurl> surls;
    struct ns1__srmCopyResponse * builtResponse;
    
};
}
#endif // __COPY_HPP
