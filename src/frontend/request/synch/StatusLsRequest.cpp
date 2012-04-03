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


#include "StatusLsRequest.hpp"
#include "srmlogit.h"

void storm::StatusLsRequest::load(ns1__srmStatusOfLsRequestRequest* request) throw (storm::invalid_request)
{
	if(request->requestToken == NULL)
	{
		throw storm::invalid_request("Received NULL requestToken in the request");
	}
	m_requestToken = std::string(request->requestToken);
	if(request->count != NULL)
	{
		m_count = *request->count;
	}
	if(request->offset != NULL)
	{
		m_offset = *request->offset;
	}
}

int storm::StatusLsRequest::performXmlRpcCall(ns1__srmStatusOfLsRequestResponse_* response){
	char *funcName = "StatusLsRequest::performXmlRpcCall()";
	int ret = ns1__srmStatusOfLsRequest_impl(m_soapRequest, m_request, response);
	if(response->srmStatusOfLsRequestResponse != NULL && response->srmStatusOfLsRequestResponse->returnStatus != NULL )
	{
		m_status = response->srmStatusOfLsRequestResponse->returnStatus->statusCode;
		if(response->srmStatusOfLsRequestResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmStatusOfLsRequestResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::StatusLsRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::StatusLsRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::StatusLsRequest::buildResponse()", "called.\n");
	//fake
}
