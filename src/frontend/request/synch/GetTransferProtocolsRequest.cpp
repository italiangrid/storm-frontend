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


#include "GetTransferProtocolsRequest.hpp"
#include "srmlogit.h"

void storm::GetTransferProtocolsRequest::load(ns1__srmGetTransferProtocolsRequest* request) throw (storm::invalid_request)
{
}

int storm::GetTransferProtocolsRequest::performXmlRpcCall(ns1__srmGetTransferProtocolsResponse_* response){
	char *funcName = "GetTransferProtocolsRequest::performXmlRpcCall()";
	int ret = ns1__srmGetTransferProtocols_impl(m_soapRequest, m_request, response);
	if(response->srmGetTransferProtocolsResponse != NULL && response->srmGetTransferProtocolsResponse->returnStatus != NULL )
	{
		m_status = response->srmGetTransferProtocolsResponse->returnStatus->statusCode;
		if(response->srmGetTransferProtocolsResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmGetTransferProtocolsResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::GetTransferProtocolsRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::GetTransferProtocolsRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::GetTransferProtocolsRequest::buildResponse()", "called.\n");
	//fake
}
