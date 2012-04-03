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

#ifndef SUSPEND_REQUEST_REQUEST_HPP
#define SUSPEND_REQUEST_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class SuspendRequestRequest: public SynchRequest<ns1__srmSuspendRequestRequest, ns1__srmSuspendRequestResponse,ns1__srmSuspendRequestResponse_> {
public:
	SuspendRequestRequest(struct soap* soapRequest, struct ns1__srmSuspendRequestRequest* request, std::string requestName, std::string monitorName) throw (invalid_request) :
		SynchRequest<ns1__srmSuspendRequestRequest, ns1__srmSuspendRequestResponse, ns1__srmSuspendRequestResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~SuspendRequestRequest() {}

	int performXmlRpcCall(ns1__srmSuspendRequestResponse_* response);

    void load(ns1__srmSuspendRequestRequest* req) throw (invalid_request);

	void buildResponse() throw (std::logic_error , InvalidResponse);

	std::string getRequestToken() throw (std::logic_error)
	{
		return m_requestToken;
	}
private:
	std::string m_requestToken;
};
}
#endif // SUSPEND_REQUEST_REQUEST_HPP
