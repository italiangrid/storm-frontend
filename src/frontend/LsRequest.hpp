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

#ifndef LS_REQUEST_HPP
#define LS_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class LsRequest: public SynchRequest<ns1__srmLsRequest, ns1__srmLsResponse,ns1__srmLsResponse_> {
public:
	LsRequest(struct soap* soapRequest, struct ns1__srmLsRequest* request):
			SynchRequest<ns1__srmLsRequest, ns1__srmLsResponse, ns1__srmLsResponse_>
					(soapRequest, request) ,
					m_fullDetailedList(false), m_allLevelRecursive(false),
					m_numOfLevels(-1), m_offset(-1), m_count(-1) {
		this->load(request);
    }
	virtual ~LsRequest() {}

	int performXmlRpcCall(ns1__srmLsResponse_* response);

    void load(ns1__srmLsRequest* req);

    int buildResponse();
private:
	ns1__TFileStorageType m_fileStorageType;
	bool m_fullDetailedList;
	bool m_allLevelRecursive;
	int m_numOfLevels;
	int m_offset;
	int m_count;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;

};
}
#endif // LS_REQUEST_HPP
