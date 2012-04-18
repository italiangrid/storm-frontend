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

#ifndef EXTEND_FILE_LIFE_TIME_REQUEST_HPP
#define EXTEND_FILE_LIFE_TIME_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class ExtendFileLifeTimeRequest: public SynchRequest<ns1__srmExtendFileLifeTimeRequest, ns1__srmExtendFileLifeTimeResponse,ns1__srmExtendFileLifeTimeResponse_> {
public:
	ExtendFileLifeTimeRequest(struct soap* soapRequest, struct ns1__srmExtendFileLifeTimeRequest* request, std::string requestName, std::string monitorName) throw (invalid_request) :
		SynchRequest<ns1__srmExtendFileLifeTimeRequest, ns1__srmExtendFileLifeTimeResponse, ns1__srmExtendFileLifeTimeResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~ExtendFileLifeTimeRequest() {}

	int performXmlRpcCall(ns1__srmExtendFileLifeTimeResponse_* response);

    void load(ns1__srmExtendFileLifeTimeRequest* req) throw (invalid_request);

	void buildResponse() throw (std::logic_error , InvalidResponse);

	bool hasRequestToken()
	{
		return !m_requestToken.empty();
	}

	std::string getRequestToken() throw (std::logic_error)
	{
		if(!hasRequestToken())
		{
			throw std::logic_error("Unable to get the request token, token not available");
		}
		return m_requestToken;
	}
private:
	std::string m_requestToken;
	int m_newFileLifeTime;
	int m_newPinLifeTime;
};
}
#endif // EXTEND_FILE_LIFE_TIME_REQUEST_HPP