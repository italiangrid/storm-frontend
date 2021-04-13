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

#ifndef STATUS_LS_REQUEST_HPP
#define STATUS_LS_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class StatusLsRequest: public SynchRequest<ns1__srmStatusOfLsRequestRequest,
		ns1__srmStatusOfLsRequestResponse, ns1__srmStatusOfLsRequestResponse_> {
public:
	StatusLsRequest(struct soap* soapRequest,
			struct ns1__srmStatusOfLsRequestRequest* request):
			SynchRequest<ns1__srmStatusOfLsRequestRequest,
					ns1__srmStatusOfLsRequestResponse,
					ns1__srmStatusOfLsRequestResponse_>(soapRequest, request) {
		this->load(request);
	}

	int performXmlRpcCall(ns1__srmStatusOfLsRequestResponse_* response);

	void load(ns1__srmStatusOfLsRequestRequest* req);

	int buildResponse();

	std::string getRequestToken() {
		return m_requestToken;
	}

private:
	std::string m_requestToken;
	int m_offset;
	int m_count;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;

};
}
#endif // STATUS_LS_REQUEST_HPP
