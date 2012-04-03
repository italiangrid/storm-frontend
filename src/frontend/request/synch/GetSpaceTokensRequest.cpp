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

#include "GetSpaceTokensRequest.hpp"
#include "srmlogit.h"

void storm::GetSpaceTokensRequest::load(ns1__srmGetSpaceTokensRequest* request) throw (storm::invalid_request)
{
	if(request->userSpaceTokenDescription != NULL)
	{
		m_userSpaceTokenDescription = request->userSpaceTokenDescription;
	}
}

int storm::GetSpaceTokensRequest::performXmlRpcCall(ns1__srmGetSpaceTokensResponse_* response){
	char *funcName = "GetSpaceTokensRequest::performXmlRpcCall()";
	int ret = ns1__srmGetSpaceTokens_impl(m_soapRequest, m_request, response);
	if(response->srmGetSpaceTokensResponse != NULL && response->srmGetSpaceTokensResponse->returnStatus != NULL )
	{
		m_status = response->srmGetSpaceTokensResponse->returnStatus->statusCode;
		if(response->srmGetSpaceTokensResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmGetSpaceTokensResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::GetSpaceTokensRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::GetSpaceTokensRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::GetSpaceTokensRequest::buildResponse()", "called.\n");
	//fake
}
