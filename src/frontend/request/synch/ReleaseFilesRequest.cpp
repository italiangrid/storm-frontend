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

#include "ReleaseFilesRequest.hpp"
#include "srmlogit.h"

void storm::ReleaseFilesRequest::load(ns1__srmReleaseFilesRequest* request) throw (storm::invalid_request)
{
	if (request->requestToken == NULL && (request->arrayOfSURLs == NULL || request->arrayOfSURLs->__sizeurlArray == 0 || request->arrayOfSURLs->urlArray == NULL)) {
		throw storm::invalid_request("SpaceToken and SURLs array are NULL");
	}
	if (request->arrayOfSURLs != NULL && request->arrayOfSURLs->__sizeurlArray > 0 && request->arrayOfSURLs->urlArray != NULL) {
		for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
			m_surls.insert(std::string(request->arrayOfSURLs->urlArray[i]));
		}
	}
	if (request->requestToken != NULL)
	{
		m_requestToken = std::string(request->requestToken);
	}
	if (request->doRemove != NULL)
	{
		m_doRemove = convertBoolean(*(request->doRemove));
	}
}

int storm::ReleaseFilesRequest::performXmlRpcCall(ns1__srmReleaseFilesResponse_* response){
	char *funcName = "ReleaseFilesRequest::performXmlRpcCall()";
	int ret = ns1__srmReleaseFiles_impl(m_soapRequest, m_request, response);
	if(response->srmReleaseFilesResponse != NULL && response->srmReleaseFilesResponse->returnStatus != NULL )
	{
		m_status = response->srmReleaseFilesResponse->returnStatus->statusCode;
		if(response->srmReleaseFilesResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmReleaseFilesResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::ReleaseFilesRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::ReleaseFilesRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::ReleaseFilesRequest::buildResponse()", "called.\n");
	//fake
}