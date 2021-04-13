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

#ifndef STATUS_UPDATE_SPACE_REQUEST_HPP
#define STATUS_UPDATE_SPACE_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class StatusUpdateSpaceRequest: public SynchRequest<
		ns1__srmStatusOfUpdateSpaceRequestRequest,
		ns1__srmStatusOfUpdateSpaceRequestResponse,
		ns1__srmStatusOfUpdateSpaceRequestResponse_> {
public:
	StatusUpdateSpaceRequest(struct soap* soapRequest,
			struct ns1__srmStatusOfUpdateSpaceRequestRequest* request) :
			SynchRequest<ns1__srmStatusOfUpdateSpaceRequestRequest,
					ns1__srmStatusOfUpdateSpaceRequestResponse,
					ns1__srmStatusOfUpdateSpaceRequestResponse_>(soapRequest,
					request) {
		this->load(request);
	}

	int performXmlRpcCall(
			ns1__srmStatusOfUpdateSpaceRequestResponse_* response);

	void load(ns1__srmStatusOfUpdateSpaceRequestRequest* req);

	int buildResponse();

	std::string getRequestToken() {
		return m_requestToken;
	}

private:
	std::string m_requestToken;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;

};
}
#endif // STATUS_UPDATE_SPACE_REQUEST_HPP
