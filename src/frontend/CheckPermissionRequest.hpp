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

#ifndef CHECK_PERMISSION_REQUEST_HPP
#define CHECK_PERMISSION_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class CheckPermissionRequest: public SynchRequest<ns1__srmCheckPermissionRequest,
		ns1__srmCheckPermissionResponse, ns1__srmCheckPermissionResponse_> {
public:
	CheckPermissionRequest(struct soap* soapRequest, struct ns1__srmCheckPermissionRequest* request):
			SynchRequest<ns1__srmCheckPermissionRequest, ns1__srmCheckPermissionResponse, ns1__srmCheckPermissionResponse_>
			(soapRequest, request) {
		this->load(request);
	}

	int performXmlRpcCall(ns1__srmCheckPermissionResponse_* response);

    void load(ns1__srmCheckPermissionRequest* req);

    int buildResponse();

    static const std::string NAME;
    static const std::string MONITOR_NAME;

};
}
#endif // CHECK_PERMISSION_REQUEST_HPP
