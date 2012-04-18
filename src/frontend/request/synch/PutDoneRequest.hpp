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

#ifndef PUT_DONE_REQUEST_HPP
#define PUT_DONE_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class PutDoneRequest: public SynchRequest<ns1__srmPutDoneRequest, ns1__srmPutDoneResponse,ns1__srmPutDoneResponse_> {
public:
	PutDoneRequest(struct soap* soapRequest, struct ns1__srmPutDoneRequest* request, std::string requestName, std::string monitorName) throw (invalid_request) :
		SynchRequest<ns1__srmPutDoneRequest, ns1__srmPutDoneResponse, ns1__srmPutDoneResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~PutDoneRequest() {}

	int performXmlRpcCall(ns1__srmPutDoneResponse_* response);

    void load(ns1__srmPutDoneRequest* req) throw (invalid_request);

	void buildResponse() throw (std::logic_error , InvalidResponse);

	std::string getRequestToken() throw (std::logic_error)
	{
		return m_requestToken;
	}
private:
	std::string m_requestToken;
};
}
#endif // PUT_DONE_REQUEST_HPP