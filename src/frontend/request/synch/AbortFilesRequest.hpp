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

#ifndef ABORT_FILES_REQUEST_HPP
#define ABORT_FILES_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class AbortFilesRequest: public SynchRequest<ns1__srmAbortFilesRequest, ns1__srmAbortFilesResponse,ns1__srmAbortFilesResponse_> {
public:
	AbortFilesRequest(struct soap* soapRequest, struct ns1__srmAbortFilesRequest* request, std::string requestName, std::string monitorName) throw (invalid_request) :
		SynchRequest<ns1__srmAbortFilesRequest, ns1__srmAbortFilesResponse, ns1__srmAbortFilesResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~AbortFilesRequest() {}

	int performXmlRpcCall(ns1__srmAbortFilesResponse_* response);

    void load(ns1__srmAbortFilesRequest* req) throw (invalid_request);

	void buildResponse() throw (std::logic_error , InvalidResponse);

	std::string getRequestToken() throw (std::logic_error)
	{
		return m_requestToken;
	}
private:
	std::string m_requestToken;
};
}
#endif // ABORT_FILES_REQUEST_HPP
