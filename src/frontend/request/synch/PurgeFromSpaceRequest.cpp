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

#include "PurgeFromSpaceRequest.hpp"
#include "srmlogit.h"

void storm::PurgeFromSpaceRequest::load(ns1__srmPurgeFromSpaceRequest* request) throw (storm::invalid_request)
{
	if (request->spaceToken == NULL) {
		throw storm::invalid_request("targetSpaceToken is NULL");
	}
	if (request->arrayOfSURLs == NULL || request->arrayOfSURLs->__sizeurlArray == 0 || request->arrayOfSURLs->urlArray == NULL) {
		throw storm::invalid_request("SURLs array is NULL");
	}
	m_spaceToken = std::string(request->spaceToken);
	for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
		m_surls.insert(std::string(request->arrayOfSURLs->urlArray[i]));
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

int storm::PurgeFromSpaceRequest::performXmlRpcCall(ns1__srmPurgeFromSpaceResponse_* response){
	char *funcName = "PurgeFromSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmPurgeFromSpace_impl(m_soapRequest, m_request, response);
	if(response->srmPurgeFromSpaceResponse != NULL && response->srmPurgeFromSpaceResponse->returnStatus != NULL )
	{
		m_status = response->srmPurgeFromSpaceResponse->returnStatus->statusCode;
		if(response->srmPurgeFromSpaceResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmPurgeFromSpaceResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::PurgeFromSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::PurgeFromSpaceRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::PurgeFromSpaceRequest::buildResponse()", "called.\n");
	//fake
}
