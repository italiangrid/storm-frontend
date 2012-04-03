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

#include "StatusReserveSpaceRequest.hpp"
#include "srmlogit.h"

void storm::StatusReserveSpaceRequest::load(ns1__srmStatusOfReserveSpaceRequestRequest* request) throw (storm::invalid_request)
{
	if(request->requestToken == NULL)
	{
		throw storm::invalid_request("Received NULL requestToken in the request");
	}
	m_requestToken = std::string(request->requestToken);
}

int storm::StatusReserveSpaceRequest::performXmlRpcCall(ns1__srmStatusOfReserveSpaceRequestResponse_* response){
	char *funcName = "StatusReserveSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmStatusOfReserveSpaceRequest_impl(m_soapRequest, m_request, response);
	if(response->srmStatusOfReserveSpaceRequestResponse != NULL && response->srmStatusOfReserveSpaceRequestResponse->returnStatus != NULL )
	{
		m_status = response->srmStatusOfReserveSpaceRequestResponse->returnStatus->statusCode;
		if(response->srmStatusOfReserveSpaceRequestResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmStatusOfReserveSpaceRequestResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::StatusReserveSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::StatusReserveSpaceRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::StatusReserveSpaceRequest::buildResponse()", "called.\n");
	//fake
}
