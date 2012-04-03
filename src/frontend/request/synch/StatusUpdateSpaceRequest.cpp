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

#include "StatusUpdateSpaceRequest.hpp"
#include "srmlogit.h"

void storm::StatusUpdateSpaceRequest::load(ns1__srmStatusOfUpdateSpaceRequestRequest* request) throw (storm::invalid_request)
{
	if(request->requestToken == NULL)
	{
		throw storm::invalid_request("Received NULL requestToken in the request");
	}
	m_requestToken = std::string(request->requestToken);
}

int storm::StatusUpdateSpaceRequest::performXmlRpcCall(ns1__srmStatusOfUpdateSpaceRequestResponse_* response){
	char *funcName = "StatusUpdateSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmStatusOfUpdateSpaceRequest_impl(m_soapRequest, m_request, response);
	if(response->srmStatusOfUpdateSpaceRequestResponse != NULL && response->srmStatusOfUpdateSpaceRequestResponse->returnStatus != NULL )
	{
		m_status = response->srmStatusOfUpdateSpaceRequestResponse->returnStatus->statusCode;
		if(response->srmStatusOfUpdateSpaceRequestResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmStatusOfUpdateSpaceRequestResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::StatusUpdateSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::StatusUpdateSpaceRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::StatusUpdateSpaceRequest::buildResponse()", "called.\n");
	//fake
}
