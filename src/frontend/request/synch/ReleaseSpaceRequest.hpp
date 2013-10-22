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

#ifndef RELEASE_SPACE_REQUEST_HPP
#define RELEASE_SPACE_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class ReleaseSpaceRequest: public SynchRequest<ns1__srmReleaseSpaceRequest,
		ns1__srmReleaseSpaceResponse, ns1__srmReleaseSpaceResponse_> {
public:
	ReleaseSpaceRequest(struct soap* soapRequest,
			struct ns1__srmReleaseSpaceRequest* request,
			std::string requestName, std::string monitorName):
			SynchRequest<ns1__srmReleaseSpaceRequest,
					ns1__srmReleaseSpaceResponse, ns1__srmReleaseSpaceResponse_>(
					soapRequest, request, requestName, monitorName) {
		this->load(request);
	}

	virtual ~ReleaseSpaceRequest() {
	}

	int performXmlRpcCall(ns1__srmReleaseSpaceResponse_* response);

	void load(ns1__srmReleaseSpaceRequest* req);

	int buildResponse();

	std::string getSpaceToken() {
		return m_spaceToken;
	}
private:
	std::string m_spaceToken;
	bool m_forceFileRelease;
};
}
#endif // RELEASE_SPACE_REQUEST_HPP
