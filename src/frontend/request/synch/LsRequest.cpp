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


#include "LsRequest.hpp"
#include "srmlogit.h"

void storm::LsRequest::load(ns1__srmLsRequest* request) throw (storm::invalid_request)
{
	if (NULL == request->arrayOfSURLs) {
		throw storm::invalid_request("SURLs array is NULL");
	}
	if (0 == request->arrayOfSURLs->__sizeurlArray || request->arrayOfSURLs->urlArray == NULL) {
		throw storm::invalid_request("SURLs array is empty");
	}

	for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
		m_surls.insert(std::string(request->arrayOfSURLs->urlArray[i]));
	}
	if(request->fileStorageType != NULL)
	{
		m_fileStorageType = *request->fileStorageType;
	}
	if(request->fullDetailedList != NULL)
	{
		m_fullDetailedList = *request->fullDetailedList;
	}
	if(request->allLevelRecursive != NULL)
	{
		m_allLevelRecursive= *request->allLevelRecursive;
	}
	if(request->numOfLevels != NULL)
	{
		m_numOfLevels = *request->numOfLevels;
	}
	if(request->offset != NULL)
	{
		m_offset = *request->offset;
	}
	if(request->count != NULL)
	{
		m_count = *request->count;
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

int storm::LsRequest::performXmlRpcCall(ns1__srmLsResponse_* response){
	char *funcName = "LsdirRequest::performXmlRpcCall()";
	int ret = ns1__srmLs_impl(m_soapRequest, m_request, response);
	if(response->srmLsResponse != NULL && response->srmLsResponse->returnStatus != NULL )
	{
		m_status = response->srmLsResponse->returnStatus->statusCode;
		if(response->srmLsResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmLsResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::LsRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::LsRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::LsRequest::buildResponse()", "called.\n");
	//fake
}
