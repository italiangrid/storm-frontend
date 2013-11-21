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

#ifndef RESERVE_SPACE_REQUEST_HPP
#define RESERVE_SPACE_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"
#include <vector>

namespace storm {

class ReserveSpaceRequest: public SynchRequest<ns1__srmReserveSpaceRequest,
		ns1__srmReserveSpaceResponse, ns1__srmReserveSpaceResponse_> {
public:
	ReserveSpaceRequest(struct soap* soapRequest,
			struct ns1__srmReserveSpaceRequest* request):
			SynchRequest<ns1__srmReserveSpaceRequest,
					ns1__srmReserveSpaceResponse, ns1__srmReserveSpaceResponse_>(
					soapRequest, request) {
		this->load(request);
	}

	virtual ~ReserveSpaceRequest() {
	}

	int performXmlRpcCall(ns1__srmReserveSpaceResponse_* response);

	void load(ns1__srmReserveSpaceRequest* req);

	int buildResponse();
private:
	std::string m_userSpaceTokenDescription;
	ns1__TRetentionPolicy m_retentionPolicy;
	ns1__TAccessLatency m_accessLatency;
	long m_desiredSizeOfTotalSpace;
	long m_desiredSizeOfGuaranteedSpace;
	int m_desiredLifetimeOfReservedSpace;
	std::vector<long> m_arrayOfExpectedFileSizes; // ignored
	ns1__TAccessPattern m_accessPattern;
	ns1__TConnectionType m_connectionType;
	std::vector<std::string> m_arrayOfClientNetworks;
	std::vector<std::string> m_arrayOfTransferProtocols;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;
};
}
#endif // RESERVE_SPACE_REQUEST_HPP
