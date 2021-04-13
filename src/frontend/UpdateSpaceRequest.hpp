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

#ifndef UPDATE_SPACE_REQUEST_HPP
#define UPDATE_SPACE_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class UpdateSpaceRequest: public SynchRequest<ns1__srmUpdateSpaceRequest, ns1__srmUpdateSpaceResponse,ns1__srmUpdateSpaceResponse_> {
public:
	UpdateSpaceRequest(struct soap* soapRequest, struct ns1__srmUpdateSpaceRequest* request):
		SynchRequest<ns1__srmUpdateSpaceRequest, ns1__srmUpdateSpaceResponse, ns1__srmUpdateSpaceResponse_> (soapRequest, request) {
		this->load(request);
    }

	int performXmlRpcCall(ns1__srmUpdateSpaceResponse_* response);

    void load(ns1__srmUpdateSpaceRequest* req);

    int buildResponse();

	std::string getSpaceToken()
	{
		return m_spaceToken;
	}
private:
	std::string m_spaceToken;
	long m_newSizeOfTotalSpaceDesired;
	long m_newSizeOfGuaranteedSpaceDesired;
	int m_newLifeTime;

public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;
};
}
#endif // UPDATE_SPACE_REQUEST_HPP
