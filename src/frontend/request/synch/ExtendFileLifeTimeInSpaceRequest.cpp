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

#include "ExtendFileLifeTimeInSpaceRequest.hpp"
#include "srmlogit.h"

void storm::ExtendFileLifeTimeInSpaceRequest::load(ns1__srmExtendFileLifeTimeInSpaceRequest* request) throw (storm::invalid_request)
{
	if (request->spaceToken == NULL) {
		throw storm::invalid_request("targetSpaceToken is NULL");
	}
	m_spaceToken = std::string(request->spaceToken);
	if (request->arrayOfSURLs != NULL && request->arrayOfSURLs->__sizeurlArray > 0 && request->arrayOfSURLs->urlArray != NULL) {
		for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
			m_surls.insert(std::string(request->arrayOfSURLs->urlArray[i]));
		}
	}
	if (request->newLifeTime != NULL) {
		m_newLifeTime = *(request->newLifeTime);
	}
}

int storm::ExtendFileLifeTimeInSpaceRequest::performXmlRpcCall(ns1__srmExtendFileLifeTimeInSpaceResponse_* response){
	char *funcName = "ExtendFileLifeTimeInSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmExtendFileLifeTimeInSpace_impl(m_soapRequest, m_request, response);
	if(response->srmExtendFileLifeTimeInSpaceResponse != NULL && response->srmExtendFileLifeTimeInSpaceResponse->returnStatus != NULL )
	{
		m_status = response->srmExtendFileLifeTimeInSpaceResponse->returnStatus->statusCode;
		if(response->srmExtendFileLifeTimeInSpaceResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmExtendFileLifeTimeInSpaceResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::ExtendFileLifeTimeInSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::ExtendFileLifeTimeInSpaceRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::ExtendFileLifeTimeInSpaceRequest::buildResponse()", "called.\n");
	//fake
}
