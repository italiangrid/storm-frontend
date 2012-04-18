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

#ifndef RELEASE_FILES_REQUEST_HPP
#define RELEASE_FILES_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class ReleaseFilesRequest: public SynchRequest<ns1__srmReleaseFilesRequest, ns1__srmReleaseFilesResponse,ns1__srmReleaseFilesResponse_> {
public:
	ReleaseFilesRequest(struct soap* soapRequest, struct ns1__srmReleaseFilesRequest* request, std::string requestName, std::string monitorName) throw (invalid_request) :
		SynchRequest<ns1__srmReleaseFilesRequest, ns1__srmReleaseFilesResponse, ns1__srmReleaseFilesResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~ReleaseFilesRequest() {}

	int performXmlRpcCall(ns1__srmReleaseFilesResponse_* response);

    void load(ns1__srmReleaseFilesRequest* req) throw (invalid_request);

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
	bool m_doRemove;
};
}
#endif // RELEASE_FILES_REQUEST_HPP