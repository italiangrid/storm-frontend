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

#include "UpdateSpaceRequest.hpp"
#include "srmlogit.h"

void storm::UpdateSpaceRequest::load(ns1__srmUpdateSpaceRequest* request) throw (storm::invalid_request)
{
	if(request->spaceToken == NULL)
	{
		throw storm::invalid_request("Received NULL spaceToken in the request");
	}
	m_spaceToken = std::string(request->spaceToken);
	if(request->newSizeOfTotalSpaceDesired != NULL)
	{
		m_newSizeOfTotalSpaceDesired = *(request->newSizeOfTotalSpaceDesired);
	}
	if(request->newSizeOfGuaranteedSpaceDesired != NULL)
	{
		m_newSizeOfGuaranteedSpaceDesired = *(request->newSizeOfGuaranteedSpaceDesired);
	}
	if(request->newLifeTime != NULL)
	{
		m_newLifeTime = *(request->newLifeTime);
	}
	if(request->storageSystemInfo != NULL && request->storageSystemInfo->__sizeextraInfoArray > 0 && request->storageSystemInfo->extraInfoArray != NULL)
	{
		for(int i = 0; i < request->storageSystemInfo->__sizeextraInfoArray; ++i)
		{
			m_extraInfo.insert(std::make_pair(request->storageSystemInfo->extraInfoArray[i]->key,
					request->storageSystemInfo->extraInfoArray[i]->value));
		}
	}
}

int storm::UpdateSpaceRequest::performXmlRpcCall(ns1__srmUpdateSpaceResponse_* response){
	char *funcName = "UpdateSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmUpdateSpace_impl(m_soapRequest, m_request, response);
	if(response->srmUpdateSpaceResponse != NULL && response->srmUpdateSpaceResponse->returnStatus != NULL )
	{
		m_status = response->srmUpdateSpaceResponse->returnStatus->statusCode;
		if(response->srmUpdateSpaceResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmUpdateSpaceResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::UpdateSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::UpdateSpaceRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::UpdateSpaceRequest::buildResponse()", "called.\n");
	//fake
}
