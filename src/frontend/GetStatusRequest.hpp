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

#ifndef GET_STATUS_REQUEST_HPP
#define GET_STATUS_REQUEST_HPP

#include "FileStatusRequest.hpp"

namespace storm{

class GetStatusRequest : public FileStatusRequest<ns1__srmStatusOfGetRequestRequest , ns1__srmStatusOfGetRequestResponse>
{
public:
	GetStatusRequest(struct soap* soap, ns1__srmStatusOfGetRequestRequest* req):
		FileStatusRequest<ns1__srmStatusOfGetRequestRequest, ns1__srmStatusOfGetRequestResponse>(soap, req) {
		this->load(req);
	}

	virtual void load(ns1__srmStatusOfGetRequestRequest* req);

	void loadFromDB(struct srm_dbfd* db);

	ns1__srmStatusOfGetRequestResponse* buildResponse();

protected:
	void addMissingSurls();
};
}

#endif //GET_STATUS_REQUEST_HPP
