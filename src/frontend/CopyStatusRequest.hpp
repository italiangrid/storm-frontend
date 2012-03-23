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

#ifndef COPY_STATUS_REQUEST_HPP
#define COPY_STATUS_REQUEST_HPP

#include "FileStatusRequest.hpp"

namespace storm{

class CopyStatusRequest : public FileStatusRequest<ns1__srmStatusOfCopyRequestRequest , ns1__srmStatusOfCopyRequestResponse>
{
public:
	CopyStatusRequest(struct soap* soap, ns1__srmStatusOfCopyRequestRequest* req) throw (invalid_request) :
		FileStatusRequest<ns1__srmStatusOfCopyRequestRequest, ns1__srmStatusOfCopyRequestResponse>(soap, req) , m_allTargetSurlSpecified(true){
		this->load(req);
	}

	virtual void load(ns1__srmStatusOfCopyRequestRequest* req);

	void loadFromDB(struct srm_dbfd* db) throw (TokenNotFound);

	ns1__srmStatusOfCopyRequestResponse* buildResponse() throw (std::logic_error);

protected:
	void addMissingSurls() throw (std::logic_error);
private:
	bool m_allTargetSurlSpecified;

};
}

#endif //BOL_STATUS_REQUEST_HPP
