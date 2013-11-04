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

#ifndef PUT_STATUS_REQUEST_HPP
#define PUT_STATUS_REQUEST_HPP

#include "FileStatusRequest.hpp"

namespace storm{

class PutStatusRequest : public FileStatusRequest<ns1__srmStatusOfPutRequestRequest , ns1__srmStatusOfPutRequestResponse>
{
public:
	PutStatusRequest(struct soap* soap, ns1__srmStatusOfPutRequestRequest* req):
		FileStatusRequest<ns1__srmStatusOfPutRequestRequest, ns1__srmStatusOfPutRequestResponse>(soap, req) {
		this->load(req);
	}

	virtual void load(ns1__srmStatusOfPutRequestRequest* req);

	void loadFromDB(struct srm_dbfd* db);

	ns1__srmStatusOfPutRequestResponse* buildResponse();

	static const std::string NAME;
	static const std::string MONITOR_NAME;

protected:
	void addMissingSurls();
};
}

#endif //PUT_STATUS_REQUEST_HPP
