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

#include "GetSpaceMetaDataRequest.hpp"
#include "srmlogit.h"

void storm::GetSpaceMetaDataRequest::load(ns1__srmGetSpaceMetaDataRequest* request) throw (storm::invalid_request)
{
	if (NULL == request->arrayOfSpaceTokens) {
		throw storm::invalid_request("SpaceTokens array is NULL");
	}
	if (0 == request->arrayOfSpaceTokens->__sizestringArray || request->arrayOfSpaceTokens->stringArray == NULL) {
		throw storm::invalid_request("SpaceTokens array is empty");
	}

	for (int i = 0; i < request->arrayOfSpaceTokens->__sizestringArray; ++i) {
		m_spaceTokens.insert(std::string(request->arrayOfSpaceTokens->stringArray[i]));
	}
}

int storm::GetSpaceMetaDataRequest::performXmlRpcCall(ns1__srmGetSpaceMetaDataResponse_* response){
	char *funcName = "GetSpaceMetaDataRequest::performXmlRpcCall()";
	int ret = ns1__srmGetSpaceMetaData_impl(m_soapRequest, m_request, response);
	if(response->srmGetSpaceMetaDataResponse != NULL && response->srmGetSpaceMetaDataResponse->returnStatus != NULL )
	{
		m_status = response->srmGetSpaceMetaDataResponse->returnStatus->statusCode;
		if(response->srmGetSpaceMetaDataResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmGetSpaceMetaDataResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::GetSpaceMetaDataRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::GetSpaceMetaDataRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::GetSpaceMetaDataRequest::buildResponse()", "called.\n");
	//fake
}
