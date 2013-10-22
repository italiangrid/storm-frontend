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

#ifndef ABORT_REQUEST_REQUEST_HPP
#define ABORT_REQUEST_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class AbortRequestRequest: public SynchRequest<ns1__srmAbortRequestRequest, ns1__srmAbortRequestResponse,ns1__srmAbortRequestResponse_> {
public:
	AbortRequestRequest(struct soap* soapRequest, struct ns1__srmAbortRequestRequest* request, std::string requestName, std::string monitorName):
		SynchRequest<ns1__srmAbortRequestRequest, ns1__srmAbortRequestResponse, ns1__srmAbortRequestResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~AbortRequestRequest() {}

	int performXmlRpcCall(ns1__srmAbortRequestResponse_* response);

    void load(ns1__srmAbortRequestRequest* req);

    int buildResponse();

	std::string getRequestToken()
	{
		return m_requestToken;
	}
private:
	std::string m_requestToken;
};
}
#endif // ABORT_REQUEST_REQUEST_HPP
