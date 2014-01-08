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

#ifndef SET_PERMISSION_REQUEST_HPP
#define SET_PERMISSION_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class SetPermissionRequest: public SynchRequest<ns1__srmSetPermissionRequest,
		ns1__srmSetPermissionResponse, ns1__srmSetPermissionResponse_> {
public:
	SetPermissionRequest(struct soap* soapRequest,
			struct ns1__srmSetPermissionRequest* request):
			SynchRequest<ns1__srmSetPermissionRequest,
					ns1__srmSetPermissionResponse,
					ns1__srmSetPermissionResponse_>(soapRequest, request) {
		this->load(request);
	}

	virtual ~SetPermissionRequest() {
	}

	int performXmlRpcCall(ns1__srmSetPermissionResponse_* response);

	void load(ns1__srmSetPermissionRequest* req);

	int buildResponse();
private:
	ns1__TPermissionType m_permissionType;
	ns1__TPermissionMode m_ownerPermission;
	ns1__TPermissionMode m_otherPermission;
	std::set<std::pair<std::string, ns1__TPermissionMode> > m_userPermissions;
	std::set<std::pair<std::string, ns1__TPermissionMode> > m_groupPermissions;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;
};
}
#endif // SET_PERMISSION_REQUEST_HPP
