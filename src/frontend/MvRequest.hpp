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

#ifndef MV_REQUEST_HPP
#define MV_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class MvRequest: public SynchRequest<ns1__srmMvRequest, ns1__srmMvResponse,
		ns1__srmMvResponse_> {
public:
	MvRequest(struct soap* soapRequest, struct ns1__srmMvRequest* request):
			SynchRequest<ns1__srmMvRequest, ns1__srmMvResponse,
					ns1__srmMvResponse_>(soapRequest, request) {
		this->load(request);
	}

	int performXmlRpcCall(ns1__srmMvResponse_* response);

	void load(ns1__srmMvRequest* req);

	int buildResponse();

	std::string from_surl() const;
	std::string to_surl() const;

private:
	std::string m_fromSURL;
	std::string m_toSURL;
public:
	static const std::string NAME;
	static const std::string MONITOR_NAME;

};
}
#endif // MV_REQUEST_HPP
