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

#ifndef GET_SPACE_TOKENS_REQUEST_HPP
#define GET_SPACE_TOKENS_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class GetSpaceTokensRequest: public SynchRequest<ns1__srmGetSpaceTokensRequest, ns1__srmGetSpaceTokensResponse,ns1__srmGetSpaceTokensResponse_> {
public:
	GetSpaceTokensRequest(struct soap* soapRequest, struct ns1__srmGetSpaceTokensRequest* request) throw (invalid_request) :
		SynchRequest<ns1__srmGetSpaceTokensRequest, ns1__srmGetSpaceTokensResponse, ns1__srmGetSpaceTokensResponse_> (soapRequest, request) {
		this->load(request);
    }

	virtual ~GetSpaceTokensRequest() {}

	int performXmlRpcCall(ns1__srmGetSpaceTokensResponse_* response);

    void load(ns1__srmGetSpaceTokensRequest* req);

    int buildResponse();

	std::string getUserSpaceTokenDescription()
	{
		return m_userSpaceTokenDescription;
	}
private:
	std::string m_userSpaceTokenDescription;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;

};
}
#endif // GET_SPACE_TOKENS_REQUEST_HPP
