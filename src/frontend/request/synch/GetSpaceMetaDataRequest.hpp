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

#ifndef GET_SPACE_META_DATA_REQUEST_HPP
#define GET_SPACE_META_DATA_REQUEST_HPP

#include "SynchRequest.hpp"
#include "srmv2H.h"

namespace storm {

class GetSpaceMetaDataRequest: public SynchRequest<ns1__srmGetSpaceMetaDataRequest, ns1__srmGetSpaceMetaDataResponse,ns1__srmGetSpaceMetaDataResponse_> {
public:
	GetSpaceMetaDataRequest(struct soap* soapRequest, struct ns1__srmGetSpaceMetaDataRequest* request, std::string requestName, std::string monitorName) throw (invalid_request) :
		SynchRequest<ns1__srmGetSpaceMetaDataRequest, ns1__srmGetSpaceMetaDataResponse, ns1__srmGetSpaceMetaDataResponse_> (soapRequest, request, requestName, monitorName) {
		this->load(request);
    }

	virtual ~GetSpaceMetaDataRequest() {}

	int performXmlRpcCall(ns1__srmGetSpaceMetaDataResponse_* response);

    void load(ns1__srmGetSpaceMetaDataRequest* req) throw (invalid_request);

    int buildResponse() throw (std::logic_error , InvalidResponse);

	std::string getSpaceTokensList()
	{
		std::string builtList;
		std::set<std::string>::const_iterator const vectorEnd = m_spaceTokens.end();
		bool first = true;
		for (std::set<std::string>::const_iterator i = m_spaceTokens.begin(); i != vectorEnd; ++i) {
			if(first)
			{
				first = false;
			}
			else
			{
				builtList += ' ';
			}
			builtList += *i;

		}
		return builtList;
	}

	int getSpaceTokensNumber()
	{
		return m_spaceTokens.size();
	}
private:
	std::set<std::string> m_spaceTokens;
};
}
#endif // GET_SPACE_META_DATA_REQUEST_HPP
